// Host-side tests for dataslot_search_id() and dataslot_size() caching in
// src/MPUBIOS/drivers/apf/apf.cpp.
//
// dataslot_search_id() does a linear search through dataslot RAM, reading
// DATASLOT_RAM_ACCESS(i<<2) on every iteration (volatile MMIO read over the
// APF bus). Up to 128 reads per call. dataslot_size() reads one additional
// word (DATASLOT_RAM_ACCESS((i+1)<<2)) for the size.
//
// Both functions cache their results: repeated lookups for the same dataslot
// cost zero MMIO reads. The cache is invalidated by dataslot_cache_invalidate()
// (called from core_update_dataslots() when disks change).
//
// Compile and run with:
//   g++ -std=c++14 tests/test_dataslot_search.cpp -o /tmp/test_dataslot_search && /tmp/test_dataslot_search

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Mock of DATASLOT_RAM_ACCESS(x) = *(volatile unsigned int *)(DATASLOT_ID_BASE + x)
static volatile uint32_t g_dataslot_ram[257];  // cover i up to 256 (i<<2 = 1024)
static uint64_t g_reads = 0;

static inline uint32_t ram_access(unsigned int x) {
  g_reads++;
  return g_dataslot_ram[x / 4];
}

// ---- Baseline: linear search, no cache (matches original source) ----
static int baseline_search_id(uint16_t value) {
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

static uint32_t baseline_dataslot_size(uint16_t value) {
  int i = baseline_search_id(value);
  return ram_access((i + 1) << 2);
}

// ---- Optimized: cached search + size (matches apf.cpp implementation) ----
static uint16_t g_ds_cache_value = 0xFFFF;
static int g_ds_cache_index = -1;
static uint32_t g_ds_cache_size = 0;
static bool g_ds_cache_valid = false;

static void ds_cache_invalidate(void) {
  g_ds_cache_valid = false;
}

static int optimized_search_id(uint16_t value) {
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
      g_ds_cache_size = ram_access((i + 1) << 2);
      g_ds_cache_valid = true;
      return i & 0xFFFF;
    }
    i = i + 2;
  } while (i <= 0x100);

  g_ds_cache_valid = false;
  return 0;
}

static uint32_t optimized_dataslot_size(uint16_t value) {
  if (g_ds_cache_valid && g_ds_cache_value == value) {
    return g_ds_cache_size;
  }
  int i = optimized_search_id(value);
  if (g_ds_cache_valid && g_ds_cache_value == value) {
    return g_ds_cache_size;
  }
  return ram_access((i + 1) << 2);
}

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { failures++; printf("FAIL line %d: %s\n", __LINE__, #cond); } \
} while (0)

int main() {
  // Set up dataslot RAM: entries at indices 0,2,4,...,62 hold dataslot numbers
  // 100,101,...,131. Sizes at odd indices.
  for (int i = 0; i < 257; i++) g_dataslot_ram[i] = 0;
  for (int i = 0; i < 32; i++) {
    g_ds_cache_valid = false;
    g_dataslot_ram[i * 2] = (uint32_t)(100 + i);
    g_dataslot_ram[i * 2 + 1] = (uint32_t)(1024 * (i + 1));
  }

  // 1. dataslot_search_id() returns correct index with and without cache.
  {
    g_ds_cache_valid = false;
    int bi = baseline_search_id(100);
    g_ds_cache_valid = false;
    int oi = optimized_search_id(100);
    CHECK(bi == oi);
    CHECK(oi == 0);
  }
  {
    g_ds_cache_valid = false;
    int bi = baseline_search_id(130);
    g_ds_cache_valid = false;
    int oi = optimized_search_id(130);
    CHECK(bi == oi);
    CHECK(oi == 60);
  }
  {
    g_ds_cache_valid = false;
    int bi = baseline_search_id(999);
    g_ds_cache_valid = false;
    int oi = optimized_search_id(999);
    CHECK(bi == oi);
    CHECK(oi == 0);
  }

  // 2. dataslot_size() returns correct value with and without cache.
  {
    g_ds_cache_valid = false;
    uint32_t bs = baseline_dataslot_size(100);
    g_ds_cache_valid = false;
    uint32_t os = optimized_dataslot_size(100);
    CHECK(bs == os);
    CHECK(os == 1024);  // dataslot 100 at index 0, size at index 1 = 1024*1
  }
  {
    g_ds_cache_valid = false;
    uint32_t bs = baseline_dataslot_size(130);
    g_ds_cache_valid = false;
    uint32_t os = optimized_dataslot_size(130);
    CHECK(bs == os);
    CHECK(os == 31744);  // dataslot 130 at index 60, size at index 61 = 1024*31
  }

  // 3. Cache hit: dataslot_size() costs zero MMIO reads on repeated lookup.
  {
    g_ds_cache_valid = false;
    g_reads = 0;
    optimized_dataslot_size(100);  // populate cache
    uint64_t first_reads = g_reads;

    g_reads = 0;
    optimized_dataslot_size(100);  // cache hit
    uint64_t hit_reads = g_reads;

    CHECK(hit_reads == 0);
    CHECK(first_reads > 0);
  }

  // 4. dataslot_search_id() cache hit also costs zero reads.
  {
    g_ds_cache_valid = false;
    g_reads = 0;
    optimized_search_id(100);  // populate cache
    uint64_t first_reads = g_reads;

    g_reads = 0;
    optimized_search_id(100);  // cache hit
    uint64_t hit_reads = g_reads;

    CHECK(hit_reads == 0);
    CHECK(first_reads > 0);
  }

  // 5. Cache invalidation: re-search after invalidate.
  {
    g_ds_cache_valid = false;
    g_reads = 0;
    optimized_dataslot_size(100);
    uint64_t first = g_reads;

    ds_cache_invalidate();
    g_reads = 0;
    optimized_dataslot_size(100);
    uint64_t after_invalidate = g_reads;

    CHECK(after_invalidate == first);
    CHECK(after_invalidate > 0);
  }

  // 6. Exhaustive: dataslot_search_id() returns same index for all values.
  int mismatches = 0;
  for (int v = 0; v <= 65535; v++) {
    g_ds_cache_valid = false;
    int bi = baseline_search_id((uint16_t)v);
    g_ds_cache_valid = false;
    int oi = optimized_search_id((uint16_t)v);
    if (bi != oi) mismatches++;
  }
  CHECK(mismatches == 0);

  // 7. Exhaustive: dataslot_size() returns same value for all values.
  mismatches = 0;
  for (int v = 0; v <= 65535; v++) {
    g_ds_cache_valid = false;
    uint32_t bs = baseline_dataslot_size((uint16_t)v);
    g_ds_cache_valid = false;
    uint32_t os = optimized_dataslot_size((uint16_t)v);
    if (bs != os) mismatches++;
  }
  CHECK(mismatches == 0);

  if (failures == 0) { printf("test_dataslot_search: all checks passed\n"); return 0; }
  printf("test_dataslot_search: %d failure(s)\n", failures);
  return 1;
}
