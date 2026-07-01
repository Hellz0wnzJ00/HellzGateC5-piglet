# Host tests

These test the pure logic (counting/dedup and fan control) on a normal PC with
g++ — no ESP32 hardware needed. They `#include` the **real** headers from
`common/` and `firmware/master_c5/` (not copies), so they validate the actual
shipped code. On a host build the FreeRTOS spinlock and Arduino calls are
stubbed automatically inside the headers.

## Run

```sh
cd tests
g++ -std=c++17 -Wall -Wextra -O2 -I../common tally_test.cpp -o tally_test && ./tally_test
g++ -std=c++17 -Wall -Wextra -O2 -I../common fan_test.cpp  -o fan_test  && ./fan_test
```

(`-I../common` lets `network_tally.h` find `protocol.h`.)

## Coverage

**tally_test.cpp** — total vs unique in one pass; BLE dedup separate from Wi-Fi;
client records isolated from AP counts; per-node counters across all types;
reset.

**fan_test.cpp** — AUTO hysteresis (on 75 / off ≤65); safety force-on at 90;
manual on/off (off yields to safety); tier-2 fault detection (working vs dead
fan, fault clears on cooling / fan-off).
