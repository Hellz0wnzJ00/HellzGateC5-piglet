# master_c5 — master controller firmware

Coordinates the nine scan nodes over I²C, aggregates + GPS-stamps data, logs
WiGLE CSV to SD, serves the web UI, controls the fan.

## Built & tested (host)
- `network_tally.h` — single-pass total+unique counting, BLE/client dedup,
  per-node counters. Same header builds on device and in host tests.
- `fan_control.h`  — hottest-node auto fan (hysteresis, safety force-on) with
  tier-2 fault detection.

See `../../tests/` for the host tests that exercise these.

## TODO
- I²C ingest task (poll nodes, fill queue, track last-seen + temp)
- GPS UART + stamping
- SD CSV logging + WiGLE/WDGW upload
- OLED glance render
- Responsive web UI

Uses `../../common/protocol.h`.
