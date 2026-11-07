// Host-side tests for the word-wise data checksum in SendSector()
// (src/MPUBIOS/drivers/floppy_drive/fdd.cpp).
//
// The Amiga MFM data checksum used to be computed one byte at a time:
// checksum[k] ^= x ^ x >> 1 for every 4th byte x, 512 iterations per sector.
// That transform is linear over XOR, so SendSector() now XORs the sector as
// 128 little-endian 32-bit words and applies (x ^ x >> 1) per byte once at
// the end. These tests pin down that both produce identical checksums.
//
// Compile and run with:
//   g++ -std=c++14 -Wall -Wextra tests/test_fdd_data_checksum.cpp -o /tmp/test_fdd_data_checksum && /tmp/test_fdd_data_checksum

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DATA_SIZE 0x400 // MFM bytes per sector, as in fdd.cpp (512 data bytes)
static const int SECTOR_COUNT = 11;

typedef uint32_t __attribute__((may_alias)) u32_alias;

alignas(4) static uint8_t track[512 * SECTOR_COUNT];

static void checksum_old(const uint8_t *pData, uint8_t *checksum) {
  checksum[0] = checksum[1] = checksum[2] = checksum[3] = 0;
  const uint8_t *p = pData;
  unsigned short i = DATA_SIZE / 2 / 4;
  while (i--) {
    uint8_t x;
    x = *p++; checksum[0] ^= x ^ x >> 1;
    x = *p++; checksum[1] ^= x ^ x >> 1;
    x = *p++; checksum[2] ^= x ^ x >> 1;
    x = *p++; checksum[3] ^= x ^ x >> 1;
  }
}

// Mirrors the optimized block in SendSector().
static void checksum_new(const uint8_t *pData, uint8_t *checksum) {
  const u32_alias *w = (const u32_alias *)pData;
  uint32_t acc = 0;
  unsigned short i = DATA_SIZE / 2 / 4;
  while (i--) acc ^= *w++;
  acc ^= (acc >> 1) & 0x7F7F7F7Fu;
  checksum[0] = (uint8_t)acc;
  checksum[1] = (uint8_t)(acc >> 8);
  checksum[2] = (uint8_t)(acc >> 16);
  checksum[3] = (uint8_t)(acc >> 24);
}

static int compare_sector(int sector) {
  uint8_t a[4], b[4];
  checksum_old(&track[sector << 9], a);
  checksum_new(&track[sector << 9], b);
  return memcmp(a, b, 4) != 0;
}

int main() {
  int mismatches = 0;

  // The routine assumes a little-endian target, like the VexRiscv MPU.
  const uint32_t one = 1;
  assert(*(const uint8_t *)&one == 1);

  // Constant fills: every byte value in every sector slot.
  for (int v = 0; v < 256; v++) {
    memset(track, v, sizeof(track));
    for (int s = 0; s < SECTOR_COUNT; s++) mismatches += compare_sector(s);
  }

  // Single-bit patterns at every position of one sector (catches lane and
  // cross-byte shift mistakes in the (acc >> 1) & 0x7F7F7F7F step).
  for (int pos = 0; pos < 512; pos++) {
    for (int bit = 0; bit < 8; bit++) {
      memset(track, 0, 512);
      track[pos] = (uint8_t)(1u << bit);
      mismatches += compare_sector(0);
    }
  }

  // Random tracks.
  srand(12345);
  for (int t = 0; t < 2000; t++) {
    for (size_t k = 0; k < sizeof(track); k++) track[k] = (uint8_t)rand();
    for (int s = 0; s < SECTOR_COUNT; s++) mismatches += compare_sector(s);
  }

  // Sector pointers stay 4-byte aligned for the word loads.
  for (int s = 0; s < SECTOR_COUNT; s++)
    assert((((uintptr_t)&track[s << 9]) & 3) == 0);

  if (mismatches) {
    printf("test_fdd_data_checksum: %d mismatches\n", mismatches);
    return 1;
  }
  printf("test_fdd_data_checksum: all checks passed\n");
  return 0;
}
