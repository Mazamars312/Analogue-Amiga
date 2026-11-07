// Host-side tests for input_reg_update() poll-interval optimization in
// src/MPUBIOS/drivers/KMIO/inputs.cpp.
//
// input_reg_update() runs on every mainloop iteration. It reads
// AFP_REGISTOR(5) (joystick mode setting) every iteration to detect changes.
// Since the joystick mode only changes on menu interaction, we now poll it at
// a fixed interval (POLL_INTERVAL = 4) instead of every iteration, reducing
// volatile MMIO reads by 4x with an imperceptible detection delay.
//
// These tests model the function logic and pin down:
//   - Joystick mode changes are still detected (within POLL_INTERVAL polls)
//   - SPI commands are still sent when the mode changes
//   - MMIO reads are reduced by POLL_INTERVAL x
//   - No SPI command is sent when the mode hasn't changed
//
// Compile and run with:
//   g++ -std=c++14 tests/test_input_reg_update.cpp -o /tmp/test_input_reg_update && /tmp/test_input_reg_update

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Mock of AFP_REGISTOR(5) = *(volatile unsigned int *)(0xFFFFFF14)
// Bits 28-31 hold the joystick mode setting.
static volatile uint32_t g_afp_reg5 = 0;
static uint64_t g_reads = 0;

static inline uint32_t read_afp_reg5() {
  g_reads++;
  return g_afp_reg5;
}

// ---- Model of optimized input_reg_update() ----
static const int POLL_INTERVAL = 4;
static uint8_t g_last = 0xFF;
static uint8_t g_counter = 0;
static uint64_t g_spi_cmds = 0;

static void input_reg_update_model() {
  if (++g_counter >= POLL_INTERVAL) {
    g_counter = 0;
    uint8_t current = (uint8_t)(read_afp_reg5() >> 28);
    if (current != g_last) {
      g_last = current;
      g_spi_cmds++;  // HPS_spi_uio_cmd8(UIO_MM2_JOY, current)
    }
  }
}

static void reset_model() {
  g_last = 0xFF;
  g_counter = 0;
  g_spi_cmds = 0;
  g_reads = 0;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Test 1: No changes - reads happen every POLL_INTERVAL polls, SPI cmd sent once
  {
    reset_model();
    g_afp_reg5 = 0x00000000;  // mode 0

    for (int i = 0; i < 100; i++) input_reg_update_model();

    CHECK(g_spi_cmds == 1);  // First poll detects mode 0 (from 0xFF)
    CHECK(g_reads == 25);    // 100 / 4 = 25 reads
    printf("Test 1 (no changes): reads=%llu spi_cmds=%llu\n", g_reads, g_spi_cmds);
  }

  // Test 2: Change detected within POLL_INTERVAL polls
  {
    reset_model();
    g_afp_reg5 = 0x00000000;  // mode 0
    for (int i = 0; i < 20; i++) input_reg_update_model();  // Establish mode 0

    uint64_t reads_before = g_reads;
    uint64_t spi_cmds_before = g_spi_cmds;

    g_afp_reg5 = 0x10000000;  // Change to mode 1
    for (int i = 0; i < POLL_INTERVAL + 2; i++) input_reg_update_model();

    CHECK(g_spi_cmds > spi_cmds_before);  // Change detected
    CHECK(g_reads - reads_before <= POLL_INTERVAL + 1);  // Within interval + 1
    printf("Test 2 (change detected): reads_after_change=%llu (max %d)\n",
           g_reads - reads_before, POLL_INTERVAL + 1);
  }

  // Test 3: Multiple mode changes all detected
  {
    reset_model();
    g_afp_reg5 = 0x00000000;
    for (int i = 0; i < 20; i++) input_reg_update_model();

    uint64_t cmd_count = g_spi_cmds;

    g_afp_reg5 = 0x20000000;  // mode 2
    for (int i = 0; i < 10; i++) input_reg_update_model();
    CHECK(g_spi_cmds > cmd_count);
    cmd_count = g_spi_cmds;

    g_afp_reg5 = 0x30000000;  // mode 3
    for (int i = 0; i < 10; i++) input_reg_update_model();
    CHECK(g_spi_cmds > cmd_count);
    cmd_count = g_spi_cmds;

    g_afp_reg5 = 0x00000000;  // back to mode 0
    for (int i = 0; i < 10; i++) input_reg_update_model();
    CHECK(g_spi_cmds > cmd_count);

    printf("Test 3 (multiple changes): all %llu changes detected\n", g_spi_cmds);
  }

  // Test 4: Read reduction ratio (75% fewer reads)
  {
    reset_model();
    g_afp_reg5 = 0x50000000;  // mode 5

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) input_reg_update_model();

    CHECK(g_reads == iterations / POLL_INTERVAL);  // Exactly 1/4 of iterations
    double reduction = 100.0 * (iterations - g_reads) / iterations;
    CHECK(reduction == 75.0);
    printf("Test 4 (reduction): %d iterations, %llu reads, %.1f%% reduction\n",
           iterations, g_reads, reduction);
  }

  // Test 5: No SPI command when mode hasn't changed between polls
  {
    reset_model();
    g_afp_reg5 = 0x00000000;  // mode 0
    for (int i = 0; i < 4; i++) input_reg_update_model();  // First poll sets mode 0
    uint64_t cmds_after_first = g_spi_cmds;
    CHECK(cmds_after_first == 1);

    for (int i = 0; i < 20; i++) input_reg_update_model();  // Mode stays 0
    CHECK(g_spi_cmds == cmds_after_first);  // No new SPI commands
    printf("Test 5 (no spurious SPI): cmd_count_stable=%llu\n", g_spi_cmds);
  }

  if (failures == 0) { printf("test_input_reg_update: all checks passed\n"); return 0; }
  printf("test_input_reg_update: %d failure(s)\n", failures);
  return 1;
}
