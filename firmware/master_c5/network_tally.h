// network_tally.h — HellzGate C5
// Single-pass tally for the master's extraction task.
//
// Every incoming ScanRecord is fed through ingest() exactly once. From that
// one pass we get BOTH numbers:
//   * totalHits   — every logged observation (a big number; multiple nodes
//                   seeing the same AP each bump it). This is your "scanned"
//                   / collection-volume count.
//   * uniqueAPs   — distinct BSSIDs, deduped. Matches what WDGW credits as
//                   `captured`. This is your real "networks found" count.
// plus the per-band unique split and per-node hit counters for the OLED
// node-status page.
//
// Threading model (matches the all-C5, single-core reality):
//   - ONE task (the extraction task) calls ingest(). It is the only writer.
//   - The OLED task and web-UI task call snapshot() to read a consistent copy.
//   The heavy work (hash-set insert) happens OUTSIDE the critical section;
//   only the small scalar counters are touched under a short spinlock, so
//   readers never see a half-updated set of numbers.
//
// Derived from / adapted from Hamspiced/Piglet (CC BY-NC 4.0).

#pragma once
#include <unordered_set>
#include <string.h>
#include "protocol.h"

// On the ESP32 the FreeRTOS spinlock guards the scalar counters. On a plain
// host build (unit tests, single-threaded) we stub it so the SAME header
// compiles unchanged — no separate test copy that could drift.
#if defined(ESP_PLATFORM) || defined(ARDUINO)
  #include "freertos/FreeRTOS.h"
#else
  typedef int portMUX_TYPE;
  #ifndef portMUX_INITIALIZER_UNLOCKED
  #define portMUX_INITIALIZER_UNLOCKED 0
  #endif
  static inline void portENTER_CRITICAL(portMUX_TYPE*) {}
  static inline void portEXIT_CRITICAL(portMUX_TYPE*) {}
#endif

// Plain-old-data snapshot handed to display / web UI. Copyable, no locks held
// by the caller. Read these freely.
struct TallyCounts {
  uint32_t totalHits;                        // all Wi-Fi AP observations logged
  uint32_t uniqueAPs;                        // distinct Wi-Fi BSSIDs
  uint32_t unique24;                         // distinct BSSIDs on 2.4 GHz
  uint32_t unique5;                          // distinct BSSIDs on 5 GHz
  uint32_t bleHits;                          // all BLE observations logged
  uint32_t uniqueBLE;                        // distinct BLE device MACs
  uint32_t clientHits;                       // all client observations (v2)
  uint32_t uniqueClients;                    // distinct client MACs   (v2)
  uint32_t perNodeHits[HELLZGATE_MAX_NODES]; // records seen from each node
};

class NetworkTally {
public:
  NetworkTally() { reset(); }

  // Start a fresh session: clears dedup sets and zeroes every counter.
  void reset() {
    portENTER_CRITICAL(&mux_);
    memset(&c_, 0, sizeof(c_));
    portEXIT_CRITICAL(&mux_);
    seenAP_.clear();
    seenBLE_.clear();
    seenClient_.clear();
    // Reserve up front to avoid rehash stalls mid-session. Tune to a typical
    // session size for your area. Each bucket is cheap; this just trades a
    // little RAM for smoother ingest.
    seenAP_.reserve(4096);
  }

  // Feed exactly one observation. Called only by the extraction task.
  void ingest(const ScanRecord& r) {
    if (r.type == TYPE_BLE) {
      ingestBle(r);
      return;
    }
    if (r.type == TYPE_CLIENT) {
      ingestClient(r);
      return;
    }

    // --- Wi-Fi AP path ---
    // Do the set insert BEFORE taking the lock: insert() may allocate, and we
    // must not allocate while holding a spinlock.
    const uint64_t key = packMac(r.bssid);
    const bool isNew = seenAP_.insert(key).second;

    portENTER_CRITICAL(&mux_);
    c_.totalHits++;                          // total always counts
    if (r.nodeId < HELLZGATE_MAX_NODES)
      c_.perNodeHits[r.nodeId]++;
    if (isNew) {                             // unique only on first sighting
      c_.uniqueAPs++;
      if (r.band == BAND_5) c_.unique5++;
      else                  c_.unique24++;
    }
    portEXIT_CRITICAL(&mux_);
  }

  // Consistent copy for OLED / web UI. Safe to call from any task.
  TallyCounts snapshot() {
    TallyCounts out;
    portENTER_CRITICAL(&mux_);
    out = c_;
    portEXIT_CRITICAL(&mux_);
    return out;
  }

private:
  void ingestBle(const ScanRecord& r) {
    // BLE device advertisement. Dedups by device MAC exactly like Wi-Fi APs
    // dedup by BSSID. Tracked separately so BLE and Wi-Fi counts never mix.
    const uint64_t key = packMac(r.bssid);
    const bool isNew = seenBLE_.insert(key).second;

    portENTER_CRITICAL(&mux_);
    c_.bleHits++;
    if (r.nodeId < HELLZGATE_MAX_NODES)
      c_.perNodeHits[r.nodeId]++;
    if (isNew) c_.uniqueBLE++;
    portEXIT_CRITICAL(&mux_);
  }

  void ingestClient(const ScanRecord& r) {
    // v2 feature. Inert in v1 because nodes only emit TYPE_AP records, but the
    // plumbing is here so turning on client capture later needs no rework.
    const uint64_t key = packMac(r.bssid);
    const bool isNew = seenClient_.insert(key).second;

    portENTER_CRITICAL(&mux_);
    c_.clientHits++;
    if (r.nodeId < HELLZGATE_MAX_NODES)
      c_.perNodeHits[r.nodeId]++;
    if (isNew) c_.uniqueClients++;
    portEXIT_CRITICAL(&mux_);
  }

  // A 6-byte MAC fits exactly in 48 bits, so we pack it into a uint64_t and
  // let the hash set dedup on that. Far cheaper than hashing a byte array.
  static inline uint64_t packMac(const uint8_t* m) {
    return ((uint64_t)m[0] << 40) | ((uint64_t)m[1] << 32) |
           ((uint64_t)m[2] << 24) | ((uint64_t)m[3] << 16) |
           ((uint64_t)m[4] <<  8) |  (uint64_t)m[5];
  }

  TallyCounts     c_;
  std::unordered_set<uint64_t> seenAP_;
  std::unordered_set<uint64_t> seenBLE_;
  std::unordered_set<uint64_t> seenClient_;
  portMUX_TYPE    mux_ = portMUX_INITIALIZER_UNLOCKED;
};
