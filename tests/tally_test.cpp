// Host test for the real network_tally.h (no duplicate copy).
// Build: g++ -std=c++17 -Wall -Wextra -O2 tally_test.cpp -o tally_test && ./tally_test
#include <cstdio>
#include <cstring>
#include <cassert>
#include "../common/protocol.h"
#include "../firmware/master_c5/network_tally.h"

static ScanRecord mk(uint8_t last,uint8_t band,uint8_t node,uint8_t type){
  ScanRecord r; memset(&r,0,sizeof(r));
  r.bssid[0]=0xAA; r.bssid[5]=last; r.band=band; r.nodeId=node; r.type=type;
  return r;
}
int main(){
  printf("sizeof(ScanRecord)=%zu\n", sizeof(ScanRecord));
  NetworkTally t;
  t.ingest(mk(0x01,BAND_24,0,TYPE_AP));
  t.ingest(mk(0x01,BAND_24,1,TYPE_AP));
  t.ingest(mk(0x01,BAND_24,2,TYPE_AP));
  t.ingest(mk(0x02,BAND_24,0,TYPE_AP));
  t.ingest(mk(0x02,BAND_24,0,TYPE_AP));
  t.ingest(mk(0x03,BAND_24,4,TYPE_AP));
  t.ingest(mk(0x10,BAND_5,0,TYPE_AP));
  t.ingest(mk(0x11,BAND_5,3,TYPE_AP));
  t.ingest(mk(0x10,BAND_5,3,TYPE_AP));
  t.ingest(mk(0x40,BAND_24,0,TYPE_BLE));
  t.ingest(mk(0x40,BAND_24,5,TYPE_BLE));
  t.ingest(mk(0x41,BAND_24,5,TYPE_BLE));
  t.ingest(mk(0x50,BAND_24,0,TYPE_CLIENT));
  t.ingest(mk(0x50,BAND_24,1,TYPE_CLIENT));

  TallyCounts n=t.snapshot();
  assert(n.totalHits==9);
  assert(n.uniqueAPs==5);
  assert(n.unique24==3);
  assert(n.unique5==2);
  assert(n.bleHits==3);
  assert(n.uniqueBLE==2);
  assert(n.clientHits==2);
  assert(n.uniqueClients==1);
  assert(n.perNodeHits[0]==6);
  assert(n.perNodeHits[5]==2);
  t.reset(); n=t.snapshot();
  assert(n.totalHits==0 && n.uniqueBLE==0 && n.uniqueAPs==0 && n.perNodeHits[0]==0);
  printf("TALLY (with BLE) ALL PASSED\n");
  return 0;
}
