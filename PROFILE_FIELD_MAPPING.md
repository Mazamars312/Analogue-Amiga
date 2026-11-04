# Amiga Machine Profile - Field Mapping and Audit

This document records the audit of each field from menu register to FPGA consumer,
as required by Plan A1-A2 of AMIGA_MODEL_AND_ACCELERATION_PLAN.md.

## 1. Register Bridge Map

The Pocket menu writes to 8 bridge registers at 0xF0000000-0xF000001C.
The FPGA `substitute_mcu_apf.v` latches these into `mpu_reg_0` through `mpu_reg_7`.
The MPU firmware reads them via `AFP_REGISTOR(n)` at 0xFFFFFF00+(n<<2).

| Reg | Address | Menu Field(s) | Mask | Used by |
|-----|---------|---------------|------|---------|
| 0 | 0xF0000000 | Reset Amiga (id 120) | 0xFFFFFFFE | `UIO_MM2_RST` |
| 1 | 0xF0000004 | Floppy Drives (id 122) | varies | `UIO_MM2_FLP` |
| 2 | 0xF0000008 | System Configuration (id 127) | 0xFFFFFFEC | `UIO_MM2_CHIP` |
| 3 | 0xF000000C | CPU Type (id 128) | 0xFFFFFFF8 | `UIO_MM2_CPU` |
| 4 | 0xF0000010 | Memory: Chip (129), Slow (130), Fast (131) | varies | `UIO_MM2_MEM` |
| 5 | 0xF0000014 | Port 1/Port 2 (123), Mouse Speed (124) | varies | `UIO_MM2_JOY` |
| 6 | 0xF0000018 | **UNUSED** → **Machine Profile (id 140)** | — | (new) |
| 7 | 0xF000001C | FDD/HDD LED (121), Audio Filter (132), Audio Mix (133) | varies | (audio/led) |

Register 6 (0xF0000018, mpu_reg_6) was previously unused and is now allocated
to the Machine Profile selector.

### Mask semantics

Based on observed entries, mask bit `0` = writable by menu, mask bit `1` =
read-only/preserved. For profile IDs 0-6 (3 bits), the mask must be
`0xFFFFFFF8` (same as CPU Type).

## 2. Field Encodings

### 2.1 System Configuration (reg 2, 0xF0000008) — 5 significant bits

Menu "System Configuration" (id 127), defaultval 6 (0x0006).

| Menu value | Name | Bit 4 (AGA) | Bit 3 | Bit 2 (A1K) | Bit 1 (NTSC) | Bit 0 (Turbo) |
|------------|------|-------------|-------|-------------|--------------|---------------|
| 0x0000 | ECS/PAL/Std | 0 | 0 | 0 | 0 | 0 |
| 0x0002 | ECS/NTSC/Std | 0 | 0 | 0 | 1 | 0 |
| 0x0001 | ECS/PAL/Turbo | 0 | 0 | 0 | 0 | 1 |
| 0x0003 | ECS/NTSC/Turbo | 0 | 0 | 0 | 1 | 1 |
| 0x0010 | AGA/PAL/Std | 1 | 0 | 0 | 0 | 0 |
| 0x0012 | AGA/NTSC/Std | 1 | 0 | 0 | 1 | 0 |
| 0x0011 | AGA/PAL/Turbo | 1 | 0 | 0 | 0 | 1 |
| 0x0013 | AGA/NTSC/Turbo | 1 | 0 | 0 | 1 | 1 |

### 2.1.1 Chipset label discrepancy (FOUND)

**The menu labels bit-3-clear values as "ECS", but `minimig.v` decodes:**

```
ecs = |chipset_config[4:3]   // ECS if bit 4 OR bit 3 is set
aga = chipset_config[4]       // AGA if bit 4 is set
a1k = chipset_config[2]       // A1000 mode if bit 2 is set
ntsc <= chipset_config[1]     // NTSC if bit 1 is set
```

Consequences:
- Menu `0x0000` (labeled "ECS/PAL/Std") → hardware `ecs=0, aga=0` → **OCS**, not ECS.
- Menu `0x0010` (labeled "AGA/PAL/Std") → hardware `ecs=1, aga=1` → AGA with ECS also asserted.
- No menu option sets bit 3 alone (pure ECS without AGA). Bit 3 is available for profile use.

**Resolution:** Named profiles use explicit encodings:
- OCS: `chipset_base = 0x00` (bits [4:3] = 00 → ecs=0)
- ECS: `chipset_base = 0x08` (bit 3 set → ecs=1 via |4:3])
- AGA: `chipset_base = 0x10` (bit 4 set → ecs=1 AND aga=1)

NTSC is merged from `reg_chip & 0x02` (bit 1 of existing menu setting).
Turbo (bit 0) is forced to 0 for named profiles.

### 2.2 CPU Configuration (reg 3, 0xF000000C) — 5 significant bits

Menu "CPU Type" (id 128), defaultval 3 (0x0003).

| Menu value | Name | Bits[1:0] (TT) | Bit 2 (C) | Bit 3 (K) | Actual CPU core |
|------------|------|----------------|-----------|-----------|-----------------|
| 0x0000 | 68000 CPU | 00 | 0 | 0 | fx68k (cpucfg[1]=0) |
| 0x0001 | 68010 CPU | 01 | 0 | 0 | fx68k (cpucfg[1]=0) — **no distinct 68010 core** |
| 0x0003 | 68020 No Cache | 11 | 0 | 0 | TG68K (cpucfg[1]=1) |
| 0x0007 | 68020 Cache | 11 | 1 | 0 | TG68K with cache |

**Audit finding:** The 68010 menu entry does not select a distinct CPU core.
Both 68000 and 68010 route to `fx68k` because `cpu_wrapper.v` selects TG68K
only when `cpucfg[1]` is set (TT=11). TT=00 and TT=01 both use fx68k.

Named profile CPU encodings:
- 68000 (A500/A500+/A600): `0x00` (TT=00, C=0)
- 020 No Cache (A1200, AmigaVision): `0x03` (TT=11, C=0)
- 020 Cache (A1200 Accelerated): `0x07` (TT=11, C=1)

### 2.3 Memory Configuration (reg 4, 0xF0000010) — 8 bits

Shared by Memory: Chip (id 129), Slow (id 130), Fast (id 131).

| Bits | Field | Values |
|------|-------|--------|
| [1:0] | Chip RAM | 0=512K, 1=1M, 2=1.5M, 3=2M |
| [3:2] | Slow RAM | 0=None, 1=512K, 2=1M, 3=1.5M |
| [6:4] | Fast RAM | 0=None, 1=2M, 2=4M, 3=8M, 4=16M, 5=32M |
| [7] | HRTmon enable | Not used by profiles |

(Fast RAM is three bits at [6:4], not [5:4] as first drafted - `core_top.v`
wires `fastramcfg = memcfg[6:4]`. The profile values were already correct;
only this table and the firmware comments were wrong.)

Default: Chip=3 (2M), Slow=0 (None), Fast=5 (32M) → `0x53`

Named profile memory encodings:
- A500: Chip=0 (512K), Slow=1 (512K), Fast=0 → `0x04`
- A500 Plus: Chip=1 (1M), Slow=0, Fast=0 → `0x01`
- A600: Chip=1 (1M), Slow=0, Fast=0 → `0x01`
- A1200: Chip=3 (2M), Slow=0, Fast=0 → `0x03`
- A1200 Accelerated: Chip=3 (2M), Slow=0, Fast=5 (32M) → `0x53`
- AmigaVision: Chip=3 (2M), Slow=0, Fast=5 (32M) → `0x53`

## 3. CPU Selection Path (audit)

```
menu CPU Type (0xF000000C)
    → AFP_REGISTOR(3) → mpu_reg_3
    → MPU sends UIO_MM2_CPU with value
    → userio.v: t_cpu_config <= IO_DIN[4:0] (on cmd==3)
    → userio.v: cpu_config <= t_cpu_config[1:0] (latched during reset)
    → userio.v: cache_config <= t_cpu_config[4:2] (continuous, NOT reset-latched)
    → minimig.v: cpucfg = cpu_config
    → cpu_wrapper.v:
        if (cpucfg[1]) → TG68KdotC_Kernel
        else            → fx68k
```

**Live cache update issue — RESOLVED:** `cache_config` was updated continuously (not just
during reset), so a cache menu change took effect immediately and violated the "staged
until reset" principle. `userio.v` now latches `cache_config[2:0]` from `t_cpu_config[4:2]`
inside the same `if (reset)` block as `chipset_config`, `cpu_config`, `memory_config`, and
`ide_config`. All machine-config outputs are now staged until reset. The menu-facing
`t_cpu_config` staging register can still be written at any time via `UIO_MM2_CPU`; it only
takes effect on the next reset.

## 4. MPU Configuration Command Sequence

`minimig_restart_first()` (boot) and `minimig_restart_running_core()` (reset) both send:

| Command | Register | Source |
|---------|----------|--------|
| UIO_MM2_FLP (0xF6) | mpu_reg_1 (0xF0000004) | Floppy |
| UIO_MM2_CHIP (0xF2) | mpu_reg_2 (0xF0000008) | Chipset → **profile-resolved** |
| UIO_MM2_CPU (0xF3) | mpu_reg_3 (0xF000000C) | CPU → **profile-resolved** |
| UIO_MM2_MEM (0xF4) | mpu_reg_4 (0xF0000010) | Memory → **profile-resolved** |
| UIO_MM2_JOY (0xF8) | mpu_reg_5 (0xF0000014) | Joystick |

Then `minimig_fdd_update()` and `minimig_hdd_update()` are called (boot only for fdd_update).

The profile resolver intercepts regs 2-4 and substitutes profile-defined values when
a named profile is selected. Regs 1 and 5 always pass through.

## 5. Reset Sequence

```
1. User selects profile in menu → written to mpu_reg_6 (0xF0000018)
2. User presses "Reset Amiga" → AFP_REGISTOR(0) bit 0 set
3. MPU mainloop() detects reset request:
   a. Asserts core reset
   b. Clears AFP_REGISTOR(0)
   c. Calls minimig_restart_running_core()
      - Reads mpu_reg_6 for profile ID
      - Resolves effective chip/cpu/mem config
      - Sends UIO_MM2_CHIP/CPU/MEM commands with resolved values
   d. Deasserts core reset
4. userio.v latches configs while reset is active (existing mechanism)
5. Amiga starts with new configuration
```

Configs are already staged until reset because `userio.v` only latches
`chipset_config`, `cpu_config`, `memory_config` when `reset` is active.

## 6. Profile Table

| ID | Name | CPU | Chipset | Chip | Slow | Fast | Cache | IDE | ROM |
|----|------|-----|---------|------|------|------|-------|-----|-----|
| 0 | Custom | User | User | User | User | User | User | — | User |
| 1 | A500 | 68000 | OCS | 512K | 512K | None | No | No | KS 1.3 |
| 2 | A500 Plus | 68000 | ECS | 1M | None | None | No | No | KS 2.0+ |
| 3 | A600 | 68000 | ECS | 1M | None | None | No | Auto* | KS 2.0+ |
| 4 | A1200 | 020 | AGA | 2M | None | None | No | No | KS 3.0/3.1 |
| 5 | A1200 Accel | 020 | AGA | 2M | None | 32M | Yes | No | KS 3.0/3.1 |
| 6 | AmigaVision | 020 | AGA | 2M | None | 32M | No | No | KS 3.0/3.1 |

\* **IDE is not profile-controlled.** `minimig_hdd_update()` sends
`UIO_MM2_HDD` with Gayle/master/slave enable bits derived from which HDF data
slots (310-313) are occupied, on every profile. The resolver therefore carries
no IDE flag (the initial draft's `ide_enable` was dead code and was removed);
the A600 profile's "IDE" is descriptive - mount an HDF and the interface is on.

## 7. Compatibility Limits (documented differences)

- **A500**: Core is fx68k (68000-compatible), not a cycle-exact 68000. OCS chipset
  in hardware (menu label discrepancy noted above).
- **A500 Plus / A600**: Same fx68k core. ECS via bit 3 of chipset_config.
- **A1200**: TG68K 020-compatible, not a full 68020. No MMU, no FPU. AGA chipset.
  Differs from stock EC020 timing and address behavior.
- **A1200 Accelerated**: Same CPU as A1200 with cache enabled and 32MB Fast RAM.
  Cache behavior may differ from a physical 020 cache.
- **AmigaVision**: Same as A1200 Accelerated hardware but with cache disabled.

No profile implies A1000 boot ROM, A3000 SCSI/MMU, A4000 CPU, CDTV, or CD32
peripherals have been implemented.

## 8. Files Changed

| File | Change |
|------|--------|
| `dist/Cores/Mazamars312.Amiga/interact.json` | Added Machine Profile menu entry (id 140, addr 0xF0000018) |
| `dist/Settings/Mazamars312.Amiga/Interact/_core/interact_persist.json` | Added profile persistence (id 140, default 0=Custom) |
| `src/MPUBIOS/app/machine_profiles.h` | Profile table + resolver, shared between firmware and host tests (extracted from minimig_config.cpp; dead `ide_enable` removed) |
| `src/MPUBIOS/app/minimig_config.cpp` | Both restart functions resolve via `resolve_profile()` |
| `tests/test_machine_profiles.cpp` | Host unit tests: encodings, NTSC merge, turbo clear, Custom pass-through, unknown-ID fallback (Plan A6) |
| `dist/Cores/Mazamars312.Amiga/amiga_mpu_bios.bin` | Rebuilt with the profile resolver (previous binary predated it) |
| `.github/workflows/ci.yml`, `Dockerfile` | Run the resolver tests in CI |
| `.github/workflows/build.yml` | Push trigger limited to bitstream-affecting paths; PRs still always build |
| `README.md` | User-facing Machine Profiles section |
| `PROFILE_FIELD_MAPPING.md` | This document |

## 9. Build Verification

- Host unit tests (`tests/test_machine_profiles.cpp`) compile and pass with
  g++ -std=c++14; they exercise the same `machine_profiles.h` the firmware
  includes.
- Firmware rebuilt with `riscv-none-elf-gcc` 15.2.0 (xPack), same flags as
  `.github/workflows/ci.yml`: builds clean, 81504 B of 128 KB RAM used, and
  the profile strings are present in `amiga_mpu_bios.bin`. The rebuilt binary
  is committed to `dist/` (the previously committed binary predated the
  profile feature).
- Pocket hardware validation (cold boot, Reset Amiga per profile, PAL/NTSC,
  memory layouts) is still outstanding - see Plan A6. Nothing here has been
  tested on a physical Pocket.
