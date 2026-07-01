# HellzGate C5 — Architecture & Design

Detailed design record for HellzGate C5, a permission-based fork/adaptation of
[Piglet](https://github.com/Hamspiced/piglet) (Hamspiced, CC BY-NC 4.0). Captures
hardware, firmware split, decisions, and built-vs-planned status.

## 1. Hardware
Single PCB, ten XIAO-form-factor slots: 1 master ("MASTER ESP") + 9 scan nodes
(ESP-1..ESP-9). This build is all XIAO ESP32-C5, master included. The C5 is
single-core, so the master's scan/extract split runs as two FreeRTOS tasks
time-slicing on one core (rather than two physical cores). Nodes are XIAO
ESP32-C5 (dual-band 2.4/5 GHz Wi-Fi 6 + BLE; 5 GHz needs the C5 radio).
Master peripherals: NEO-6M GPS (UART), SSD1306 OLED, SD (SPI), USB-C, and a
2-wire 30×30 fan via an AP22802 load switch (enable on master GPIO D7). Each node
slot has a power/enable slide switch and a Schottky diode on its 5 V rail.

### Why wired I²C, not ESP-Now
Piglet uses ESP-Now because its nodes are separate devices. HellzGate's nodes sit
on one board on a shared I²C bus, so a wired bus is correct here: ESP-Now would
lock each node's radio to one channel and suspend scanning, whereas wired I²C
leaves every radio free to channel-hop across 2.4/5 GHz + BLE full-time.
Master = I²C controller; nodes = I²C peripherals.
Risk: ESP32 I²C slave mode is finicky and one wedged node can stall the bus →
per-node poll timeout + the per-node power switches mitigate.

## 2. Node identity
Node slots are wired identically, so identity is assigned at flash time:
`#define NODE_ID n` (1..9) per silkscreen slot; I²C addr = 0x10 + NODE_ID.
`ScanRecord.nodeId` / `perNodeHits[]` are 0-indexed (0 = ESP-1). Label each board.
Optional friendly `nodeName1..9` (config) shown in the web UI instead of the MAC.

## 3. Firmware split
Node: scan → buffer → answer I²C reads; reports die temp; no GPS/SD/OLED/UI.
Master: ingest/coordination (I²C poll → queue) and extraction (dedup/tally →
GPS-stamp → CSV → OLED/UI). A FreeRTOS queue is the boundary.

## 4. Counting (network_tally.h) — BUILT & TESTED
One pass yields both: totalHits (every observation) and uniqueAPs (deduped);
plus unique24/unique5, bleHits/uniqueBLE, clientHits/uniqueClients (v2), and
perNodeHits[9]. MACs pack into uint64_t for cheap dedup. Single writer; readers
use snapshot(). CSV logs every observation (WDGW files repeats as `updated` for
positioning) — dedup governs only displayed counts. Memory: comfortable to ~5k
unique in SRAM; PSRAM/bloom beyond ~8–10k.

## 5. Fan (fan_control.h) — BUILT & TESTED
Auto on hottest node, hysteresis (default on 75 / off 65 °C), safety force-on at
90 °C (C5 has NO thermal self-shutdown → fan is the only protection). Modes
auto/on/off. Tier-2: flags a fan commanded on that isn't cooling (default 60 s
window). Inference from temperature, not RPM. Car note: cabin heat eats headroom;
no fan beats a sealed hot car — the on-screen temp is the warning.

## 6. Display — fixed always-on glance screen
Headless board (no button): one screen, no pages, no sleep. Top strip = temp +
fan icon (filled=running, blink=suspect fault). WiFi/BLE unique heroes, hits +
GPS/SD, nine node dots (filled=alive, hollow=dropped, position=slot). Reads
straight from TallyCounts + fan state. Detail lives in the web UI.

## 7. Web UI (settled, not built)
One responsive page, mobile-first (SoftAP field use), also fine on laptop.
Responsive is free at runtime (phone lays out; master serves same bytes).
Bake in: drop Font Awesome → inline SVG; paginate the network table; gzip assets.
Live numbers via a small JSON poll of TallyCounts. Node table = friendly names,
MAC secondary. See docs/mockups/webui_console.html.

## 8. Build order
1) Bench-prove I²C with two boards (riskiest, do first). 2) Node fills
ScanRecord; master ingest task fills queue + node status. 3) Wire in tally + fan.
4) GPS + SD CSV. 5) OLED render. 6) Web UI + uploads.

## 9. Size / RAM
Flash isn't the constraint (8 MB, ~3.3 MB app; core+WiFi/BLE+TLS dominate; all
HellzGate logic <3 KB). RAM is (384 KB SRAM, ~150–200 KB usable heap) — the
dedup set is the thing to watch (PSRAM for big sessions).

## 10. Attribution
Fork/adaptation of Piglet by Hamspiced, with permission. See CREDITS.md and
LICENSE (CC BY-NC 4.0). Keep attribution in any further derivative.
