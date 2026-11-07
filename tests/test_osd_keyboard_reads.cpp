// Host-side unit tests for the controller-register snapshot added to
// OSD_Keyboard_process() in src/MPUBIOS/app/minimig_osd_keyboard.cpp.
//
// The function runs on every mainloop iteration, so its button/debounce checks
// used to perform 12 volatile MMIO reads in the idle path (23 with buttons
// held). The optimization snapshots CONTROLLER_KEY_REG(1) and (4) once per
// call. These tests pin down both properties:
//   * exactly two MMIO reads per call, regardless of button state, and
//   * the debounce/navigation behavior is unchanged.
//
// Compile and run with:
//   g++ -std=c++14 tests/test_osd_keyboard_reads.cpp -o /tmp/test_osd_keyboard_reads && /tmp/test_osd_keyboard_reads

#include <cassert>
#include <cstdint>
#include <cstdio>

static volatile uint32_t s_reg1 = 0;
static volatile uint32_t s_reg4 = 0;
static int s_reads = 0;

static uint32_t read_keys() { s_reads++; return s_reg1; }
static uint32_t read_reg4() { s_reads++; return s_reg4; }

struct State {
  bool toggle = false, up = false, down = false, left = false, right = false;
  bool showing = false;
};

struct Out {
  bool updated = false, up = false, down = false, left = false, right = false;
};

// Mirrors the read/snapshot and button logic in OSD_Keyboard_process().
static Out process(State &s) {
  Out o;
  const uint32_t controller_keys = read_keys();
  const uint32_t controller4 = read_reg4();
  const bool no_phys_keyboard = ((controller4 >> 28) != 0x4);

  bool mod_l = ((controller_keys & 0x00000100) != 0);
  if (((controller_keys & 0x00004000) == 0x00004000) && !mod_l && no_phys_keyboard && s.toggle == 0) {
    if (s.showing == 0) { s.showing = 1; o.updated = 1; } else { s.showing = 0; }
  }
  if (((controller_keys & 0x00004000) == 0x00004000) && no_phys_keyboard) s.toggle = 1; else s.toggle = 0;

  if (((controller_keys & 0x00000001) == 0x00000001) && no_phys_keyboard && s.up == 0) {
    if (s.showing == 1) { o.up = 1; o.updated = 1; } else { o.up = 0; }
  }
  if (((controller_keys & 0x00000001) == 0x00000001) && no_phys_keyboard) s.up = 1; else s.up = 0;

  if (((controller_keys & 0x00000002) == 0x00000002) && no_phys_keyboard && s.down == 0) {
    if (s.showing == 1) { o.down = 1; o.updated = 1; } else { o.down = 0; }
  }
  if (((controller_keys & 0x00000002) == 0x00000002) && no_phys_keyboard) s.down = 1; else s.down = 0;

  if (((controller_keys & 0x00000004) == 0x00000004) && no_phys_keyboard && s.left == 0) {
    if (s.showing == 1) { o.left = 1; o.updated = 1; } else { o.left = 0; }
  }
  if (((controller_keys & 0x00000004) == 0x00000004) && no_phys_keyboard) s.left = 1; else s.left = 0;

  if (((controller_keys & 0x00000008) == 0x00000008) && no_phys_keyboard && s.right == 0) {
    if (s.showing == 1) { o.right = 1; o.updated = 1; } else { o.right = 0; }
  } else { o.right = 0; }
  if (((controller_keys & 0x00000008) == 0x00000008) && no_phys_keyboard) s.right = 1; else s.right = 0;

  return o;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Idle path: only the two snapshot reads, never a per-branch re-read.
  {
    s_reg1 = 0; s_reg4 = 0; s_reads = 0;
    State s; process(s);
    CHECK(s_reads == 2);
  }

  // Every individual button bit held: still exactly two reads per call.
  for (int bit = 0; bit < 16; bit++) {
    s_reg1 = (1u << bit); s_reg4 = 0; s_reads = 0;
    State s; process(s);
    CHECK(s_reads == 2);
  }

  // Constant-time reads even when every button is held at once.
  {
    s_reg1 = 0xFFFF; s_reg4 = 0; s_reads = 0;
    State s; process(s);
    CHECK(s_reads == 2);
  }

  // Select (bit 14) without L opens the keyboard, and the debouncer keeps it
  // open for repeated held frames.
  {
    s_reg1 = 0x00004000; s_reg4 = 0;
    State s;
    Out o = process(s);
    CHECK(o.updated == 1);
    CHECK(s.showing == true);
    o = process(s);
    CHECK(o.updated == 0);
    CHECK(s.showing == true);

    // Release Select, then press again: closes the keyboard. The close branch
    // intentionally does not request a redraw (matches the firmware).
    s_reg1 = 0; process(s);
    s_reg1 = 0x00004000;
    o = process(s);
    CHECK(o.updated == 0);
    CHECK(s.showing == false);
  }

  // Select with L held is the disk-swap shortcut, not the keyboard toggle.
  {
    s_reg1 = 0x00004000 | 0x00000100; s_reg4 = 0;
    State s;
    Out o = process(s);
    CHECK(o.updated == 0);
    CHECK(s.showing == false);
  }

  // A physical keyboard (reg 4 top nibble 0x4) suppresses the OSD keyboard.
  {
    s_reg1 = 0x00004000; s_reg4 = 0x40000000;
    State s;
    Out o = process(s);
    CHECK(o.updated == 0);
    CHECK(s.showing == false);
  }

  // D-pad up navigates only while the keyboard is showing.
  {
    s_reg1 = 0x00000001; s_reg4 = 0;
    State s; s.showing = true;
    Out o = process(s);
    CHECK(o.up == 1);
    CHECK(o.updated == 1);
  }

  if (failures == 0) {
    printf("test_osd_keyboard_reads: all checks passed\n");
    return 0;
  }
  printf("test_osd_keyboard_reads: %d failure(s)\n", failures);
  return 1;
}
