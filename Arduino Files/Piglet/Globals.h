#pragma once
#define FIRMWARE_VERSION "v2.57"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include "PinMapDefs.h"
#include "Config.h"

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ================================================================
// === HELLZGATE FORK CHANGE — see CHANGELOG.md ===
// The board's OLED module (Wisevision X096-2864KSWPG01-H30, SSD1315
// controller) supports BOTH I2C and SPI. Schematic analysis suggests
// this board's CS#/RES#/D/C# pins are routed as live signals (the
// pattern for 4-wire SPI), not tied off the way a fixed-I2C module
// would be — but this needs real hardware confirmation, not just a
// schematic read. Test stock I2C init first; if Serial shows
// "[LCD] SSD1306 init FAIL at 0x3C and 0x3D", uncomment the line below,
// fill in the actual CS/DC/RST GPIO numbers for your board (confirm
// these against your own schematic — do not guess), and re-flash.
//
// MUST live in this header, not a .cpp file — Piglet.ino and Globals.cpp
// are separate translation units and only share what's in shared headers.
//
// #define HELLZGATE_OLED_SPI
//
#ifdef HELLZGATE_OLED_SPI
  #include <SPI.h>
  // TODO: confirm these three GPIO numbers against your actual board
  // before enabling. Do not trust these placeholder values.
  #define HELLZGATE_OLED_CS   -1
  #define HELLZGATE_OLED_DC   -1
  #define HELLZGATE_OLED_RST  -1
#endif
// ================================================================

// ---- Global objects ----
extern PinMap pins;
extern Config cfg;
extern Adafruit_SSD1306 display;
extern TinyGPSPlus gps;
extern HardwareSerial GPSSerial;
extern WebServer server;

// ---- State flags ----
extern bool sdOk;
extern bool scanningEnabled;
extern bool gpsHasFix;
extern bool allowScanForOled;
extern bool userScanOverride;
extern bool autoPaused;

// ---- OLED page system ----
// 0=Status, 1=Networks, 2=Navigation, 3=Pause, 4=Pig, 5=MeshNode
static const uint8_t PAGE_COUNT = 6;
extern uint8_t currentPage;
extern bool statusPagePaused;   // true when user double-pressed to pause on status page

// ---- AP state ----
extern uint32_t apStartMs;
extern bool apClientSeen;
extern bool apWindowActive;
extern const uint32_t AP_WINDOW_MS;

// Extended AP window: triggered when the first client connects during the initial
// 60 s window. Replaces the 60 s budget with a 5 min rolling timer that the WebUI
// can reset via /extend, and the user can short-circuit via /start.
extern bool apExtended;
extern uint32_t apExtendedStartMs;
extern bool apForceClose;
extern const uint32_t AP_EXTENDED_WINDOW_MS;
extern const uint32_t AP_EXTEND_PROMPT_LEAD_MS;

// ---- Counters ----
extern uint32_t networksFound2G;
extern uint32_t networksFound5G;

// ---- Log state ----
extern File logFile;
extern String currentCsvPath;
extern wl_status_t lastStaStatus;

// ---- Upload state ----
extern bool     uploading;
extern bool     uploadPausedScanWasEnabled;
extern uint32_t uploadTotalFiles;
extern uint32_t uploadDoneFiles;
extern String   uploadCurrentFile;
extern String   uploadLastResult;
extern String   uploadTargetName;   // service label shown on OLED (e.g. "WDGW UL" / "WiGLE UL")
extern uint32_t uploadFailedFiles;  // failed upload attempts in the current batch
extern int      wigleTokenStatus;
extern int      wdgwarsKeyStatus;  // HELLZGATE FORK CHANGE — see CHANGELOG.md
extern bool     fanOn;             // HELLZGATE FORK CHANGE
extern int      wigleLastHttpCode;

// ---- WiGLE constants ----
extern const char* WIGLE_HOST;
extern const uint16_t WIGLE_PORT;

// ---- Optional TLS buffer sizing (no-op for broad core compatibility) ----
inline void tlsMaybeSetBufferSizes(WiFiClientSecure&, int, int) {}
