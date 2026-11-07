// Host-side tests for core_poll_io() in src/MPUBIOS/app/core.cpp.
//
// core_poll_io() runs on every mainloop iteration. It used to issue 3
// spi_w(0) transactions over the FPGA SPI bus: the first reads the
// controller command (c1/c2), and the next two were discarded. FPGA
// analysis (userio.v) shows the two discarded reads are vestigial: for
// cmd=0x00, &cmd[7:4] is false so no FPGA state changes, and IO_WAIT is
// hardwired to 0. This test models the function and verifies that removing
// the two dummy reads does not change c1/c2 extraction or HandleFDD
// behaviour.
//
// Compile and run:
//   g++ -std=c++14 -Wall -Wextra tests/test_core_poll_io.cpp -o /tmp/test_core_poll_io && /tmp/test_core_poll_io

#include <cassert>
#include <cstdint>
#include <cstdio>

// Mock volatile MMIO read from the FPGA controller register.
// Returns a 16-bit value: high byte = cmd request + drive number,
// low byte = track number.
static volatile uint16_t g_fpga_word = 0;
static uint64_t g_spi_reads = 0;

static uint16_t spi_w(uint16_t /*word*/) {
  g_spi_reads++;
  return g_fpga_word;
}

// Command bit masks from fdd.h
static const uint8_t CMD_RDTRK = 0x01;
static const uint8_t CMD_WRTRK = 0x02;

// c1 = (tmp >> 8) & 0xFF: bits 8-15 of the FPGA word
// c2 = tmp & 0xFF: bits 0-7 of the FPGA word
static void core_poll_io_model(bool discard_dummy_reads, uint8_t &c1_out, uint8_t &c2_out) {
  uint16_t tmp = spi_w(0);
  c1_out = (uint8_t)(tmp >> 8);
  c2_out = (uint8_t)tmp;

  if (!discard_dummy_reads) {
    spi_w(0);  // dummy read 1 (removed by optimization)
    spi_w(0);  // dummy read 2 (removed by optimization)
  }
}

// HandleFDD models: only acts on CMD_RDTRK or CMD_WRTRK
static bool handle_fdd_would_act(uint8_t c1) {
  return (c1 & CMD_RDTRK) != 0 || (c1 & CMD_WRTRK) != 0;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Test 1: c1/c2 extraction is identical with or without dummy reads
  for (int word = 0; word < 0x10000; word += 257) {  // sample across all values
    g_fpga_word = (uint16_t)word;

    g_spi_reads = 0;
    uint8_t c1_with, c2_with;
    core_poll_io_model(false, c1_with, c2_with);
    uint64_t reads_with = g_spi_reads;

    g_spi_reads = 0;
    uint8_t c1_without, c2_without;
    core_poll_io_model(true, c1_without, c2_without);
    uint64_t reads_without = g_spi_reads;

    CHECK(c1_with == c1_without);
    CHECK(c2_with == c2_without);
    CHECK(reads_with == 3);
    CHECK(reads_without == 1);
  }

  // Test 2: HandleFDD behavior is unchanged
  // CMD_RDTRK set (c1 high byte has bit 0)
  {
    g_fpga_word = (uint16_t)(CMD_RDTRK << 8) | 0x42;  // read track, track 0x42
    g_spi_reads = 0;
    uint8_t c1, c2;
    core_poll_io_model(true, c1, c2);
    CHECK(c1 == CMD_RDTRK);
    CHECK(c2 == 0x42);
    CHECK(handle_fdd_would_act(c1));
    CHECK(g_spi_reads == 1);
  }

  // CMD_WRTRK set
  {
    g_fpga_word = (uint16_t)(CMD_WRTRK << 8) | 0x07;
    g_spi_reads = 0;
    uint8_t c1, c2;
    core_poll_io_model(true, c1, c2);
    CHECK(c1 == CMD_WRTRK);
    CHECK(c2 == 0x07);
    CHECK(handle_fdd_would_act(c1));
    CHECK(g_spi_reads == 1);
  }

  // No command (idle poll)
  {
    g_fpga_word = 0x0000;
    g_spi_reads = 0;
    uint8_t c1, c2;
    core_poll_io_model(true, c1, c2);
    CHECK(c1 == 0);
    CHECK(c2 == 0);
    CHECK(!handle_fdd_would_act(c1));
    CHECK(g_spi_reads == 1);
  }

  // Test 3: Exactly 1 spi_w call per poll after optimization (was 3)
  g_fpga_word = 0x1234;
  g_spi_reads = 0;
  {
    uint8_t c1, c2;
    core_poll_io_model(true, c1, c2);
    CHECK(c1 == 0x12);
    CHECK(c2 == 0x34);
  }
  CHECK(g_spi_reads == 1);

  if (failures == 0) { printf("test_core_poll_io: all checks passed\n"); return 0; }
  printf("test_core_poll_io: %d failure(s)\n", failures);
  return 1;
}
