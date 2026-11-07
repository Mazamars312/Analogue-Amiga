// Host-side tests for the dataslot_search_id() cache in
// src/MPUBIOS/drivers/apf/apf.cpp.
//
// dataslot_search_id() does a linear search through dataslot RAM, reading
// DATASLOT_RAM_ACCESS(i<<2) on every iteration (volatile MMIO read over the
// APF bus). Up to 128 reads per call. The dataslot table only changes when
// disks are inserted/removed, so the index per dataslot number is cached:
// repeated lookups for the same dataslot cost zero MMIO reads.
//
// These tests model the function logic and pin down:
//   - Same result with and without cache
//   - Zero MMIO reads on cache hit
//   - Cache invalidation forces re-search
//   - Misses are not cached (re-search each time)
//
// Compile and run with:
//   g++ -std=c++14 tests/test_dataslot_search.cpp -o /tmp/test_dataslot_search && /tmp/test_dataslot_search

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Mock of DATASLOT_RAM_ACCESS(x) = *(volatile unsigned int *)(DATASLOT_ID_BASE + x)
static volatile uint32_t g_dataslot_ram[257];  // 257 words to cover i up to 256 (i<<2 = 1024)
static uint64_t g_reads = 0;

static inline uint32_t ram_access(unsigned int x) {
  g_reads++;
  return g_dataslot_ram[x / 4];
}

// ---- Baseline: linear search, no cache (matches original source) ----
static int baseline_search(uint16_t value) {
  int i = 0;
  uint32_t temp;
  do {
    temp = ram_access(i << 2) & 0xffff;
    if (temp == value) {
      return i & 0xFFFF;
    }
    i = i + 2;
  } while (i <= 0x100);
  return 0;
}

// ---- Optimized: cached search ----
static uint16_t g_ds_cache_value = 0xFFFF;
static int g_ds_cache_index = -1;
static bool g_ds_cache_valid = false;

static void ds_cache_invalidate(void) {
  g_ds_cache_valid = false;
}

static int optimized_search(uint16_t value) {
  if (g_ds_cache_valid && g_ds_cache_value == value) {
    return g_ds_cache_index;
  }

  int i = 0;
  uint32_t temp;
  do {
    temp = ram_access(i << 2) & 0xffff;
    if (temp == value) {
      g_ds_cache_value = value;
      g_ds_cache_index = i;
      g_ds_cache_valid = true;
      return i & 0xFFFF;
    }
    i = i + 2;
  } while (i <= 0x100);

  g_ds_cache_valid = false;
  return 0;
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Set up dataslot RAM: entries at indices 0,2,4,...,62 hold dataslot numbers
  // 100,101,...,131. Sizes at odd indices. Make the array large enough to cover
  // the search loop's final read at i=256 (index 64).
  for (int i = 0; i < 257; i++) g_dataslot_ram[i] = 0;
  for (int i = 0; i < 32; i++) {
    g_dataslot_ram[i * 2] = (uint32_t)(100 + i);
    g_dataslot_ram[i * 2 + 1] = (uint32_t)(1024 * (i + 1));
  }

  // 1. Same result for a found dataslot (first slot at index 0).
  {
    g_ds_cache_valid = false;
    int bi = baseline_search(100);
    g_ds_cache_valid = false;
    int oi = optimized_search(100);
    CHECK(bi == oi);
    CHECK(oi == 0);
  }

  // 2. Same result for a found dataslot in the middle (dataslot 130 at index 60).
  {
    g_ds_cache_valid = false;
    int bi = baseline_search(130);
    g_ds_cache_valid = false;
    int oi = optimized_search(130);
    CHECK(bi == oi);
    CHECK(oi == 60);
  }

  // 3. Same result for a miss (dataslot not in table).
  {
    g_ds_cache_valid = false;
    int bi = baseline_search(999);
    g_ds_cache_valid = false;
    int oi = optimized_search(999);
    CHECK(bi == oi);
    CHECK(oi == 0);
  }

  // 4. Cache hit: zero MMIO reads on repeated lookup.
  {
    g_ds_cache_valid = false;
    g_reads = 0;
    optimized_search(100);  // populate cache
    uint64_t first_reads = g_reads;

    g_reads = 0;
    optimized_search(100);  // cache hit
    uint64_t hit_reads = g_reads;

    CHECK(hit_reads == 0);
    CHECK(first_reads > 0);
  }

  // 5. Cache invalidation: re-search after invalidate.
  {
    g_ds_cache_valid = false;
    g_reads = 0;
    optimized_search(100);
    uint64_t first = g_reads;

    ds_cache_invalidate();
    g_reads = 0;
    optimized_search(100);
    uint64_t after_invalidate = g_reads;

    CHECK(after_invalidate == first);
    CHECK(after_invalidate > 0);
  }

  // 6. Different dataslot values each get their own cache entry (last one wins,
  //    since the cache holds a single entry; correctness checked below).
  {
    g_ds_cache_valid = false;
    int o100 = optimized_search(100);  // populates cache with 100 -> 0
    CHECK(o100 == 0);

    g_reads = 0;
    int o130 = optimized_search(130);  // cache miss, re-searches for 130 -> 60
    uint64_t reads_for_130 = g_reads;
    CHECK(o130 == 60);
    CHECK(reads_for_130 > 0);

    g_reads = 0;
    int o100_again = optimized_search(100);  // cache miss again (130 evicted 100), re-searches
    uint64_t reads_for_100_again = g_reads;
    CHECK(o100_again == 0);
    CHECK(reads_for_100_again > 0);
  }

  // 7. Exhaustive: same result for every dataslot value 0..65535.
  int mismatches = 0;
  for (int v = 0; v <= 65535; v++) {
    g_ds_cache_valid = false;
    int bi = baseline_search((uint16_t)v);
    g_ds_cache_valid = false;
    int oi = optimized_search((uint16_t)v);
    if (bi != oi) mismatches++;
  }
  CHECK(mismatches == 0);

  if (failures == 0) { printf("test_dataslot_search: all checks passed\n"); return 0; }
  printf("test_dataslot_search: %d failure(s)\n", failures);
  return 1;
}
