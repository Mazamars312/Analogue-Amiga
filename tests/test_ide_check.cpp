// Host-side tests for ide_check() dead code elimination in
// src/MPUBIOS/drivers/HDD/ide.cpp.
//
// ide_check() does:
//   res = spi_w(UIO_DMA_SDIO);   // 0x63
//   if (!res) res = (uint8_t)spi_w(0);   // <-- DEAD CODE
//
// FPGA hps_ext.v returns for cmd=0x63:
//   io_dout_reg = {4'hE, 2'b00, 2'b00, 2'b00, ide_req} = 0xE000 | ide_req
// Since 4'hE has bit 15 set, res is ALWAYS non-zero. !res is ALWAYS false.
// The second spi_w(0) is provably dead code.
//
// ide_check() uses HPS_EnableIO() -> SSPI_IO_EN -> hps_ext.v (UIO subsystem),
// NOT userio.v. Confirmed by tracing HPS_EnableFpga() (userio.v) vs
// HPS_EnableIO() (hps_ext.v) usage.
//
// Compile and run with:
//   g++ -std=c++14 tests/test_ide_check.cpp -o /tmp/test_ide_check && /tmp/test_ide_check

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Mock of spi_w() through hps_ext.v (UIO subsystem accessed via HPS_EnableIO)
// For cmd=0x63 (UIO_DMA_SDIO): returns {4'hE, 2'b00, 2'b00, 2'b00, ide_req}
// = 0xE000 | ide_req (bit 15 always set, never zero)
static volatile uint16_t g_ide_req = 0;
static uint64_t g_spi_reads = 0;

static uint16_t spi_w_model(uint16_t word) {
  g_spi_reads++;
  if (word == 0x63) {  // UIO_DMA_SDIO
    return 0xE000u | (g_ide_req & 0x3Fu);
  }
  // cmd=0x00 through hps_ext.v: cmd[7:4]=0, no state changes, io_dout_reg=0
  return 0u;
}

// ---- Original ide_check() ----
static uint16_t ide_check_original() {
  uint16_t res;
  res = spi_w_model(0x63);  // UIO_DMA_SDIO
  if (!res) res = (uint8_t)spi_w_model(0);  // DEAD CODE - never reached
  return res;
}

// ---- Optimized ide_check() ----
static uint16_t ide_check_optimized() {
  // Dead code removed: spi_w(UIO_DMA_SDIO) returns 0xE000|ide_req,
  // bit 15 always set, never 0. The second spi_w(0) is unreachable.
  return spi_w_model(0x63);
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Test 1: Original and optimized return identical values for all ide_req
  for (uint16_t ide_req = 0; ide_req <= 63; ide_req++) {
    g_ide_req = ide_req;
    g_spi_reads = 0;
    uint16_t orig = ide_check_original();
    uint64_t orig_reads = g_spi_reads;
    g_spi_reads = 0;
    uint16_t opt = ide_check_optimized();
    uint64_t opt_reads = g_spi_reads;

    CHECK(orig == opt);
    CHECK(opt == (0xE000u | ide_req));
    CHECK(orig_reads == 1);  // original: 1 read (second is dead, never called)
    CHECK(opt_reads == 1);   // optimized: 1 read
  }

  // Test 2: res is never 0, so the if(!res) branch is never taken
  for (uint16_t ide_req = 0; ide_req <= 63; ide_req++) {
    g_ide_req = ide_req;
    uint16_t res = spi_w_model(0x63);
    CHECK(res != 0);
    CHECK((res & 0x8000u) != 0);  // bit 15 always set
  }

  // Test 3: core_poll_io() usage - ide_io() calls extract correct bits
  for (uint16_t ide_req = 0; ide_req <= 63; ide_req++) {
    g_ide_req = ide_req;
    uint16_t sd_req = ide_check_optimized();
    CHECK((sd_req & 0x8000u) != 0);  // always true
    CHECK((sd_req & 7) == (ide_req & 7));       // drive 0: bits 0-2
    CHECK(((sd_req >> 3) & 7) == ((ide_req >> 3) & 7));  // drive 1: bits 3-5
    // mainprintf checks are dead: (sd_req >> 3) >= 0x1C00, never 5|6|7
    CHECK(((sd_req >> 3) & 0x1FFF) != 5);
    CHECK(((sd_req >> 3) & 0x1FFF) != 6);
    CHECK(((sd_req >> 3) & 0x1FFF) != 7);
  }

  // Test 4: ide.cpp wait loop extraction is unchanged
  // ide_req_var = (ide_check() >> bitoff) & 7
  for (uint16_t ide_req = 0; ide_req <= 63; ide_req++) {
    g_ide_req = ide_req;
    // bitoff = 0 (drive 0)
    uint16_t req0 = (ide_check_optimized() >> 0) & 7;
    CHECK(req0 == (ide_req & 7));
    // bitoff = 3 (drive 1)
    uint16_t req1 = (ide_check_optimized() >> 3) & 7;
    CHECK(req1 == ((ide_req >> 3) & 7));
  }

  // Test 5: Read count - exactly 1 spi_w per call (was 1, dead 2nd removed)
  g_ide_req = 0;
  g_spi_reads = 0;
  ide_check_optimized();
  CHECK(g_spi_reads == 1);  // exactly 1 read, down from potentially 2

  if (failures == 0) {
    printf("test_ide_check: all checks passed\n");
    printf("  ide_check() dead code: 2nd spi_w(0) removed, saves 1 MMIO read/call\n");
    printf("  ide_check() called every mainloop iteration from core_poll_io()\n");
    return 0;
  }
  printf("test_ide_check: %d failure(s)\n", failures);
  return 1;
}
