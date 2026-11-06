// Host-side unit tests for buffer_has_substr bootblock scanning logic.
// Compile and run with:
//   g++ -std=c++14 tests/test_buffer_search.cpp -o test_buffer_search && ./test_buffer_search

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Replicate the exact function from src/MPUBIOS/app/minimig_config.cpp
static bool buffer_has_substr(const uint8_t *buf, int len, const char *sub) {
  int sub_len = strlen(sub);
  if (sub_len > len || sub_len == 0) return false;

  char first_upper = sub[0];
  if (first_upper >= 'a' && first_upper <= 'z') first_upper -= 32;
  char first_lower = (first_upper >= 'A' && first_upper <= 'Z') ? (first_upper + 32) : first_upper;

  int max_i = len - sub_len;
  for (int i = 0; i <= max_i; i++) {
    char c = (char)buf[i];
    if (c != first_upper && c != first_lower) continue;

    bool match = true;
    for (int j = 1; j < sub_len; j++) {
      char c1 = (char)buf[i + j];
      char c2 = sub[j];
      // Case-insensitive ASCII comparison
      if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
      if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

int main() {
  uint8_t buf[256];
  memset(buf, 0, sizeof(buf));

  // 1. Basic search at beginning
  memcpy(buf, "TURRICAN II", 11);
  assert(buffer_has_substr(buf, 256, "TURRICAN") == true);
  assert(buffer_has_substr(buf, 256, "turrican") == true);
  assert(buffer_has_substr(buf, 256, "TuRrIcAn") == true);
  assert(buffer_has_substr(buf, 256, "TURRICAN II") == true);
  assert(buffer_has_substr(buf, 256, "FACTOR 5") == false);

  // 2. Search at arbitrary offset
  memset(buf, 0xAA, sizeof(buf));
  memcpy(buf + 100, "Superfrog (C) Team17", 20);
  assert(buffer_has_substr(buf, 256, "SUPERFROG") == true);
  assert(buffer_has_substr(buf, 256, "superfrog") == true);
  assert(buffer_has_substr(buf, 256, "TEAM17") == true);

  // 3. Search at exact buffer boundary (end of buffer)
  memset(buf, 0, sizeof(buf));
  memcpy(buf + 256 - 9, "WORKBENCH", 9);
  assert(buffer_has_substr(buf, 256, "WORKBENCH") == true);
  assert(buffer_has_substr(buf, 256, "workbench") == true);
  // Beyond buffer length should not match
  assert(buffer_has_substr(buf, 255, "WORKBENCH") == false);

  // 4. Single-character search
  assert(buffer_has_substr(buf + 256 - 9, 9, "W") == true);
  assert(buffer_has_substr(buf + 256 - 9, 9, "w") == true);
  assert(buffer_has_substr(buf + 256 - 9, 9, "H") == true);
  assert(buffer_has_substr(buf + 256 - 9, 9, "Z") == false);

  // 5. Partial matches and false starts
  memset(buf, 0, sizeof(buf));
  memcpy(buf, "TURR-TURRI-TURRICAN", 19);
  assert(buffer_has_substr(buf, 19, "TURRICAN") == true);

  // 6. Substring longer than buffer
  assert(buffer_has_substr(buf, 5, "TURRICAN") == false);

  // 7. Non-letter character matching (space, digits)
  memcpy(buf, "FACTOR 5", 8);
  assert(buffer_has_substr(buf, 8, "FACTOR 5") == true);
  assert(buffer_has_substr(buf, 8, "factor 5") == true);

  printf("All buffer_has_substr tests passed successfully!\n");
  return 0;
}
