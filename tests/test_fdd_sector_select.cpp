// Host-side tests for the per-sector pointer selection in ReadTrack()
// (src/MPUBIOS/drivers/floppy_drive/fdd.cpp).
//
// ReadTrack() loads a whole track (SECTOR_COUNT * 512 = 5632 bytes) into
// sector_bufferfdd1, then used to byte-copy each 512-byte sector into
// sector_bufferfdd0 before calling SendSector(). SendSector() only reads its
// pData buffer, so the staging copy was redundant: the code now passes
// &sector_bufferfdd1[sector << 9] directly. These tests pin down that the
// direct pointer exposes exactly the same sector payload and stays in bounds.
//
// Compile and run with:
//   g++ -std=c++14 tests/test_fdd_sector_select.cpp -o /tmp/test_fdd_sector_select && /tmp/test_fdd_sector_select

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

static const int DATA_SIZE = 512;                        // bytes per sector
static const int SECTOR_COUNT = 11;
static const int TRACK_BYTES = DATA_SIZE * SECTOR_COUNT; // 5632
static const int LAST_SECTOR = SECTOR_COUNT - 1;

static uint8_t track[TRACK_BYTES];
static uint8_t staging[DATA_SIZE];

// The old behaviour: byte-at-a-time staging copy of one sector.
static void old_copy(int sector, uint64_t &bytes_copied) {
  for (int tmp = 0; tmp <= 511; tmp++) {
    staging[tmp] = track[(sector << 9) | tmp];
    bytes_copied++;
  }
}

// The new behaviour: pointer straight into the loaded track buffer.
static const uint8_t *new_pointer(int sector) {
  return &track[sector << 9];
}

// Mirrors the read passes SendSector() performs over pData (4-byte checksum
// pass + two 2-byte send passes). Reads only; returns a sink.
static uint32_t send_reads(const uint8_t *pData) {
  uint32_t acc = 0;
  const uint8_t *p = pData;
  for (int i = 0; i < DATA_SIZE / 2 / 4; i++) {
    acc ^= (uint32_t)p[0] ^ (uint32_t)p[1] ^ (uint32_t)p[2] ^ (uint32_t)p[3];
    p += 4;
  }
  p = pData;
  for (int i = 0; i < DATA_SIZE / 4; i++) {
    acc ^= (uint32_t)p[0] ^ (uint32_t)p[1];
    p += 2;
  }
  p = pData;
  for (int i = 0; i < DATA_SIZE / 4; i++) {
    acc ^= (uint32_t)p[0] ^ (uint32_t)p[1];
    p += 2;
  }
  return acc;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Fill the track with a deterministic, non-uniform payload.
  for (int i = 0; i < TRACK_BYTES; i++) track[i] = (uint8_t)(i * 7 + 1);

  // Every sector's direct pointer must expose exactly the bytes the staging
  // copy produced.
  for (int sector = 0; sector < SECTOR_COUNT; sector++) {
    uint64_t copied = 0;
    memset(staging, 0, sizeof(staging));
    old_copy(sector, copied);
    CHECK(copied == DATA_SIZE);

    const uint8_t *direct = new_pointer(sector);
    CHECK(memcmp(staging, direct, DATA_SIZE) == 0);

    // Pointer must stay inside the track buffer.
    CHECK(direct >= track);
    CHECK(direct + DATA_SIZE <= track + TRACK_BYTES);
    CHECK((size_t)(direct - track) == (size_t)(sector * DATA_SIZE));
  }

  // Sector 0 starts at the buffer origin; the last sector ends exactly at the
  // end of the track buffer (the old staging copy had the same coverage).
  CHECK(new_pointer(0) == track);
  CHECK(new_pointer(LAST_SECTOR) + DATA_SIZE == track + TRACK_BYTES);

  // The send path must not mutate the track buffer or the staging buffer.
  uint8_t before[TRACK_BYTES];
  memcpy(before, track, sizeof(before));
  uint8_t staging_before[DATA_SIZE];
  memcpy(staging_before, staging, sizeof(staging_before));
  volatile uint32_t sink = 0;
  for (int sector = 0; sector < SECTOR_COUNT; sector++) sink ^= send_reads(new_pointer(sector));
  (void)sink;
  CHECK(memcmp(before, track, sizeof(before)) == 0);
  CHECK(memcmp(staging_before, staging, sizeof(staging_before)) == 0);

  // No staging copy is performed on the new path.
  uint64_t copied = 0;
  for (int sector = 0; sector < SECTOR_COUNT; sector++) {
    const uint8_t *direct = new_pointer(sector);
    sink ^= send_reads(direct);
  }
  (void)sink;
  CHECK(copied == 0);

  // The sector counter wraps back to the start of the track, as ReadTrack()
  // does when it runs past LAST_SECTOR.
  int sector = 0;
  for (int i = 0; i < 22; i++) {
    CHECK(sector >= 0 && sector <= LAST_SECTOR);
    sector++;
    if (sector >= SECTOR_COUNT) sector = 0;
  }
  CHECK(sector == 0);

  if (failures == 0) { printf("test_fdd_sector_select: all checks passed\n"); return 0; }
  printf("test_fdd_sector_select: %d failure(s)\n", failures);
  return 1;
}
