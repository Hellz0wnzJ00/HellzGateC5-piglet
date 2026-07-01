#include "Globals.h"
#include <Wire.h>

// ---- Global objects ----
// SPI/I2C OLED branch controlled by HELLZGATE_OLED_SPI — see Globals.h for
// the full explanation and the CS/DC/RST placeholder values.
PinMap pins = PINS_C6; // default until detected/configured
Config cfg;
#ifdef HELLZGATE_OLED_SPI
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI,
                            HELLZGATE_OLED_DC, HELLZGATE_OLED_RST, HELLZGATE_OLED_CS);
#else
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#endif
TinyGPSPlus gps;
HardwareSerial GPSSerial(1);
WebServer server(80);

// ---- State flags ----
bool sdOk = false;
bool scanningEnabled = true;
bool gpsHasFix = false;
bool allowScanForOled = false;
bool userScanOverride = false;
bool autoPaused = false;

// ---- OLED page system ----
uint8_t currentPage = 0;
bool statusPagePaused = false;

// ---- AP state ----
uint32_t apStartMs = 0;
bool apClientSeen = false;
bool apWindowActive = false;
const uint32_t AP_WINDOW_MS = 60000UL;

bool apExtended = false;
uint32_t apExtendedStartMs = 0;
bool apForceClose = false;
const uint32_t AP_EXTENDED_WINDOW_MS = 5UL * 60UL * 1000UL;  // 5 minutes
const uint32_t AP_EXTEND_PROMPT_LEAD_MS = 30000UL;            // prompt 30 s before expiry

// ---- Counters ----
uint32_t networksFound2G = 0;
uint32_t networksFound5G = 0;

// ---- Log state ----
File logFile;
String currentCsvPath;
wl_status_t lastStaStatus = WL_IDLE_STATUS;

// ---- Upload state ----
bool     uploading = false;
bool     uploadPausedScanWasEnabled = false;
uint32_t uploadTotalFiles = 0;
uint32_t uploadDoneFiles  = 0;
String   uploadCurrentFile = "";
String   uploadLastResult  = "";
String   uploadTargetName  = "";
uint32_t uploadFailedFiles = 0;
int      wigleTokenStatus  = 0;
int      wdgwarsKeyStatus  = 0;  // HELLZGATE FORK CHANGE — see CHANGELOG.md
bool     fanOn = false;          // HELLZGATE FORK CHANGE — actual current fan output state
int      wigleLastHttpCode = 0;

// ---- WiGLE constants ----
const char* WIGLE_HOST = "api.wigle.net";
const uint16_t WIGLE_PORT = 443;
