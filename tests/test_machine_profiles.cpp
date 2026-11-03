// Host-side unit tests for the machine profile resolver (Plan A6).
// Compile and run on any host C++ compiler:
//   g++ -std=c++14 tests/test_machine_profiles.cpp -o test_machine_profiles && ./test_machine_profiles
//
// The resolver is shared with the MPU firmware via
// src/MPUBIOS/app/machine_profiles.h, so these tests exercise the exact
// table and logic that ships in amiga_mpu_bios.bin.

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <initializer_list>

// Capture resolver diagnostics instead of printing via firmware mainprintf.
static char g_log[2048];
static void test_log(const char *fmt, ...) {
  va_list ap; va_start(ap, fmt);
  vsnprintf(g_log + strlen(g_log), sizeof(g_log) - strlen(g_log), fmt, ap);
  va_end(ap);
}
#define MACHINE_PROFILE_LOG test_log
#include "../src/MPUBIOS/app/machine_profiles.h"

static int failures = 0;
#define CHECK(cond) do { \
  if (!(cond)) { \
    failures++; \
    printf("FAIL line %d: %s\n", __LINE__, #cond); \
  } \
} while (0)

// Decode helpers matching the RTL consumers:
//   minimig.v:  ecs = |chipset_config[4:3], aga = chipset_config[4],
//               ntsc = chipset_config[1], turbo = chipset_config[0]
//   cpu_wrapper.v: TG68K selected when cpucfg[1] set, fx68k otherwise
//   core_top.v: fastramcfg = memcfg[6:4]
struct Eff { uint8_t chip, cpu, mem; };
static Eff resolve(uint8_t id, uint8_t chip, uint8_t cpu, uint8_t mem) {
  Eff e;
  g_log[0] = 0;
  resolve_profile(id, chip, cpu, mem, &e.chip, &e.cpu, &e.mem);
  return e;
}

int main() {
  // --- Custom (0) passes everything through, including NTSC/turbo/A1K bits ---
  Eff c = resolve(PROFILE_CUSTOM, 0x1B, 0x05, 0x53);
  CHECK(c.chip == 0x1B && c.cpu == 0x05 && c.mem == 0x53);
  CHECK(strstr(g_log, "Custom") != nullptr);

  // --- Unknown IDs fall back to Custom pass-through with a diagnostic ---
  for (uint8_t bad : { (uint8_t)7, (uint8_t)100, (uint8_t)255 }) {
    Eff u = resolve(bad, 0x13, 0x07, 0x2A);
    CHECK(u.chip == 0x13 && u.cpu == 0x07 && u.mem == 0x2A);
    CHECK(strstr(g_log, "unknown profile ID") != nullptr);
  }

  // --- A500: 68000/OCS/512K Chip/512K Slow ---
  Eff a500 = resolve(PROFILE_A500, 0x00, 0x03, 0x53);
  CHECK(a500.chip == 0x00);                       // OCS (ecs=0, aga=0), PAL, no turbo
  CHECK(a500.cpu  == 0x00);                       // fx68k 68000 path, no cache
  CHECK((a500.mem & 0x03) == 0);                  // Chip = 512K
  CHECK(((a500.mem >> 2) & 0x03) == 1);           // Slow = 512K
  CHECK(((a500.mem >> 4) & 0x07) == 0);           // Fast = None

  // --- A500 Plus / A600: 68000/ECS/1M Chip ---
  for (uint8_t id : { (uint8_t)PROFILE_A500_PLUS, (uint8_t)PROFILE_A600 }) {
    Eff e = resolve(id, 0x00, 0x00, 0x00);
    CHECK(e.chip == 0x08);                        // ECS (bit 3 -> ecs=1, aga=0)
    CHECK(e.cpu == 0x00);
    CHECK(e.mem == 0x01);                         // Chip = 1M, no Slow, no Fast
  }

  // --- A1200: 020/AGA/2M Chip, no Fast, no cache ---
  Eff a1200 = resolve(PROFILE_A1200, 0x00, 0x00, 0x00);
  CHECK(a1200.chip == 0x10);                      // AGA (bit 4; ecs implied)
  CHECK(a1200.cpu == 0x03);                       // TT=11 -> TG68K, cache off
  CHECK(a1200.mem == 0x03);                       // Chip = 2M

  // --- A1200 Accelerated: 020/AGA/2M Chip/32M Fast/cache ---
  Eff accel = resolve(PROFILE_A1200_ACCEL, 0x00, 0x00, 0x00);
  CHECK(accel.chip == 0x10);
  CHECK(accel.cpu == 0x07);                       // TT=11 + cache bit 2
  CHECK((accel.mem & 0x03) == 3);                 // Chip = 2M
  CHECK(((accel.mem >> 4) & 0x07) == 5);          // Fast = 32M (bits[6:4], per core_top.v)

  // --- AmigaVision: same RAM as accelerated, cache disabled ---
  Eff av = resolve(PROFILE_AMIGAVISION, 0x00, 0x00, 0x00);
  CHECK(av.chip == 0x10);
  CHECK(av.cpu == 0x03);                          // cache off
  CHECK(av.mem == 0x53);

  // --- PAL/NTSC merges from the independent video-standard choice (bit 1) ---
  for (uint8_t id = PROFILE_A500; id <= PROFILE_AMIGAVISION; id++) {
    Eff pal  = resolve(id, 0x00, 0x00, 0x00);
    Eff ntsc = resolve(id, 0x02, 0x00, 0x00);
    CHECK((pal.chip & 0x02) == 0);
    CHECK((ntsc.chip & 0x02) == 0x02);
    CHECK((ntsc.chip & ~0x02) == pal.chip);       // only the NTSC bit differs
  }

  // --- Turbo (bit 0) and A1000 (bit 2) are forced off by named profiles ---
  Eff forced = resolve(PROFILE_A1200, 0x03, 0x00, 0x00);  // NTSC+Turbo set in custom reg
  CHECK((forced.chip & 0x01) == 0);               // turbo cleared
  CHECK((forced.chip & 0x04) == 0);               // A1000 cleared
  CHECK((forced.chip & 0x02) == 0x02);            // NTSC preserved

  // --- CPU/cache/menu register bits beyond the profile fields don't leak ---
  Eff leak = resolve(PROFILE_A1200, 0x00, 0x0F, 0xFF);   // custom fast-kick + hrtmon bits
  CHECK(leak.cpu == 0x03);                        // profile CPU wins wholesale
  CHECK(leak.mem == 0x03);                        // hrtmon bit 7 not preserved

  if (failures == 0) {
    printf("All machine profile resolver tests passed.\n");
    return 0;
  }
  printf("%d test(s) FAILED.\n", failures);
  return 1;
}
