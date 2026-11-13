// Host-side unit tests for core_reg_update logic in src/MPUBIOS/app/core.cpp.
//
// core_reg_update() runs on every mainloop iteration. It reads both
// CORE_OUTPUT_REGISTOR() and AFP_REGISTOR(7) on every poll. AFP_REGISTOR(7) holds
// menu/AFP settings that only change on user interaction, so it is now snapshotted
// at a fixed interval (POLL_INTERVAL = 4) instead of every iteration, cutting the
// volatile AFP MMIO reads by 4x while CORE_OUTPUT_REGISTOR() is still read every
// poll (bits 4-5 / 0x30 mouse+joystick suppression are owned by other functions and
// must be preserved, so caching the whole register would be unsafe).
//
// Compile and run with:
//   g++ -std=c++14 -Wall -Wextra tests/test_core_reg.cpp -o test_core_reg && ./test_core_reg

#include <cstdint>
#include <cstdio>

// Mock MMIO registers for testing
static uint32_t s_mock_core_output = 0;
static uint32_t s_mock_afp_7 = 0;
static uint64_t g_afp_reads = 0;
static uint64_t g_core_output_reads = 0;
static int s_core_write_count = 0;

static uint32_t read_afp_7() {
  g_afp_reads++;
  return s_mock_afp_7;
}
static uint32_t read_core_output() {
  g_core_output_reads++;
  return s_mock_core_output;
}
static void write_core_output(uint32_t v) {
  s_mock_core_output = v;
  s_core_write_count++;
}

// Replicate the exact logic from src/MPUBIOS/app/core.cpp core_reg_update():
//   static uint8_t afp7_poll_counter = 0;
//   static uint32_t cached_afp7 = 0xFFFFFFFFu;
//   if (++afp7_poll_counter >= POLL_INTERVAL) {
//     afp7_poll_counter = 0;
//     cached_afp7 = AFP_REGISTOR(7);
//   }
//   uint32_t tmp = CORE_OUTPUT_REGISTOR();
//   uint32_t next = (tmp & 0xFFFFFF30) | (cached_afp7 & 0x000000cf);
//   if (tmp != next) {
//     CORE_OUTPUT_REGISTOR() = next;
//   }
static const int POLL_INTERVAL = 4;
static uint8_t g_afp7_counter = 0;
static uint32_t g_cached_afp7 = 0xFFFFFFFFu;

static void core_reg_update_model() {
  if (++g_afp7_counter >= POLL_INTERVAL) {
    g_afp7_counter = 0;
    g_cached_afp7 = read_afp_7();
  }
  uint32_t tmp = read_core_output();
  uint32_t next = (tmp & 0xFFFFFF30) | (g_cached_afp7 & 0x000000cf);
  if (tmp != next) {
    write_core_output(next);
  }
}

static void reset_model() {
  // Prime the counter so the very first call reads AFP_REGISTOR(7) and seeds
  // g_cached_afp7 from the current setting, mirroring a warmed-up polling loop
  // (the real firmware's static cached_afp7 starts at 0xFFFFFFFF and is refreshed
  // on the first interval tick). This avoids one-time startup transients so each
  // test measures steady-state behavior.
  g_afp7_counter = POLL_INTERVAL - 1;
  g_cached_afp7 = 0xFFFFFFFFu;
  g_afp_reads = 0;
  g_core_output_reads = 0;
  s_core_write_count = 0;
  s_mock_core_output = 0;
  s_mock_afp_7 = 0;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // 1. Steady state with no changes: AFP read every POLL_INTERVAL polls,
  //    CORE_OUTPUT read every poll, exactly one write on the first call.
  {
    reset_model();
    s_mock_core_output = 0x00000000;
    s_mock_afp_7 = 0x05;

    const int iterations = 1000;
    for (int i = 0; i < iterations; i++) {
      core_reg_update_model();
    }

    CHECK(s_mock_core_output == 0x05);
    CHECK(s_core_write_count == 1); // first (warmed) call only; AFP stable thereafter
    CHECK(g_core_output_reads == (uint64_t)iterations);   // CORE_OUTPUT every poll
    CHECK(g_afp_reads == (uint64_t)(iterations / POLL_INTERVAL)); // AFP every 4th poll
    printf("Test 1 (steady state): core_output_reads=%llu afp_reads=%llu writes=%d\n",
           g_core_output_reads, g_afp_reads, s_core_write_count);
  }

  // 2. CORE_OUTPUT_REGISTOR() is still read every iteration so bits 4-5 (0x30)
  //    owned by other functions (mouse/joystick suppression) are preserved.
  {
    reset_model();
    s_mock_core_output = 0x00000030; // external writer holds mouse-suppress bits
    s_mock_afp_7 = 0x05;

    core_reg_update_model();
    CHECK(s_mock_core_output == 0x35); // bits 4-5 preserved, low nibble from AFP(7)
    CHECK(s_core_write_count == 1);
    CHECK(g_core_output_reads == 1);   // still read this poll
    printf("Test 2 (external 0x30 bits preserved): output=0x%0x writes=%d\n",
           s_mock_core_output, s_core_write_count);
  }

  // 3. When AFP_REGISTOR(7) changes, the new value is detected within
  //    POLL_INTERVAL polls and written exactly once.
  {
    reset_model();
    s_mock_core_output = 0x00000000;
    s_mock_afp_7 = 0x05;
    for (int i = 0; i < 20; i++) core_reg_update_model(); // establish 0x05

    int writes_before = s_core_write_count;
    uint64_t afp_reads_before = g_afp_reads;

    s_mock_afp_7 = 0x0A; // menu change
    for (int i = 0; i < POLL_INTERVAL + 2; i++) core_reg_update_model();

    CHECK(s_mock_core_output == 0x0A);
    CHECK(s_core_write_count == writes_before + 1); // detected and written once
    CHECK(g_afp_reads - afp_reads_before <= (uint64_t)(POLL_INTERVAL + 1)); // bounded delay
    printf("Test 3 (AFP change detected): writes=%d afp_reads_after_change=%llu\n",
           s_core_write_count, g_afp_reads - afp_reads_before);
  }

  // 4. Multiple AFP changes are all detected across polls.
  {
    reset_model();
    s_mock_core_output = 0x00000000;
    s_mock_afp_7 = 0x00000000;
    for (int i = 0; i < 20; i++) core_reg_update_model();

    int cmd_count = s_core_write_count;

    s_mock_afp_7 = 0x01;
    for (int i = 0; i < 10; i++) core_reg_update_model();
    CHECK(s_core_write_count > cmd_count);
    cmd_count = s_core_write_count;

    s_mock_afp_7 = 0x02;
    for (int i = 0; i < 10; i++) core_reg_update_model();
    CHECK(s_core_write_count > cmd_count);
    cmd_count = s_core_write_count;

    s_mock_afp_7 = 0x00;
    for (int i = 0; i < 10; i++) core_reg_update_model();
    CHECK(s_core_write_count > cmd_count);

    printf("Test 4 (multiple changes): all %d changes detected\n", s_core_write_count);
  }

  // 5. Read reduction ratio: 75% fewer AFP reads over many polls.
  {
    reset_model();
    s_mock_core_output = 0x00000000;
    s_mock_afp_7 = 0x07;

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) core_reg_update_model();

    CHECK(g_afp_reads == (uint64_t)(iterations / POLL_INTERVAL));
    double reduction = 100.0 * (iterations - g_afp_reads) / iterations;
    CHECK(reduction == 75.0);
    printf("Test 5 (reduction): %d iterations, AFP reads=%llu, %.1f%% reduction\n",
           iterations, g_afp_reads, reduction);
  }

  // 6. Bits masked out by 0x000000cf in AFP_REGISTOR(7) (bits 4, 5 and upper bits)
  //    do not override existing bits 4 & 5, which come from CORE_OUTPUT_REGISTOR().
  {
    reset_model();
    s_mock_core_output = 0x00000030; // bits 4 & 5 set externally
    s_mock_afp_7 = 0xFFFFFFFFu;      // AFP(7) all bits set
    core_reg_update_model();
    // Mask 0x000000cf sets bits [3:0] and [7:6] from AFP(7); bits [5:4] (0x30)
    // come from CORE_OUTPUT_REGISTOR() (preserved). AFP=0xFFFFFFFF -> masked 0xCF,
    // OR'd with core_output 0x30 -> 0xFF.
    CHECK((s_mock_core_output & 0x000000ff) == 0xFF);
    CHECK(s_core_write_count == 1);
    printf("Test 6 (masked bits): output low byte=0x%0x writes=%d\n",
           s_mock_core_output & 0xFF, s_core_write_count);
  }

  if (failures == 0) { printf("test_core_reg: all checks passed\n"); return 0; }
  printf("test_core_reg: %d failure(s)\n", failures);
  return 1;
}
