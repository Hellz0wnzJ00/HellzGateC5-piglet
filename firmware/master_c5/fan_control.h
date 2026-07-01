// fan_control.h — HellzGate C5
// Temperature-based fan control for the master.
//
// The fan runs through the AP22802 load switch (U3); its enable pin is on a
// master GPIO (D7 on the schematic). This module decides ON/OFF from the
// HOTTEST die temperature in the cluster — the master's own plus every node's
// die temp reported over I2C — so cooling tracks whichever board is working
// hardest, not just the master.
//
// IMPORTANT (from the ESP32-C5 datasheet): junction max is 125 C and the chip
// has NO thermal shutdown of its own. The fan is the only thermal protection,
// so this module includes a SAFETY force-on that overrides manual OFF.
//
// It's ON/OFF control (2-wire fan through a load switch — no PWM speed).
// Hysteresis (on/off gap) stops rapid clicking around a single temperature.
//
// Derived from / adapted from Hamspiced/Piglet (CC BY-NC 4.0).

#pragma once
#include <stdint.h>

enum FanMode : uint8_t {
  FAN_AUTO      = 0,   // temperature-based (default)
  FAN_FORCE_ON  = 1,   // always on
  FAN_FORCE_OFF = 2    // always off (safety override can still force it on)
};

class FanControl {
public:
  // Relaxed but safe defaults. All overridable from the config file.
  void configure(float tempOnC        = 75.0f,
                 float tempOffC        = 65.0f,
                 float safetyC         = 90.0f,
                 FanMode mode          = FAN_AUTO,
                 uint32_t faultWindowMs = 60000) {
    tempOn_       = tempOnC;
    tempOff_      = tempOffC;
    safety_       = safetyC;
    mode_         = mode;
    faultWindow_  = faultWindowMs;
  }

  void setMode(FanMode m) { mode_ = m; }

  // Pure decision logic — no hardware, no time, so it's unit-testable on a
  // host. Takes the hottest die temp in the cluster; returns the new fan state.
  // Uses current state for hysteresis in AUTO mode. Does NOT track faults —
  // use evaluate() for that.
  bool decide(float hottestC) {
    fanOn_ = computeOn(hottestC);
    return fanOn_;
  }

  // Tier 2: time-aware decision + fan-fault detection.
  // Same on/off result as decide(), but also watches for "commanded on yet the
  // cluster keeps heating" — i.e. the fan is enabled but not actually moving
  // air (dead fan, unplugged, seized, blown load switch). Pass millis() as
  // nowMs. When it commands the fan on it records a baseline temp; if the fan
  // is still on after faultWindow and the temp hasn't dropped below that
  // baseline, it raises suspectFault(). Clears the moment temps actually fall
  // or the fan turns off.
  //
  // Note: this is INFERENCE from temperature, not RPM proof. Extreme ambient
  // where a working fan genuinely can't keep up could false-alarm. True RPM
  // confirmation would need a 3-wire tach fan (not this hardware).
  bool evaluate(float hottestC, uint32_t nowMs) {
    const bool prevOn = fanOn_;
    fanOn_ = computeOn(hottestC);

    if (fanOn_ && !prevOn) {
      // rising edge: fan just commanded on — start the watch
      onSince_      = nowMs;
      baselineTemp_ = hottestC;
      suspectFault_ = false;
    } else if (!fanOn_) {
      // off: no fault possible
      suspectFault_ = false;
    } else {
      // on and was on: has it cooled?
      if (hottestC < baselineTemp_) {
        suspectFault_ = false;                         // temps falling -> working
      } else if ((uint32_t)(nowMs - onSince_) >= faultWindow_) {
        suspectFault_ = true;                          // on long enough, no cooling
      }
    }
    return fanOn_;
  }

  bool suspectFault() const { return suspectFault_; }

  bool isOn()      const { return fanOn_; }
  bool isSafety(float hottestC) const { return hottestC >= safety_; }
  FanMode mode()   const { return mode_; }
  float tempOn()   const { return tempOn_; }
  float tempOff()  const { return tempOff_; }
  float safety()   const { return safety_; }

private:
  // Core on/off decision (mode + hysteresis + safety). No side effects on
  // fault state — shared by decide() and evaluate().
  bool computeOn(float hottestC) const {
    bool on;
    switch (mode_) {
      case FAN_FORCE_ON:  on = true;  break;
      case FAN_FORCE_OFF: on = false; break;
      case FAN_AUTO:
      default:
        if (fanOn_) on = (hottestC > tempOff_);   // stay on until it cools
        else        on = (hottestC >= tempOn_);   // kick on at the threshold
        break;
    }
    // Safety backstop: force on above the safety temp no matter the mode,
    // because the chip won't protect itself.
    if (hottestC >= safety_) on = true;
    return on;
  }

  float    tempOn_       = 75.0f;
  float    tempOff_      = 65.0f;
  float    safety_       = 90.0f;
  FanMode  mode_         = FAN_AUTO;
  bool     fanOn_        = false;
  // Tier 2 fault detection
  uint32_t faultWindow_  = 60000;   // ms fan may be on without cooling
  uint32_t onSince_      = 0;       // when fan was last commanded on
  float    baselineTemp_ = 0.0f;    // hottest temp at that moment
  bool     suspectFault_ = false;   // set when on-but-not-cooling
};

// ---------------------------------------------------------------------------
// Arduino-only hardware glue. Guarded so the logic above compiles on a host
// for testing. On the master, call fanBegin() once, then fanApply() whenever
// you've computed the hottest cluster temp.
// ---------------------------------------------------------------------------
#ifdef ARDUINO
#include <Arduino.h>

// AP22802 EN is active-high (EN high -> fan powered). Flip if your board wires
// it inverted.
#ifndef FAN_EN_ACTIVE_HIGH
#define FAN_EN_ACTIVE_HIGH 1
#endif

inline void fanBegin(uint8_t enPin) {
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, FAN_EN_ACTIVE_HIGH ? LOW : HIGH);  // start OFF
}

// Read the master's own die temperature (Arduino-ESP32 built-in).
inline float fanReadMasterTempC() {
  return temperatureRead();
}

// Drive the enable pin to match the controller's decision.
inline void fanApply(uint8_t enPin, FanControl& fan, float hottestC) {
  bool on = fan.decide(hottestC);
  bool level = FAN_EN_ACTIVE_HIGH ? on : !on;
  digitalWrite(enPin, level ? HIGH : LOW);
}
#endif
