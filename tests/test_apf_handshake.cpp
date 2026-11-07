// Host-side tests for the APF handshake polling loops in
// src/MPUBIOS/drivers/apf/apf.cpp.
//
// The loops used to OR a minimum-spin counter into the exit condition:
//     while (!(codes & FLAG) | (i <= N))
// so they always made N+1 volatile MMIO polls even when the bridge had already
// set FLAG, and the counter never bounded the wait. They now poll only until
// the flag is observed. These tests pin down that behaviour.
//
// Compile and run with:
//   g++ -std=c++14 tests/test_apf_handshake.cpp -o /tmp/test_apf_handshake && /tmp/test_apf_handshake

#include <cassert>
#include <cstdint>
#include <cstdio>

static volatile int s_poll = 0;
static int s_latency = 0;
static int s_reads = 0;

// Mock of READ_TARGET_DATASLOT_CONTROL(0): bit 0 is ACK/DONE. The bridge sets
// it on the (latency + 1)-th poll.
static uint32_t read_control() {
  s_reads++;
  s_poll = s_poll + 1;
  return (s_poll > s_latency) ? 1u : 0u;
}

// The fixed loop shape shared by all three handshake sites.
static int wait_for_flag() {
  uint32_t codes;
  do {
    codes = read_control();
  } while (!(codes & 1u));
  return s_poll;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Already-signalled flag: exactly one MMIO poll (previously 101 or 10001).
  {
    s_latency = 0; s_poll = 0; s_reads = 0;
    CHECK(wait_for_flag() == 1);
    CHECK(s_reads == 1);
  }

  // Flag arrives after a few polls: stop on the first poll that sees it.
  for (int latency = 0; latency <= 512; latency++) {
    s_latency = latency; s_poll = 0; s_reads = 0;
    int polls = wait_for_flag();
    CHECK(polls == latency + 1);
    CHECK(s_reads == latency + 1);
  }

  // A long wait still terminates as soon as the flag is observed (this is the
  // path the old minimum-spin counter could not express: >100 polls).
  {
    s_latency = 10000; s_poll = 0; s_reads = 0;
    CHECK(wait_for_flag() == 10001);
    CHECK(s_reads == 10001);
  }

  if (failures == 0) { printf("test_apf_handshake: all checks passed\n"); return 0; }
  printf("test_apf_handshake: %d failure(s)\n", failures);
  return 1;
}
