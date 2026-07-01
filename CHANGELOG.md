# Changelog

## HellzGate C5 Fork Changes

HellzGate C5 is forked from [Piglet](https://github.com/Hamspiced/piglet) by
Hamspiced / Midwest Gadgets LLC, used and adapted with permission. Everything
below is specific to this fork. Inline code changes are tagged
`HELLZGATE FORK CHANGE` — search for that string to find exact lines:

```
grep -rn "HELLZGATE FORK CHANGE" .
```

Upstream Piglet's own changelog is preserved below this section, untouched.

### Mesh Networking

- **Channel-split load balancing** (`MeshNode.cpp`, `coreReassignChannels()`).
  Stock Piglet's channel-splitting math dumped the entire remainder onto the
  last registered node — e.g. at 9 nodes, 8 nodes got 4 channels each while
  the 9th got 8, roughly doubling its scan-cycle time. HellzGate spreads the
  remainder one-per-node across the first N nodes instead, so every node's
  channel count stays within 1 of every other (9 nodes → four nodes at 5
  channels, five nodes at 4).
- **ESP-NOW mesh encryption (PMK/LMK).** Stock Piglet's mesh traffic is
  unencrypted. HellzGate adds a configurable 16-byte pre-shared key
  (`espnowKey` in Config/web UI), applied via `esp_now_set_pmk()` on both
  master and nodes, with real unicast peer connections encrypted using it as
  their LMK. Broadcast frames stay unencrypted since ESP-NOW cannot encrypt
  broadcast/multicast at all (protocol limitation, not a gap) — broadcast is
  only ever used for the initial discovery handshake, never scan data. Node
  firmware (`PigletNode.ino`) has no SD/config storage, so its key is a
  compile-time constant that must be manually matched to the master's
  configured key before flashing.
- **Per-node scan mode — data model only, not yet functional.** Added a
  `scanMode` field (WiFi / BLE / Both) to the master's per-node registry and
  the node-assignment wire protocol, defaulting to WiFi for every node
  (current behavior unchanged). The actual BLE scan loop and web UI control
  to set this per node are not yet built — this is config plumbing only,
  laid ahead of that work.

### Hardware Compatibility

- **SPI OLED fallback, dormant by default.** The board's OLED module
  (Wisevision X096-2864KSWPG01-H30, SSD1315 controller) supports both I2C
  and SPI; schematic analysis suggests this board's CS#/RES#/D/C# pins are
  wired for SPI rather than the I2C mode stock Piglet's firmware assumes.
  Added a compile-time toggle (`HELLZGATE_OLED_SPI` in `Globals.h`) that
  switches the display driver's interface — off by default, so current
  behavior is unchanged until real hardware testing confirms which mode
  this board actually needs.

### Branding / Theme

- **Boot splash** — "HellzGate" wordmark bitmap replaces Piglet's text
  splash as the first thing shown on boot; Piglet's own credit splash still
  plays immediately after, untouched.
- **Web UI** — retheme to a dark red/graphite palette with a built-in theme
  switcher (Graphite default, Crimson, Classic/stock-Piglet-teal, Amber),
  plus a corner-bracket/scanline visual treatment matching the board's
  branding. Page title, favicon, and header text rebranded. Preview
  (`docs/webui-preview.html`) also reworked into a responsive grid layout
  with collapsible sections instead of one long scrolling column.
- **Default AP SSID** rebranded to `HellzGate-WARDRIVE`.
- **Glitch-skull easter egg — assets built, not yet wired up.** A skull
  bitmap (`hellzgate_skull_bmp`) exists in `hellzgate_bitmaps.h` as a
  planned replacement for Piglet's pig-themed animation easter eggs, but the
  trigger/animation state machine itself hasn't been written yet.

### Documentation

- `FORK_STYLE_GUIDE.md` — conventions for tracking fork-specific changes
  going forward.
- `SETUP_GUIDE.md` — living build/setup log, hardware-architecture details
  intentionally omitted (kept in a separate, non-public repo).
- `docs/webui-preview.html` — standalone visual showcase of the web UI theme.

---

# Upstream Piglet Changelog

## v2.57 (2026-06-24)

### New Features
- **Auto-Start Wardriving After Uploads** (`autoStartAfterUpload`): new config option that disconnects from home Wi-Fi immediately after boot uploads complete and begins scanning without delay. Previously the device held the STA connection open, which paused scanning until the link dropped naturally. Configurable via web UI or `wardriver.cfg`. Disabled by default.

  > **Note:** Once enabled, the web UI is not reachable on the home network after boot (device disconnects immediately after uploading). To disable it, either power on away from the home network so the Wardriver AP broadcasts — connect to it and visit `http://192.168.4.1` — or remove the SD card and set `autoStartAfterUpload=false` in `wardriver.cfg` directly.

### Bug Fixes
- **Mesh node mode on S3 / C6**: nodes no longer attempt to scan 5 GHz channels (36–177) on 2.4 GHz-only hardware. Previously those scan attempts failed silently and wasted ~80 ms each per cycle; the node now skips channels > 14 when not running on a C5.

### T-Dongle C5
- Synced mesh WiFi init fix: `enterCoreMode()` and `enterNodeMode()` now use `WiFi.mode(WIFI_OFF) → WIFI_STA` (full deinit/reinit) instead of `WiFi.disconnect`. Matches the XIAO fix that restored Core/Node connectivity.
- Added `[CORE] RX CORE_REQUEST` diagnostic print in `jcmkOnRecv` and channel-verification prints in both enter functions.

---

## v1.3-beta (2026-02-23)

### New Features
- **WiGLE Upload History Tracking**: Web UI now displays upload statistics (new networks discovered, total networks) for uploaded files
- **Automatic Boot Upload with Quota Management**: Configurable `maxBootUploads` setting (default: 25) to control how many files upload automatically at boot
- **24-Hour History Caching**: Upload history API calls are cached for 24 hours to conserve WiGLE API quota (25 calls/day limit)
- **On-Demand History Refresh**: History automatically refreshes in web UI when cache expires (only when connected to home network)

### Improvements
- **Optimized Upload Performance**: Removed token pre-checks and reduced timeouts for faster batch uploads
- **Enhanced WiFi Stability**: Scanning now properly pauses when connected to home network to prevent connection drops
- **Web Server Startup Timing**: Web server now starts after WiGLE operations complete to avoid resource conflicts
- **Improved Configuration Management**: Added `maxBootUploads` and `speedUnits` configuration options
- **Better Status Display**: Config form in web UI now properly displays all saved values including WiGLE token

### Bug Fixes
- Fixed scanning interference causing 100% ping loss when connected to home WiFi
- Fixed web UI configuration display issues (all fields now populate correctly)
- Fixed chunked encoding errors in `/status.json` and `/files.json` endpoints
- Corrected WiGLE token display (now shows actual token instead of "(set)")
- Fixed JSON buffer overflow issues in files endpoint

### Technical Changes
- Increased JSON buffer for files endpoint from 4KB to 8KB to handle upload statistics
- Switched from HTTP/1.1 to HTTP/1.0 for WiGLE API compatibility
- Added proper `client.flush()` to ensure complete data transmission
- Reduced upload timeout from 60s to 25s for better reliability
- History parsing now uses incremental JSON parsing to reduce memory fragmentation

### Configuration
- New config option: `maxBootUploads` - Max CSV files to upload at boot (0-25, default: 25)
- Updated config option: `speedUnits` - Display speed in km/h or mph
- Config file now saves `maxBootUploads` setting to `/wardriver.cfg`

### Requirements
- **CRITICAL**: PSRAM must be enabled in Arduino IDE for reliable TLS/HTTPS uploads
  - ESP32-C5/C6: Use OPI PSRAM
  - ESP32-S3: Use QSPI PSRAM
- Arduino-ESP32 core v3.0.0 or later
- Updated library dependencies documented in README

### Known Issues
- ESP32-C5/C6 require PSRAM enabled or TLS connections will fail due to insufficient heap
- Initial boot may show "Failed to allocate dummy cacheline for PSRAM" warning (can be ignored)

### Migration Notes
- No breaking changes from v1.2
- Existing `/wardriver.cfg` files are compatible
- New `maxBootUploads` setting will default to 25 if not present in config

---

## v1.2 (Previous Release)
- Initial stable release with basic wardriving functionality
- SD card CSV logging
- Web UI for file management
- Manual WiGLE upload support
