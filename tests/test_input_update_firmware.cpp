// Host-side tests that exercise the REAL input_update() (and through it the
// real update_mouse_inputs()) from src/MPUBIOS/drivers/KMIO/inputs.cpp,
// compiled here against mock APF registers (tests/firmware_support/).
//
// tests/test_mouse_inputs.cpp validates a host-side MODEL of the logic; its
// read-count assertions measure the model, so a firmware change that added
// register reads back would still pass there. This file includes the
// firmware source itself, so the assertions below measure the code that
// actually ships:
//
//   - input_update() runs on a 2ms timer interrupt (~500 polls/sec) and must
//     read the volatile AFP_REGISTOR(5) exactly once per poll (snapshotted
//     into a local), not once per use (previously up to 3 reads per poll:
//     mouse-mode check, CD32 flag, and speed extraction).
//   - The guarded CORE_OUTPUT_REGISTOR() writes fire only on state
//     transitions, not on every poll.
//
// Compile and run with:
//   g++ -std=c++14 -Wall -Wextra -Itests/firmware_support -Isrc/MPUBIOS/drivers/KMIO tests/test_input_update_firmware.cpp -o /tmp/test_input_update_firmware
//   /tmp/test_input_update_firmware

#include <cstdint>
#include <cstdio>

// ---- Mock register storage + access counters ----
static uint32_t s_afp_regs[8] = {0};
static uint32_t s_controller_key[4] = {0};
static uint32_t s_controller_joy[4] = {0};
static uint32_t s_controller_trig[4] = {0};

static uint64_t s_afp_reads = 0;
static uint64_t s_key_reads = 0;
static uint64_t s_core_output_reads = 0;
static uint64_t s_core_output_writes = 0;

uint32_t mock_afp_reg_read(int idx) {
  s_afp_reads++;
  return s_afp_regs[idx];
}
uint32_t mock_controller_key_read(int idx) {
  s_key_reads++;
  return s_controller_key[idx - 1];
}
uint32_t mock_controller_joy_read(int idx) {
  return s_controller_joy[idx - 1];
}
uint32_t mock_controller_trig_read(int idx) {
  return s_controller_trig[idx - 1];
}

static uint32_t s_core_output_storage = 0;
uint32_t mock_core_output_read(void) {
  s_core_output_reads++;
  return s_core_output_storage;
}
void mock_core_output_write(uint32_t v) {
  s_core_output_writes++;
  s_core_output_storage = v;
}

// ---- Stubs for the firmware's other dependencies ----
static uint64_t s_spi_cmd8_count = 0;
static uint64_t s_osd_disable_count = 0;
static uint64_t s_info_enable_count = 0;
static bool s_suppress_joystick = false;

void timer1Interrupts_reset(void) {}
void timer1Interrupts_setup(int ms) { (void)ms; }
uint8_t HPS_spi_uio_cmd8(uint8_t cmd, uint8_t parm) {
  (void)cmd; (void)parm;
  s_spi_cmd8_count++;
  return 0;
}
void OsdUpdate(int n) { (void)n; }
void OsdClear() {}
void OsdWrite(const char* s) { (void)s; }
void OsdDisable() { s_osd_disable_count++; }
void InfoEnable(int x, int y, int width, int height, int background_color, int text_color) {
  (void)x; (void)y; (void)width; (void)height; (void)background_color; (void)text_color;
  s_info_enable_count++;
}
void minimig_fdd_swap_disks() {}
extern "C" void pocket_amiga_shortcuts_init(void) {}
extern "C" void pocket_amiga_shortcuts_update(uint32_t c1, bool is_cd32_mode, bool osd_active) {
  (void)c1; (void)is_cd32_mode; (void)osd_active;
}
extern "C" bool pocket_amiga_shortcuts_suppress_joystick(void) { return s_suppress_joystick; }

// inputs.cpp references this keyboard lookup table, owned by another TU.
unsigned int keytable[16] = {0};

// ---- Compile the real firmware source under test ----
#ifndef INPUTS_CPP_UNDER_TEST
#define INPUTS_CPP_UNDER_TEST "../src/MPUBIOS/drivers/KMIO/inputs.cpp"
#endif
#include INPUTS_CPP_UNDER_TEST

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

static void reset_state() {
  for (int i = 0; i < 8; i++) s_afp_regs[i] = 0;
  for (int i = 0; i < 4; i++) {
    s_controller_key[i] = 0;
    s_controller_joy[i] = 0;
    s_controller_trig[i] = 0;
  }
  s_afp_reads = 0;
  s_key_reads = 0;
  s_core_output_reads = 0;
  s_core_output_writes = 0;
  s_core_output_storage = 0;
  s_spi_cmd8_count = 0;
  s_osd_disable_count = 0;
  s_info_enable_count = 0;
  s_suppress_joystick = false;
  mouse_enabled = 0;
  mouse_enabled_press = 0;
  mouse_enabled_press_old = 0;
  keyboard_osd_showing = 0;
}

int main() {
  // Test 1 (mouse mode, CD32 flag set): the REAL input_update() must read
  // AFP_REGISTOR(5) exactly once per poll. Before the snapshot optimization
  // this was up to 3 reads per poll, so this check fails on the old code.
  {
    reset_state();
    s_afp_regs[5] = 0x80000005 | 0x20000000; // mouse active (0x80), CD32 mode, speed 5

    for (int i = 0; i < 500; i++) {
      input_update();
    }
    // Exactly 500 reads for 500 polls of the firmware's input_update().
    CHECK(s_afp_reads == 500);
    // The mouse path really ran (controller reads happened inside the
    // mouse branch, not just the top-level input_update() reads).
    CHECK(s_key_reads >= 1000);
  }

  // Test 2 (non-mouse mode): still exactly one AFP_REGISTOR(5) read per poll.
  {
    reset_state();
    s_afp_regs[5] = 0x00000000; // non-mouse mode

    for (int i = 0; i < 500; i++) {
      input_update();
    }
    CHECK(s_afp_reads == 500);
  }

  // Test 3 (idle): the real input_update() performs no CORE_OUTPUT_REGISTOR()
  // writes while the joystick-suppression state is steady.
  {
    reset_state();
    for (int i = 0; i < 1000; i++) {
      input_update();
    }
    CHECK(s_core_output_writes == 0);
  }

  // Test 4 (suppression toggle): engaging joystick suppression writes bit
  // 0x10 exactly once, not on every poll; releasing writes exactly once more.
  {
    reset_state();
    s_suppress_joystick = true;
    for (int i = 0; i < 500; i++) {
      input_update();
    }
    CHECK(s_core_output_writes == 1);
    CHECK((s_core_output_storage & 0x10) == 0x10);

    s_suppress_joystick = false;
    for (int i = 0; i < 500; i++) {
      input_update();
    }
    CHECK(s_core_output_writes == 2);
    CHECK((s_core_output_storage & 0x10) == 0);
  }

  if (failures == 0) {
    printf("test_input_update_firmware: all checks passed\n");
    return 0;
  }
  printf("test_input_update_firmware: %d failure(s)\n", failures);
  return 1;
}
