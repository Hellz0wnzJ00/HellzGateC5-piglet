#pragma once
#include <Arduino.h>

// ================================================================
//  MeshNode — JCMK-compatible ESP-Now wardriving node (page 5)
//  Compatible with justcallmekoko/ESP32DualBandWardriver core role.
// ================================================================

// JCMK channel table (shared so Display can show range labels)
extern const uint8_t JCMK_ESPNOW_CH;
extern const uint8_t JCMK_NUM_CHANNELS;
extern const uint8_t JCMK_CHANNELS[];

// Node state (read from Display.cpp to render page 5)
extern bool     meshNodeActive;
extern bool     jcmkHaveCore;
extern uint8_t  jcmkCoreMac[6];
extern uint8_t  jcmkAssignVer;
extern uint8_t  jcmkStartIdx;
extern uint8_t  jcmkEndIdx;
extern uint32_t jcmkNetworksFound;
extern uint32_t jcmkSentCount;

// Lifecycle — called from Piglet.ino on page enter/exit
void enterNodeMode();
void exitNodeMode();

// Loop tick — call every loop() iteration while on page 5
void nodeModeTick();

// OLED page renderer — called from Display.cpp updateOLED() (handles both modes)
void drawPageMeshNode();

// ================================================================
//  MeshCore — Core (coordinator) role
//  Long-press on page 5 while in Node mode activates Core mode;
//  long-press again returns to Node mode.
// ================================================================

#define CORE_MAX_NODES 12

// === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
// Was: no per-node scan-mode concept; every node always WiFi-scanned.
// Now: each node carries a configurable scanMode (WiFi/BLE/Both), sent to
//      it via jcmk_admin_msg_t alongside its channel range. Defaults to
//      HZ_SCAN_WIFI for every node — behavior is unchanged unless a user
//      explicitly changes it via the web UI (Phase 2, not yet built).
//      Actual BLE scanning on the node side is Phase 3/4, not yet built —
//      this is just the config plumbing.
// ===================================================================
enum HzScanMode : uint8_t {
  HZ_SCAN_WIFI = 0,   // default — matches all current/existing behavior
  HZ_SCAN_BLE  = 1,
  HZ_SCAN_BOTH = 2
};

struct CoreNodeInfo {
  bool     active;
  uint8_t  mac[6];
  uint8_t  startIdx;   // index into JCMK_CHANNELS[]
  uint8_t  endIdx;
  uint32_t lastHbMs;
  uint32_t recordsRx;
  bool     isBiscuit;  // true = Biscuit Node protocol (requires full-size 212-byte packets)
  uint8_t  scanMode;   // HzScanMode — defaults to HZ_SCAN_WIFI on registration
};

// Core state (read from Display.cpp for page 5 rendering)
extern bool          meshCoreActive;
extern uint32_t      coreRecordsRx;
extern uint8_t       coreNodeCount;
extern CoreNodeInfo  coreNodes[CORE_MAX_NODES];

void enterCoreMode();
void exitCoreMode();
void coreModeTick();
