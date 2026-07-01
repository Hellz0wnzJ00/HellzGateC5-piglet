# node_c5 — ESP32-C5 scan node firmware (not yet built)

Fork of Piglet's `PigletNode`. Keeps the dual-band Wi-Fi + BLE scan engine;
swaps the ESP-Now comms layer for **I²C-slave** so the node answers the master's
reads on the shared bus.

Each node is flashed with a fixed `#define NODE_ID n` (1..9) matching its
silkscreen slot; its I²C address = `0x10 + NODE_ID`. The node fills
`common/protocol.h`'s `ScanRecord` per observation and reports its die
temperature in its status.

**TODO:** import PigletNode.ino, replace comms layer, fill ScanRecord, add
die-temp reporting.
