# HellzGate C5

A custom multi-node wardriving / passive wireless survey platform. One master
coordinates **nine ESP32-C5 scan nodes** on a single board over a wired I²C bus,
aggregating their Wi-Fi and BLE scan data, GPS-stamping it, logging WiGLE-ready
CSVs to SD, and serving a live web UI.

> **HellzGate C5 is a permission-based fork/adaptation of
> [Piglet](https://github.com/Hamspiced/piglet) by Hamspiced**, reworked for
> HellzGate C5 cluster hardware. See [CREDITS.md](CREDITS.md). Licensed
> **CC BY-NC 4.0** (same as Piglet) — see [LICENSE](LICENSE).

## Hardware at a glance

- **1 master** (XIAO ESP32-C5) — coordinates nodes, GPS, SD, OLED, web UI, fan
- **9 scan nodes** (XIAO ESP32-C5) — dual-band 2.4/5 GHz Wi-Fi + BLE
- **Wired I²C** between master and nodes (frees node radios to scan full-time)
- Peripherals: NEO-6M GPS, 0.96" SSD1306 OLED, SD card, USB-C power, 30×30 fan

## Repository layout

```
common/            shared node<->master protocol (protocol.h)
firmware/
  master_c5/       master controller — tally + fan modules (built), rest TODO
  node_c5/         scan node firmware — TODO (fork of PigletNode)
tests/             host tests (g++, no hardware) for the built modules
docs/
  ARCHITECTURE.md  full design + decision log
  mockups/         OLED + web UI reference mockups
config.example.cfg master config template
```

## What works today (built + host-tested)

- `common/protocol.h` — shared `ScanRecord` + node-ID scheme (Wi-Fi/BLE)
- `firmware/master_c5/network_tally.h` — single-pass **total + unique** counting,
  BSSID/BLE dedup, per-node counters
- `firmware/master_c5/fan_control.h` — hottest-node auto fan (hysteresis, safety
  force-on) + fault detection
- `tests/` — compile against the real headers; both suites pass

## Not yet built (roadmap)

Node firmware, master I²C ingest task, GPS stamping, SD CSV logging, WiGLE/WDGW
upload, OLED render, responsive web UI. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Build & test

```sh
cd tests
g++ -std=c++17 -Wall -Wextra -O2 -I../common tally_test.cpp -o tally_test && ./tally_test
g++ -std=c++17 -Wall -Wextra -O2 -I../common fan_test.cpp  -o fan_test  && ./fan_test
```

Firmware builds with Arduino IDE 2.x + arduino-esp32 core **v3.3.5+** (required
for the ESP32-C5). PSRAM on, Huge APP partition.

## Status

Early. Core data-path logic is written and tested; the node link and I/O layers
are next. Bench-prove two boards over I²C before building the rest.
