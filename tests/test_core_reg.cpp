// Host-side unit tests for core_reg_update logic in src/MPUBIOS/app/core.cpp.
// Compile and run with:
//   g++ -std=c++14 tests/test_core_reg.cpp -o test_core_reg && ./test_core_reg

#include <cassert>
#include <cstdint>
#include <cstdio>

// Mock MMIO registers for testing
static uint32_t s_mock_core_output = 0;
static uint32_t s_mock_afp_7 = 0;
static int s_core_write_count = 0;

#define MOCK_CORE_OUTPUT_REGISTOR() s_mock_core_output
#define MOCK_AFP_REGISTOR(x) s_mock_afp_7

// Replicate the exact logic from src/MPUBIOS/app/core.cpp:
//   uint32_t tmp = CORE_OUTPUT_REGISTOR();
//   uint32_t next = (tmp & 0xFFFFFF30) | (AFP_REGISTOR(7) & 0x000000cf);
//   if (tmp != next) {
//     CORE_OUTPUT_REGISTOR() = next;
//   }
static void core_reg_update_test() {
  uint32_t tmp = MOCK_CORE_OUTPUT_REGISTOR();
  uint32_t next = (tmp & 0xFFFFFF30) | (MOCK_AFP_REGISTOR(7) & 0x000000cf);
  if (tmp != next) {
    s_mock_core_output = next;
    s_core_write_count++;
  }
}

int main() {
  s_mock_core_output = 0x00000000;
  s_mock_afp_7 = 0x05;
  s_core_write_count = 0;

  // 1. Initial call updates bits to match AFP_REGISTOR(7)
  core_reg_update_test();
  assert(s_mock_core_output == 0x05);
  assert(s_core_write_count == 1);

  // 2. Subsequent hot polling loops with unchanged inputs must produce zero redundant writes
  for (int i = 0; i < 1000; i++) {
    core_reg_update_test();
  }
  assert(s_mock_core_output == 0x05);
  assert(s_core_write_count == 1);

  // 3. When external bits (e.g. keyboard/mouse OSD bits 4 & 5) are modified,
  // mask 0xFFFFFF30 preserves them without corrupting or dropping other bits
  s_mock_core_output |= 0x00000030; // 0x35
  core_reg_update_test();
  assert(s_mock_core_output == 0x35);
  assert(s_core_write_count == 1); // 0x35 already matches next, no write needed

  // 4. When AFP_REGISTOR(7) changes, new value is detected and written exactly once
  s_mock_afp_7 = 0x0A;
  core_reg_update_test();
  assert(s_mock_core_output == 0x3A);
  assert(s_core_write_count == 2);

  // Another 1000 cycles without change
  for (int i = 0; i < 1000; i++) {
    core_reg_update_test();
  }
  assert(s_mock_core_output == 0x3A);
  assert(s_core_write_count == 2);

  // 5. Bits masked out by 0x000000cf in AFP_REGISTOR(7) (e.g. bits 4, 5, and upper bits)
  // do not override existing bits 4 & 5
  s_mock_afp_7 = 0xFFFFFFFF; // AFP reg 7 has all bits set
  core_reg_update_test();
  // Mask 0x000000cf sets bits [3:0] (0xF) and [7:6] (0xC0), but bits [5:4] (0x30) come from core_output
  assert((s_mock_core_output & 0x000000ff) == 0xFF);
  assert(s_core_write_count == 3);

  printf("All core_reg_update tests passed successfully!\n");
  return 0;
}
