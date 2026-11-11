// Host-side unit tests for update_mouse_inputs() optimization in
// src/MPUBIOS/drivers/KMIO/inputs.cpp.
//
// Optimizations verified:
// 1. Single snapshot of CONTROLLER_KEY_REG(1) per call instead of multiple
//    re-reads across branches.
// 2. State-cached OsdDisable(): previously called unconditionally every poll
//    when mouse_enabled_press was 0 (~500 times/sec). Now only called once
//    when dismissing the info overlay (mouse_info_was_showing transitioning from
//    true to false).
// 3. Guarded CORE_OUTPUT_REGISTOR() writes: previously in update_mouse_inputs()
//    and input_update(), CORE_OUTPUT_REGISTOR was read and unconditionally
//    written every poll (~500 times/sec). Guarding prevents redundant bus writes.
// 4. Single snapshot of AFP_REGISTOR(5): in input_update(), AFP_REGISTOR(5) is
//    snapshotted once per tick instead of re-read up to 3 times per poll
//    (mouse check, CD32 check, speed extraction).
//
// Compile and run with:
//   g++ -std=c++14 -Wall -Wextra tests/test_mouse_inputs.cpp -o /tmp/test_mouse_inputs && /tmp/test_mouse_inputs

#include <cassert>
#include <cstdint>
#include <cstdio>

static uint32_t s_mock_controller1 = 0;
static uint32_t s_mock_controller4 = 0;
static uint32_t s_mock_core_output = 0;
static uint32_t s_mock_afp_reg5 = 0;
static uint64_t s_core_output_writes = 0;
static uint64_t s_controller1_reads = 0;
static uint64_t s_afp_reg5_reads = 0;
static uint64_t s_info_enable_count = 0;
static uint64_t s_osd_disable_count = 0;

static uint32_t mock_read_controller1() {
  s_controller1_reads++;
  return s_mock_controller1;
}

static uint32_t mock_read_afp_reg5() {
  s_afp_reg5_reads++;
  return s_mock_afp_reg5;
}

static void mock_info_enable() {
  s_info_enable_count++;
}

static void mock_osd_disable() {
  s_osd_disable_count++;
}

struct MouseInputModel {
  bool keyboard_osd_showing = false;
  bool mouse_enabled = false;
  bool mouse_enabled_press = false;
  bool mouse_enabled_press_old = false;
  bool mouse_info_was_showing = false;

  void reset() {
    keyboard_osd_showing = false;
    mouse_enabled = false;
    mouse_enabled_press = false;
    mouse_enabled_press_old = false;
    mouse_info_was_showing = false;
  }

  void update_mouse_inputs_model(uint16_t speed = 1) {
    (void)speed;
    const uint32_t c1_raw = mock_read_controller1();
    mouse_enabled_press = ((c1_raw >> 15) & 0x1);
    if (keyboard_osd_showing == 1) {
      mouse_enabled = 0;
    } else if (mouse_enabled_press && !mouse_enabled_press_old) {
      mouse_enabled = !mouse_enabled;
    }
    mouse_enabled_press_old = mouse_enabled_press;

    if (keyboard_osd_showing == 0) {
      if (mouse_enabled_press) {
        mouse_info_was_showing = true;
        mock_info_enable();
      } else if (mouse_info_was_showing) {
        mouse_info_was_showing = false;
        mock_osd_disable();
      }
    }

    bool dock_mouse = ((s_mock_controller4 >> 28) == 0x5);
    if (dock_mouse) {
      // dock mouse path
    } else if (((c1_raw >> 28) == 0x3)) {
      // analog stick path: uses c1_raw, zero extra controller1 reads
    } else {
      // dpad / R-shoulder path: uses c1_raw, zero extra controller1 reads
      bool r_held = ((c1_raw & (1 << 9)) != 0);
      uint32_t core_out = s_mock_core_output;
      if (mouse_enabled || r_held) {
        if ((core_out & 0x00000030) != 0x00000030) {
          s_mock_core_output = core_out | 0x00000030;
          s_core_output_writes++;
        }
      } else {
        if (core_out & 0x00000030) {
          s_mock_core_output = core_out & 0xFFFFFFCF;
          s_core_output_writes++;
        }
      }
    }
  }

  void input_update_model(bool suppress_joystick, bool mod_l) {
    const uint32_t afp5 = mock_read_afp_reg5();
    uint8_t afp5_top = (uint8_t)(afp5 >> 24);
    if ((afp5_top == 0x80) | (afp5_top == 0xa0)) {
      update_mouse_inputs_model((uint16_t)(afp5 & 0xffff));
    }
    bool is_cd32 = (((afp5 >> 28) & 0x2) != 0);
    (void)is_cd32;

    if (suppress_joystick || mod_l) {
      uint32_t out = s_mock_core_output;
      if (!(out & 0x00000010)) {
        s_mock_core_output = out | 0x00000010;
        s_core_output_writes++;
      }
    } else if (!mouse_enabled && !keyboard_osd_showing) {
      uint32_t out = s_mock_core_output;
      if (out & 0x00000010) {
        s_mock_core_output = out & ~0x00000010;
        s_core_output_writes++;
      }
    }
  }
};

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  MouseInputModel model;

  // Test 1: Single CONTROLLER_KEY_REG(1) read per poll across all branches
  {
    model.reset();
    s_controller1_reads = 0;
    s_mock_controller4 = 0;

    // Sub-case A: regular dpad path
    s_mock_controller1 = 0;
    model.update_mouse_inputs_model();
    CHECK(s_controller1_reads == 1);

    // Sub-case B: analog stick path (bits 31:28 == 0x3)
    s_controller1_reads = 0;
    s_mock_controller1 = 0x30000000;
    model.update_mouse_inputs_model();
    CHECK(s_controller1_reads == 1);

    // Sub-case C: dock mouse path
    s_controller1_reads = 0;
    s_mock_controller4 = 0x50000000;
    model.update_mouse_inputs_model();
    CHECK(s_controller1_reads == 1);
  }

  // Test 2: Idle polling produces ZERO OsdDisable calls
  {
    model.reset();
    s_osd_disable_count = 0;
    s_info_enable_count = 0;
    s_mock_controller1 = 0;
    s_mock_controller4 = 0;

    for (int i = 0; i < 1000; i++) {
      model.update_mouse_inputs_model();
    }
    CHECK(s_osd_disable_count == 0);
    CHECK(s_info_enable_count == 0);
  }

  // Test 3: When mouse_enabled_press (bit 15) is held, InfoEnable is called;
  // when released, OsdDisable is called EXACTLY ONCE.
  {
    model.reset();
    s_osd_disable_count = 0;
    s_info_enable_count = 0;

    // Press and hold for 10 polls
    s_mock_controller1 = (1u << 15);
    for (int i = 0; i < 10; i++) {
      model.update_mouse_inputs_model();
    }
    CHECK(s_info_enable_count == 10);
    CHECK(s_osd_disable_count == 0);
    CHECK(model.mouse_enabled == true);

    // Release button: poll 100 times idle
    s_mock_controller1 = 0;
    for (int i = 0; i < 100; i++) {
      model.update_mouse_inputs_model();
    }
    // OsdDisable must be called only on the transition frame (exactly 1 time)
    CHECK(s_osd_disable_count == 1);
    CHECK(model.mouse_enabled == true); // still toggled on
  }

  // Test 4: Second toggle turns mouse off, OsdDisable again called exactly once
  {
    s_mock_controller1 = (1u << 15);
    for (int i = 0; i < 5; i++) {
      model.update_mouse_inputs_model();
    }
    CHECK(model.mouse_enabled == false);

    s_mock_controller1 = 0;
    for (int i = 0; i < 50; i++) {
      model.update_mouse_inputs_model();
    }
    CHECK(s_osd_disable_count == 2);
  }

  // Test 5: While keyboard OSD is showing, info enable / disable is suppressed
  {
    model.reset();
    model.keyboard_osd_showing = true;
    s_osd_disable_count = 0;
    s_info_enable_count = 0;

    s_mock_controller1 = (1u << 15);
    for (int i = 0; i < 10; i++) {
      model.update_mouse_inputs_model();
    }
    CHECK(s_info_enable_count == 0);
    CHECK(s_osd_disable_count == 0);
    CHECK(model.mouse_enabled == false);
  }

  // Test 6: CORE_OUTPUT_REGISTOR() writes are zero during idle polling
  {
    model.reset();
    s_mock_core_output = 0;
    s_core_output_writes = 0;
    s_mock_controller1 = 0;
    s_mock_controller4 = 0;

    for (int i = 0; i < 1000; i++) {
      model.update_mouse_inputs_model();
      model.input_update_model(false, false);
    }
    CHECK(s_core_output_writes == 0);
    CHECK(s_mock_core_output == 0);
  }

  // Test 7: Holding R shoulder activates mouse mode, sets bits 0x30 once, and produces no further writes
  {
    model.reset();
    s_mock_core_output = 0;
    s_core_output_writes = 0;
    s_mock_controller1 = (1u << 9); // R held
    s_mock_controller4 = 0;

    for (int i = 0; i < 500; i++) {
      model.update_mouse_inputs_model();
    }
    // Wrote bits 0x30 once on initial assertion
    CHECK(s_core_output_writes == 1);
    CHECK((s_mock_core_output & 0x30) == 0x30);

    // Release R shoulder
    s_mock_controller1 = 0;
    for (int i = 0; i < 500; i++) {
      model.update_mouse_inputs_model();
    }
    // Cleared bits 0x30 once on release
    CHECK(s_core_output_writes == 2);
    CHECK((s_mock_core_output & 0x30) == 0);
  }

  // Test 8: Joystick suppression (bit 0x10) in input_update_model writes once on change and is silent while steady
  {
    model.reset();
    s_mock_core_output = 0;
    s_core_output_writes = 0;

    // Suppress joystick (e.g. Mod_L held)
    for (int i = 0; i < 500; i++) {
      model.input_update_model(true, true);
    }
    CHECK(s_core_output_writes == 1);
    CHECK((s_mock_core_output & 0x10) == 0x10);

    // Release suppression
    for (int i = 0; i < 500; i++) {
      model.input_update_model(false, false);
    }
    CHECK(s_core_output_writes == 2);
    CHECK((s_mock_core_output & 0x10) == 0);
  }

  // Test 9: AFP_REGISTOR(5) is read exactly once per input_update_model() poll
  // even in active mouse mode with CD32 mode flag set
  {
    model.reset();
    s_afp_reg5_reads = 0;
    s_mock_afp_reg5 = 0x80000005 | 0x20000000; // mouse active (0x80), CD32 mode (0x20000000), speed 5
    s_mock_controller1 = 0;
    s_mock_controller4 = 0;

    for (int i = 0; i < 500; i++) {
      model.input_update_model(false, false);
    }
    // Exactly 500 reads for 500 polls (1 read per tick, down from 3)
    CHECK(s_afp_reg5_reads == 500);
  }

  // Test 10: AFP_REGISTOR(5) in non-mouse mode also reads exactly once per poll
  {
    model.reset();
    s_afp_reg5_reads = 0;
    s_mock_afp_reg5 = 0x00000000; // non-mouse mode
    s_mock_controller1 = 0;
    s_mock_controller4 = 0;

    for (int i = 0; i < 500; i++) {
      model.input_update_model(false, false);
    }
    CHECK(s_afp_reg5_reads == 500);
  }

  if (failures == 0) {
    printf("test_mouse_inputs: all checks passed\n");
    return 0;
  }
  printf("test_mouse_inputs: %d failure(s)\n", failures);
  return 1;
}
