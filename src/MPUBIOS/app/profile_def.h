#ifndef _PROFILE_DEF_H_
#define _PROFILE_DEF_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Machine Profile definitions (Plan A/B - Named Model Profiles & Accelerated A1200)
// ============================================================================
//
// Profile identifiers (0 = Custom, pass-through existing settings).
// Stable identifiers: do not repurpose released values.
//
// Register field encodings (as consumed by userio.v / minimig.v):
//
//   chipset_config (reg 2, 0xF0000008, 5 bits):
//     bit 0 = Turbo (0 for profiles)
//     bit 1 = NTSC (merged from existing video-standard choice)
//     bit 2 = A1000 mode (0 for profiles)
//     bit 3 = ECS
//     bit 4 = AGA
//     Hardware decodes: ecs = |chipset_config[4:3], aga = chipset_config[4]
//
//   cpu_config (reg 3, 0xF000000C, 5 bits):
//     bits[1:0] = CPU type (00=68k/fx68k, 01=68k10/fx68k, 11=68k20/TG68K)
//     bit 2     = Cache enable
//     bit 3     = Fast kickstart
//
//   memory_config (reg 4, 0xF0000010, 8 bits):
//     bits[1:0] = Chip RAM (0=512K, 1=1M, 2=1.5M, 3=2M)
//     bits[3:2] = Slow RAM (0=None, 1=512K, 2=1M, 3=1.5M)
//     bits[5:4] = Fast RAM (0=None, 1=2M, 2=4M, 3=8M, 4=16M, 5=32M)
//

#define PROFILE_CUSTOM       0
#define PROFILE_A500         1
#define PROFILE_A500_PLUS    2
#define PROFILE_A600         3
#define PROFILE_A1200        4
#define PROFILE_A1200_ACCEL  5
#define PROFILE_AMIGAVISION  6

// Chipset base values (without NTSC bit and without Turbo)
#define CHIPSET_OCS_PAL      0x00  // OCS, PAL, no turbo
#define CHIPSET_ECS_PAL      0x08  // ECS (bit 3), PAL, no turbo
#define CHIPSET_AGA_PAL      0x10  // AGA (bit 4) + ECS, PAL, no turbo

// CPU config values
#define CPU_68000_NOCACHE    0x00  // TT=00, C=0
#define CPU_020_NOCACHE      0x03  // TT=11, C=0  (menu "68020 CPU - No Cache")
#define CPU_020_CACHE        0x07  // TT=11, C=1  (menu "68020 CPU - Cache")

// Memory config values
#define MEM_512K_CHIP_512K_SLOW   0x04  // Chip=0, Slow=1 at [3:2]
#define MEM_1M_CHIP               0x01  // Chip=1
#define MEM_2M_CHIP               0x03  // Chip=3
#define MEM_2M_CHIP_32M_FAST      0x53  // Chip=3, Fast=5 at [5:4]

struct ProfileDef {
  uint8_t  id;
  const char *name;
  uint8_t  chipset_base;   // chipset config without NTSC/turbo
  uint8_t  cpu_config;     // CPU type + cache
  uint8_t  memory_config;  // Chip/Slow/Fast RAM
  bool     ide_enable;     // Profile enables IDE interface
  const char *rom_family;  // Recommended Kickstart family
  const char *notes;       // Compatibility notes
};

// Authoritative profile table. Stored in MPU firmware near config code.
static const struct ProfileDef profile_table[] = {
  { PROFILE_CUSTOM,       "Custom",            0x00, 0x00, 0x00, false, "User-defined",      "Passes through existing menu settings" },
  { PROFILE_A500,         "A500",              CHIPSET_OCS_PAL, CPU_68000_NOCACHE, MEM_512K_CHIP_512K_SLOW, false, "Kickstart 1.3 (OCS)", "OCS chipset; 512K Chip + 512K Slow RAM" },
  { PROFILE_A500_PLUS,    "A500 Plus",         CHIPSET_ECS_PAL, CPU_68000_NOCACHE, MEM_1M_CHIP,           false, "Kickstart 2.0+ (ECS)", "ECS chipset; 1M Chip RAM" },
  { PROFILE_A600,         "A600",              CHIPSET_ECS_PAL, CPU_68000_NOCACHE, MEM_1M_CHIP,           true,  "Kickstart 2.0+ (ECS)", "ECS chipset; 1M Chip RAM; IDE interface" },
  { PROFILE_A1200,        "A1200",             CHIPSET_AGA_PAL, CPU_020_NOCACHE,   MEM_2M_CHIP,           false, "Kickstart 3.0/3.1 (AGA)", "AGA chipset; 020-compatible; 2M Chip RAM" },
  { PROFILE_A1200_ACCEL,  "A1200 Accelerated", CHIPSET_AGA_PAL, CPU_020_CACHE,     MEM_2M_CHIP_32M_FAST,  false, "Kickstart 3.0/3.1 (AGA)", "AGA chipset; 020-compatible with cache; 2M Chip + 32M Fast RAM" },
  { PROFILE_AMIGAVISION,  "AmigaVision",       CHIPSET_AGA_PAL, CPU_020_NOCACHE,   MEM_2M_CHIP_32M_FAST,  false, "Kickstart 3.0/3.1 (AGA)", "AGA chipset; 020-compatible; 2M Chip + 32M Fast RAM; cache disabled" }
};

#define PROFILE_TABLE_SIZE (sizeof(profile_table) / sizeof(profile_table[0]))

// Resolve the effective machine configuration from a profile identifier and
// the current menu register snapshot.
static inline bool resolve_profile_lookup(uint8_t profile_id, uint8_t reg_chip, uint8_t reg_cpu,
                                          uint8_t reg_mem, uint8_t *eff_chip, uint8_t *eff_cpu,
                                          uint8_t *eff_mem, bool *eff_ide, const struct ProfileDef **out_prof) {
  *eff_chip = reg_chip;
  *eff_cpu  = reg_cpu;
  *eff_mem  = reg_mem;
  *eff_ide  = false;
  if (out_prof) *out_prof = 0;

  const struct ProfileDef *prof = 0;
  for (uint32_t i = 0; i < PROFILE_TABLE_SIZE; i++) {
    if (profile_table[i].id == profile_id) {
      prof = &profile_table[i];
      break;
    }
  }

  if (out_prof) *out_prof = prof;

  if (prof == 0) {
    return false; // Unknown profile
  }

  if (prof->id == PROFILE_CUSTOM) {
    return true; // Custom profile: pass-through
  }

  // Named profile: apply defined machine fields.
  // Merge NTSC (bit 1) from the independent video-standard choice.
  // Turbo (bit 0) is forced to 0 for named profiles.
  *eff_chip = prof->chipset_base | (reg_chip & 0x02);
  *eff_cpu  = prof->cpu_config;
  *eff_mem  = prof->memory_config;
  *eff_ide  = prof->ide_enable;

  return true;
}

#ifdef __cplusplus
}
#endif

#endif // _PROFILE_DEF_H_
