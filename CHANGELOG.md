# Changelog

## [0.1.0] — 2026-07-01 — Initial core data-path

First commit. Establishes the fork, the architecture, and the tested core of
the master's data path. Node link and I/O layers are scaffolded, not yet built.

### Added
- Project scaffold as a permission-based fork of Hamspiced/Piglet (CC BY-NC 4.0).
- `common/protocol.h` — shared node↔master `ScanRecord`, Wi-Fi/BLE/client record
  types, and the flash-time node-ID scheme (slots ESP-1..9, 0-indexed arrays).
- `firmware/master_c5/network_tally.h` — single-pass **total + unique** counting;
  separate BSSID / BLE / client dedup; per-band split; per-node hit counters;
  lock-guarded snapshot for display/UI. Host-compatible (same header builds on
  device and in tests).
- `firmware/master_c5/fan_control.h` — hottest-node auto fan (hysteresis, safety
  force-on at 90 °C since the C5 has no thermal self-shutdown) plus **tier-2
  fault detection** (flags a fan commanded on that isn't cooling).
- `tests/` — host tests that compile against the real headers; both suites pass.
- `config.example.cfg` — master config incl. fan keys and per-node names.
- `docs/ARCHITECTURE.md` — full design record and decision log.
- `docs/mockups/` — OLED glance screen (SVG) and web console (HTML) references.

### Decisions recorded
- Wired **I²C** between master and nodes (not ESP-Now) so node radios scan
  full-time across 2.4/5 GHz + BLE.
- **Master = ESP32-C5** (all-C5 build); single-core → scan/extract run
  as two FreeRTOS tasks.
- Display is a **fixed always-on glance screen** (headless board, no button):
  temp + fan status, WiFi/BLE unique, total hits, GPS/SD, 9 node dots.
- Web UI is **one responsive page**; node table shows **friendly names**, MAC
  secondary.
- CSV logs **every** observation (dedup governs only displayed counts).

### Not yet built
Node firmware (fork of PigletNode), master I²C ingest task, GPS stamping, SD CSV
logging, WiGLE/WDGW upload, OLED render, web UI.
