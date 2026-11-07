// Host-side tests for the controller-register snapshot in
// update_keyboard_inputs() (src/MPUBIOS/drivers/KMIO/inputs.cpp).
//
// The six HID report bytes used to be re-read from CONTROLLER_JOY_REG(3) and
// CONTROLLER_TRIG_REG(3) inside the per-slot loop (36 volatile MMIO reads), and
// the modifier block read CONTROLLER_KEY_REG(3) eight more times. The function
// now snapshots the three registers once per call. These tests pin down that
// read count and the key press/release and modifier edge detection.
//
// Compile and run with:
//   g++ -std=c++14 tests/test_keyboard_scan.cpp -o /tmp/test_keyboard_scan && /tmp/test_keyboard_scan

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

static volatile uint32_t s_joy3 = 0;
static volatile uint32_t s_trig3 = 0;
static volatile uint32_t s_key3 = 0;
static int s_reads = 0;

static uint32_t rd(volatile uint32_t &r) { s_reads++; return r; }

struct State {
  uint8_t buffer[6];
  bool lalt, ralt, lshift, rshift, lwin, lctrl, rctrl, rwin;
  uint8_t events[16];
  int event_count;
};

static void emit(State &s, uint8_t v) {
  assert(s.event_count < (int)sizeof(s.events));
  s.events[s.event_count++] = v;
}

static void modifiers(State &s, uint32_t k) {
  bool la = (k >> 10) & 1, ra = (k >> 14) & 1, ls = (k >> 9) & 1, rs = (k >> 13) & 1;
  bool lw = (k >> 11) & 1, lc = (k >> 8) & 1, rc = (k >> 12) & 1, rw = (k >> 15) & 1;
  if (la && !s.lalt) { s.lalt = 1; emit(s, 0x11); } else if (!la && s.lalt) { s.lalt = 0; emit(s, 0x91); }
  if (ra && !s.ralt) { s.ralt = 1; emit(s, 0x12); } else if (!ra && s.ralt) { s.ralt = 0; emit(s, 0x92); }
  if (ls && !s.lshift) { s.lshift = 1; emit(s, 0x13); } else if (!ls && s.lshift) { s.lshift = 0; emit(s, 0x93); }
  if (rs && !s.rshift) { s.rshift = 1; emit(s, 0x14); } else if (!rs && s.rshift) { s.rshift = 0; emit(s, 0x94); }
  if (lw && !s.lwin) { s.lwin = 1; emit(s, 0x15); } else if (!lw && s.lwin) { s.lwin = 0; emit(s, 0x95); }
  if ((lc || rc) && !s.lctrl) { s.lctrl = 1; emit(s, 0x16); } else if (!(lc || rc) && s.lctrl) { s.lctrl = 0; emit(s, 0x96); }
  if (rw && !s.rwin) { s.rwin = 1; emit(s, 0x17); } else if (!rw && s.rwin) { s.rwin = 0; emit(s, 0x97); }
}

// Mirrors the snapshot + press/release search in update_keyboard_inputs().
// Byte values 0..0xFD map to themselves; 0xFE/0xFF are "unknown".
static void process(State &s) {
  const uint32_t joy3 = rd(s_joy3);
  const uint32_t trig3 = rd(s_trig3);
  const uint32_t key3 = rd(s_key3);
  uint8_t updated[6] = {0};
  uint8_t input[6] = {
      (uint8_t)((joy3 >> 24) & 0xff), (uint8_t)((joy3 >> 16) & 0xff),
      (uint8_t)((joy3 >> 8) & 0xff),  (uint8_t)(joy3 & 0xff),
      (uint8_t)((trig3 >> 8) & 0xff), (uint8_t)(trig3 & 0xff),
  };

  for (int j = 0; j < 6; j++) {
    bool failed = true;
    for (int i = 0; i < 6; i++) {
      if ((input[j] == s.buffer[i]) && input[j] != 0) { failed = false; break; }
      if (input[j] == 0) { failed = false; break; }
    }
    updated[j] = input[j];
    if (failed && input[j] != 0xFE && input[j] != 0xFF) emit(s, input[j]);
  }
  for (int j = 0; j < 6; j++) {
    bool failed = true;
    for (int i = 0; i < 6; i++) {
      if ((s.buffer[j] == updated[i]) && s.buffer[j] != 0) { failed = false; break; }
      if (s.buffer[j] == 0) { failed = false; break; }
    }
    if (failed && s.buffer[j] != 0xFE && s.buffer[j] != 0xFF) emit(s, 0x80 | s.buffer[j]);
  }
  memcpy(s.buffer, updated, 6);
  modifiers(s, key3);
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

static void reset_io() {
  s_joy3 = 0; s_trig3 = 0; s_key3 = 0; s_reads = 0;
}

int main() {
  State s;

  // Exactly three MMIO reads: joy report, trigger report, modifier register.
  {
    reset_io();
    memset(&s, 0, sizeof(s));
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 0);
  }

  // A newly pressed key in the last trigger slot is reported once.
  {
    reset_io();
    memset(&s, 0, sizeof(s));
    s_trig3 = 0x00000042;
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 1 && s.events[0] == 0x42);
  }

  // Holding the same report re-sends nothing (no repeat MMIO reads either).
  {
    s_reads = 0; s.event_count = 0;
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 0);
  }

  // Releasing the key sends press|0x80 once.
  {
    reset_io();
    s_trig3 = 0;
    s.event_count = 0;
    process(s);
    CHECK(s.event_count == 1 && s.events[0] == (0x80 | 0x42));
  }

  // Four simultaneous new keys in the report are all reported once.
  {
    reset_io();
    memset(&s, 0, sizeof(s));
    s_joy3 = 0x01020304u;
    s_trig3 = 0x05060000u;
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 4);
    CHECK(s.events[0] == 0x01 && s.events[1] == 0x02 && s.events[2] == 0x03 && s.events[3] == 0x04);
  }

  // Modifier bits come from the single key3 snapshot: press on the rising
  // edge, release on the falling edge, and nothing while held.
  {
    reset_io();
    memset(&s, 0, sizeof(s));
    s_key3 = (1u << 8) | (1u << 10) | (1u << 12);  // Ctrl, LAlt, RCtrl
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 2);  // LAlt + combined Ctrl
    CHECK(s.events[0] == 0x11 && s.events[1] == 0x16);

    s_key3 = (1u << 8) | (1u << 10) | (1u << 12);
    s_reads = 0; s.event_count = 0;
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 0);

    s_key3 = 0;
    s_reads = 0; s.event_count = 0;
    process(s);
    CHECK(s_reads == 3);
    CHECK(s.event_count == 2);
    CHECK(s.events[0] == 0x91 && s.events[1] == 0x96);
  }

  if (failures == 0) { printf("test_keyboard_scan: all checks passed\n"); return 0; }
  printf("test_keyboard_scan: %d failure(s)\n", failures);
  return 1;
}
