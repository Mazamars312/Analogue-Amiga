#include <cassert>
#include <cstdint>
#include <cstdio>
#include "../src/MPUBIOS/app/profile_def.h"

int main() {
  uint8_t eff_chip, eff_cpu, eff_mem;
  bool eff_ide;
  const struct ProfileDef *prof = nullptr;

  printf("Running test: Custom profile (pass-through)...\n");
  bool ok = resolve_profile_lookup(PROFILE_CUSTOM, 0x13, 0x07, 0x53, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_CUSTOM);
  assert(eff_chip == 0x13);
  assert(eff_cpu == 0x07);
  assert(eff_mem == 0x53);
  assert(eff_ide == false);

  printf("Running test: A500 profile (PAL and NTSC merge)...\n");
  // PAL (reg_chip bit 1 = 0)
  ok = resolve_profile_lookup(PROFILE_A500, 0x00, 0x07, 0x53, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_A500);
  assert(eff_chip == CHIPSET_OCS_PAL);
  assert(eff_cpu == CPU_68000_NOCACHE);
  assert(eff_mem == MEM_512K_CHIP_512K_SLOW);
  assert(eff_ide == false);

  // NTSC (reg_chip bit 1 = 1)
  ok = resolve_profile_lookup(PROFILE_A500, 0x02, 0x07, 0x53, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(eff_chip == (CHIPSET_OCS_PAL | 0x02));
  assert(eff_cpu == CPU_68000_NOCACHE);
  assert(eff_mem == MEM_512K_CHIP_512K_SLOW);

  printf("Running test: A500 Plus profile...\n");
  ok = resolve_profile_lookup(PROFILE_A500_PLUS, 0x00, 0x03, 0x03, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_A500_PLUS);
  assert(eff_chip == CHIPSET_ECS_PAL);
  assert(eff_cpu == CPU_68000_NOCACHE);
  assert(eff_mem == MEM_1M_CHIP);
  assert(eff_ide == false);

  printf("Running test: A600 profile (with IDE)...\n");
  ok = resolve_profile_lookup(PROFILE_A600, 0x00, 0x03, 0x03, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_A600);
  assert(eff_chip == CHIPSET_ECS_PAL);
  assert(eff_cpu == CPU_68000_NOCACHE);
  assert(eff_mem == MEM_1M_CHIP);
  assert(eff_ide == true);

  printf("Running test: A1200 profile (stock 020 AGA, no cache)...\n");
  ok = resolve_profile_lookup(PROFILE_A1200, 0x00, 0x00, 0x00, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_A1200);
  assert(eff_chip == CHIPSET_AGA_PAL);
  assert(eff_cpu == CPU_020_NOCACHE);
  assert(eff_mem == MEM_2M_CHIP);
  assert(eff_ide == false);

  printf("Running test: A1200 Accelerated profile (020 AGA, 32MB Fast, cache enabled)...\n");
  // Test PAL
  ok = resolve_profile_lookup(PROFILE_A1200_ACCEL, 0x00, 0x00, 0x00, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_A1200_ACCEL);
  assert(eff_chip == CHIPSET_AGA_PAL);
  assert(eff_cpu == CPU_020_CACHE);
  assert(eff_mem == MEM_2M_CHIP_32M_FAST);
  assert(eff_ide == false);

  // Test NTSC
  ok = resolve_profile_lookup(PROFILE_A1200_ACCEL, 0x02, 0x00, 0x00, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(eff_chip == (CHIPSET_AGA_PAL | 0x02));
  assert(eff_cpu == CPU_020_CACHE);
  assert(eff_mem == MEM_2M_CHIP_32M_FAST);

  printf("Running test: AmigaVision profile (020 AGA, 32MB Fast, no cache)...\n");
  ok = resolve_profile_lookup(PROFILE_AMIGAVISION, 0x00, 0x07, 0x00, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(ok);
  assert(prof != nullptr && prof->id == PROFILE_AMIGAVISION);
  assert(eff_chip == CHIPSET_AGA_PAL);
  assert(eff_cpu == CPU_020_NOCACHE);
  assert(eff_mem == MEM_2M_CHIP_32M_FAST);
  assert(eff_ide == false);

  printf("Running test: Unknown profile ID fallback...\n");
  ok = resolve_profile_lookup(99, 0x11, 0x03, 0x22, &eff_chip, &eff_cpu, &eff_mem, &eff_ide, &prof);
  assert(!ok);
  assert(prof == nullptr);
  assert(eff_chip == 0x11);
  assert(eff_cpu == 0x03);
  assert(eff_mem == 0x22);
  assert(eff_ide == false);

  printf("All machine profile tests passed successfully!\n");
  return 0;
}
