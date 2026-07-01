// Host test for the real fan_control.h (no duplicate copy).
// Build: g++ -std=c++17 -Wall -Wextra -O2 fan_test.cpp -o fan_test && ./fan_test
#include <cstdio>
#include <cassert>
#include "../firmware/master_c5/fan_control.h"

int main(){
  FanControl f; f.configure(75,65,90,FAN_AUTO);
  assert(f.decide(50)==false); assert(f.decide(74.9f)==false);
  assert(f.decide(75)==true);  assert(f.decide(70)==true);
  assert(f.decide(65.1f)==true); assert(f.decide(65)==false);
  assert(f.decide(74)==false); assert(f.decide(75)==true);
  FanControl s; s.configure(75,65,90,FAN_AUTO); assert(s.decide(90)==true);
  FanControl o; o.configure(75,65,90,FAN_FORCE_OFF);
  assert(o.decide(80)==false); assert(o.decide(95)==true);
  FanControl nn; nn.configure(75,65,90,FAN_FORCE_ON); assert(nn.decide(20)==true);
  printf("Tier 1 regression: still passes  OK\n");

  FanControl w; w.configure(75,65,90,FAN_AUTO,60000);
  uint32_t t=0;
  assert(w.evaluate(78.0f,t)==true); assert(w.suspectFault()==false);
  t=30000; w.evaluate(74.0f,t); assert(w.suspectFault()==false);
  t=61000; w.evaluate(70.0f,t); assert(w.suspectFault()==false);
  printf("Working fan: no fault  OK\n");

  FanControl d; d.configure(75,65,90,FAN_AUTO,60000);
  t=0; d.evaluate(76.0f,t); assert(d.suspectFault()==false);
  t=30000; d.evaluate(78.0f,t); assert(d.suspectFault()==false);
  t=61000; d.evaluate(81.0f,t); assert(d.suspectFault()==true);
  printf("Dead fan: fault flagged  OK\n");
  t=90000; d.evaluate(70.0f,t); assert(d.suspectFault()==false);
  printf("Fault clears on cooling  OK\n");

  FanControl d2; d2.configure(75,65,90,FAN_AUTO,60000);
  t=0; d2.evaluate(76.0f,t); t=61000; d2.evaluate(80.0f,t); assert(d2.suspectFault()==true);
  t=62000; d2.evaluate(60.0f,t); assert(d2.suspectFault()==false);
  printf("Fault clears when fan off  OK\n");
  printf("ALL FAN TIER-1 + TIER-2 TESTS PASSED\n");
  return 0;
}
