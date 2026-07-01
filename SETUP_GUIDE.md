# HellzGate C5 — Setup Guide

**Status: living document, in progress.** Sections are marked below as they
get confirmed. This grows alongside the build — nothing here is written
until it's actually been done or verified, so this doubles as a build log.

HellzGate C5 is forked from [Piglet](https://github.com/Hamspiced/piglet)
by Hamspiced / Midwest Gadgets LLC, used and adapted with permission. See
`FORK_STYLE_GUIDE.md` for how fork-specific changes are tracked in code.

> **Note:** this guide covers flashing, wiring for development/testing, and
> using the firmware. It intentionally does not describe the custom PCB's
> internal design (socket layout, power circuitry, component selection).
> PCB design files are kept separate from this repo.

---

## 1. Supported Roles

**Status: confirmed**

HellzGate C5 firmware runs in one of two roles, both on Seeed XIAO ESP32-C5:

- **Master** — full peripheral set (OLED, SD, GPS, button), aggregates
  scan data from nodes over ESP-NOW, logs to SD in WiGLE CSV format, serves
  the web UI
- **Node** — no peripherals required, runs `PigletNode.ino`, scans an
  assigned channel range and forwards results to the Master

GPS is not required for basic operation — the firmware runs fine without
it, just without coordinate-tagged logs.

## 2. What You'll Need (development/testing)

**Status: partial**

| Part | Status | Notes |
|---|---|---|
| XIAO ESP32-C5 | On hand | one per unit you're bringing up |
| SSD1306 OLED (I2C) | TODO | for Master unit testing only |
| SD card module | TODO | for Master unit testing only |
| Momentary button | TODO | for Master unit testing only |
| GPS module (ATGM336H or similar) | Deferred | UART, 3.3V logic |
| Breadboard + jumper wires | On hand | for Master unit dev wiring |

Node units need no peripherals — USB power only.

## 3. Toolchain Setup

**Status: not yet done**

- [ ] Arduino IDE 2.x installed
- [ ] ESP32 board package v3.0+ installed
- [ ] Board selected: `XIAO_ESP32C5`
- [ ] PSRAM: OPI PSRAM
- [ ] Partition scheme: Huge APP (3MB No OTA/1MB SPIFFS)
- [ ] Stock Piglet compiles and flashes successfully (toolchain sanity check,
      before any custom code)

## 4. Master Unit — Dev/Test Wiring

**Status: diagram ready, not yet physically built**

For breadboard development and testing (not describing the production PCB):
see `master_wiring_diagram_v1.svg` and `master_wiring_reference.md` for a
standard SSD1306 + SD module + button wiring reference against the XIAO
C5's native I2C/SPI pins. GPS omitted for now (v2).

Everything powered from the XIAO's 3.3V rail, not 5V — see the reference
doc for why (logic-level safety on the GPS RX line, once that's added).

- [ ] Physically wired on breadboard
- [ ] Power-on test — confirm no shorts before flashing
- [ ] Stock Piglet boots, OLED/SD respond correctly

## 5. Node Units — Flashing

**Status: not yet done**

- [ ] `PigletNode.ino` flashed to first test unit
- [ ] Confirms boot + ESP-NOW `CORE_REQUEST` broadcast over Serial Monitor
- [ ] Repeat for remaining node units once confirmed working

## 6. Mesh / Channel-Split Validation

**Status: fix applied in code, not yet hardware-tested**

`coreReassignChannels()` in `MeshNode.cpp` was patched to distribute the
scan channel table evenly across however many nodes are connected (see
CHANGELOG v0.1). Needs real-hardware confirmation:

- [ ] Bring up nodes one at a time (not all at once)
- [ ] Confirm channel ranges reported over Serial match expected splits as
      node count increases (see CHANGELOG for the math)
- [ ] Pull a node's power mid-run, confirm timeout drops it and reassigns
      remaining nodes correctly

## 7. Theme / Branding

**Status: built and merged into source, hardware-unverified**

- [x] Boot splash: "HellzGate" wordmark bitmap (`hellzgate_bitmaps.h` ->
      `hellzgate_splash_bmp`), flashes briefly before Piglet's own credit
      splash plays, unmodified
- [x] Glitch easter egg: bitmaps built (`hellzgate_skull_bmp`), trigger
      state-machine code not yet written
- [x] Web UI retheme: red/black palette, corner brackets, scanline overlay
      — see `docs/webui-preview.html` for a static showcase
- [x] Default AP SSID rebranded
- [ ] Physically confirm splash/theme render correctly on real OLED
      hardware (bitmap previews on a monitor is not the same as the real
      panel's contrast/glow)

## 8. First Full Boot (All Peripherals + Mesh)

**Status: not started — depends on sections 4-6 above**

- [ ] Master unit: OLED + SD + button + mesh Master mode, all working
      together
- [ ] Node units joined and channel-split confirmed
- [ ] Full end-to-end test: nodes scanning, master aggregating, SD logging
      WiGLE-format CSVs

## 9. GPS Integration (v2)

**Status: deferred**

- [ ] GPS module selected
- [ ] Wired in
- [ ] Confirmed GPS fix appears in OLED status and logged CSVs

---

## Appendix: Reference documents in this repo

- `FORK_STYLE_GUIDE.md` — how fork changes are tracked/tagged
- `CHANGELOG.md` — "HellzGate C5 Fork Changes" section at the top
- `master_wiring_diagram_v1.svg` / `master_wiring_reference.md` — dev/test
  wiring reference (not the production PCB)
- `docs/webui-preview.html` — static showcase of the web UI theme
- `hellzgate_bitmaps.h` — OLED bitmap assets
