#pragma once
#include <Arduino.h>

struct PinMap {
  int sda, scl;  // OLED
  int sd_cs, sd_sck, sd_miso, sd_mosi; // SD
  int gps_rx, gps_tx; // GPS
  int btn; // Button
  int fan_en; // HELLZGATE FORK CHANGE — Fan EN (load switch), -1 = not present
  const char* name;  // "S3" / "C6" / etc
};

// --- S3 default pins (your original) ---
static const PinMap PINS_S3 = {
  5, 6,
  4, 7, 8, 9,
  44, 43,
  2,
  -1,
  "S3"
};

// --- C6 default pins ---
static const PinMap PINS_C6 = {
  22, 23,
  21, 19, 20, 18,
  17, 16,
  1,
  -1,
  "C6"
};

// --- C5 default pins (stock Piglet generic C5 — NOT this board, see below) ---
static const PinMap PINS_C5 = {
  23, 24,
  7, 8, 9, 10,
  12, 11,
  0,
  -1,
  "C5"
};

// --- S3 + Expansion Base ---
static const PinMap PINS_S3_EXP_BASE = {
  5, 6,
  3, 7, 8, 10,
  12, 11,
  2,
  -1,
  "EXP_BASE"
};

// === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
// Corrected pin map for this board specifically, confirmed against the
// actual schematic (master header: D1/TX, D2/RX, D3/CS, D8/SCLK, D9/MISO,
// D10/MOSI, D7/EN, SDA, SCL). This DIFFERS from stock PINS_C5 above in two
// places that matter: GPS is on D2/D1 here, not D6/D7 — and D7 is
// dedicated to the Fan EN line, which is the exact pin stock PINS_C5
// assigns to GPS RX. Using the generic C5 map on this board would read
// GPS from the wrong pin entirely and collide with the fan output.
//
// Button pin (btn) is NOT clearly confirmed from the schematic — D0 and
// D6 are the only unclaimed module pins, and D0 is used as a placeholder
// below. Verify against real hardware before relying on the button.
static const PinMap PINS_HELLZGATE_C5 = {
  23, 24,           // SDA, SCL (D4, D5) — same as stock
  7, 8, 9, 10,      // SD CS/SCK/MISO/MOSI (D3, D8, D9, D10) — same as stock
  25, 0,            // GPS RX, TX (D2, D1) — CORRECTED, differs from stock
  1,                // Button (D0) — UNCONFIRMED, verify against hardware
  12,               // Fan EN (D7)
  "HELLZGATE_C5"
};