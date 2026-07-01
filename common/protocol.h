// protocol.h — HellzGate C5
// Shared record definition between the C5 scan nodes and the master.
//
// This lives in common/ and is #included by BOTH the node firmware and the
// master firmware so the byte layout is identical on each end.
//
// NOTE: the field set below is a clean starting point. When you paste
// PigletNode.ino we map its actual scan-result fields onto ScanRecord so the
// node fills this struct directly instead of Piglet's internal one.
//
// Derived from / adapted from Hamspiced/Piglet (CC BY-NC 4.0). Attribution
// retained in the project CREDITS.md.

#pragma once
#include <stdint.h>

// Bump this if the wire layout ever changes so a mismatched node/master pair
// can detect it instead of silently mis-parsing.
#define HELLZGATE_PROTOCOL_VERSION 1

// Number of SCAN nodes the master coordinates. The board has 10 XIAO slots:
// 1 master + 9 scan nodes (silkscreen ESP-1..ESP-9). The master is NOT counted
// here and has no dot on the OLED (if it dies, the whole display is gone).
#define HELLZGATE_MAX_NODES 9

// Node identity / addressing.
// The 9 node slots are wired identically (shared I2C bus + power), so a node
// cannot self-detect its slot from hardware. Each node is flashed with a fixed
// NODE_ID matching its physical silkscreen slot:
//   node firmware:  #define NODE_ID 4     // this board goes in slot ESP-4
// and derives its I2C slave address from that ID, e.g. I2C_ADDR = 0x10 + NODE_ID
// (0x11..0x19 for ESP-1..ESP-9; keeps clear of the OLED at 0x3C).
//
// In ScanRecord.nodeId and perNodeHits[], slots are 0-indexed: index 0 == ESP-1,
// index 8 == ESP-9. The OLED shows them 1..9. Label each physical C5 with its
// slot so a hollow node-dot points to the right board.
//   (If a future board revision straps a per-slot GPIO, this can become
//    automatic; today it is assigned at flash time.)

enum Band : uint8_t {
  BAND_24 = 0,   // 2.4 GHz
  BAND_5  = 1    // 5 GHz  (only the C5 radio produces these)
};

enum RecordType : uint8_t {
  TYPE_AP     = 0,   // a Wi-Fi access point beacon
  TYPE_BLE    = 1,   // a Bluetooth LE device advertisement
  TYPE_CLIENT = 2    // a Wi-Fi client/station MAC (v2 — client capture, off by default)
};

// One observation as produced by a node. Kept small and POD so it drops
// straight into an I2C buffer and a FreeRTOS queue with no serialization.
// Packed so sizeof() is identical on node and master regardless of compiler.
#pragma pack(push, 1)
struct ScanRecord {
  uint8_t  bssid[6];    // AP MAC (or station MAC when type==TYPE_CLIENT)
  int8_t   rssi;        // signal strength, dBm
  uint8_t  channel;     // Wi-Fi channel
  uint8_t  band;        // Band enum
  uint8_t  type;        // RecordType enum
  uint8_t  nodeId;      // which node saw it (0..HELLZGATE_MAX_NODES-1)
  char     ssid[33];    // up to 32 chars + null. May be empty for hidden APs.
};
#pragma pack(pop)

// GPS + timestamp are attached by the MASTER at ingest time, not by the node,
// so a node never needs a GPS fix. The master pairs each ScanRecord with its
// current fix when it writes the WiGLE CSV row.
