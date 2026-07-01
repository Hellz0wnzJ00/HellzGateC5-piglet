#pragma once
#include <Arduino.h>
#include "PinMapDefs.h"

struct Config {
  String wigleBasicToken;
  String homeSsid;
  String homePsk;
  // === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
  // Was: wardriverSsid = "Piglet-WARDRIVE", wardriverPsk = "wardrive1234"
  // Now: rebranded defaults. Still override-able via wardriver.cfg — change
  //      the password before real deployment, this is just the fallback.
  // ===================================================================
  String wardriverSsid = "HellzGate-WARDRIVE";
  String wardriverPsk  = "hellzg8te";
  uint32_t gpsBaud     = 9600;
  String scanMode      = "aggressive"; // aggressive | powersaving
  String board = "auto"; // auto | s3 | c5 | c6 | exp  (pins selected at boot; reboot required after change)
  String speedUnits  = "kmh"; // kmh | mph
  int battPin        = -1;    // GPIO for battery voltage ADC (-1 = disabled). Expects 1:2 voltage divider from LiPo.
  bool batteryTest   = false; // Enable battery test (logs elapsed time to /battery_test.csv)
  
  // Boot auto-upload limit:
  //  -1 = upload ALL files at boot (no limit)
  //   0 = disabled (no auto-upload at boot)
  //  1+ = upload up to N files at boot (WiGLE allows 25 API calls/day)
  // IMPORTANT: Requires PSRAM enabled in Arduino IDE for reliable TLS connections.
  int maxBootUploads = 25;

  // WDGoWars API key from https://wdgwars.pl/profile -> "Generate API key".
  // If set, CSVs are uploaded to WDGoWars BEFORE WiGLE at every boot.
  // Leave empty to disable WDGoWars uploads.
  String wdgwarsApiKey;

  // Optional device name — appended to WiGLE CSV header and filename so
  // multiple Piglets uploading to the same account can be distinguished.
  // E.g. deviceName=rover1  →  device=Piglet-rover1  /  rover1_Piglet_WiGLE_....csv
  // Leave empty for default ("Piglet-Wardriver" / "WiGLE_....csv").
  String deviceName;

  // Auto-start mesh mode after boot uploads: core, node, or none (default).
  // core — become the mesh coordinator (receives wardriving records from nodes).
  // node — become a scanning node that forwards records to the Core.
  // none — normal solo wardriving mode.
  // === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
  // Was: "none" (stock Piglet default — solo wardriving).
  // Now: "core" — a HellzGate master's entire job is coordinating its node
  // array, so it should boot ready to do that with zero config required.
  String meshModeOnBoot = "core";

  // === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
  // ESP-NOW mesh encryption key (PMK). Must be identical on the master AND
  // every node, or encrypted peers won't be able to communicate at all.
  // Padded/truncated to exactly 16 bytes internally — see
  // MeshNode.cpp buildEspNowPmk(). Change this default before real
  // deployment; it's a fallback, not a secret.
  String espnowKey = "HellzGateMeshKey";

  // === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
  // Fan control mode: auto (on while scanning, off when paused), on
  // (always on), off (always off). GPIO is on/off only — no PWM or
  // tachometer feedback on this board's fan circuit.
  String fanMode = "auto";

  // Rotate the OLED display 180° (true = upside-down mount, false = normal).
  // Requires reboot to take effect.
  bool rotateScreen180 = false;

  // When true: after boot uploads complete, disconnect from home WiFi and
  // begin wardriving immediately instead of staying on the STA connection.
  // The web UI is still reachable if you connect to the Wardriver AP later,
  // but the device will not hold the STA link open. Requires reboot.
  bool autoStartAfterUpload = false;
};

const PinMap& detectPinsByChip();
PinMap pickPinsFromConfig();
bool wardriverIsC5();

String trimCopy(String s);
bool parseKeyValueLine(const String& lineIn, String& keyOut, String& valOut);
void cfgAssignKV(const String& k, const String& v);
bool loadConfigFromSD();
bool saveConfigToSD();
