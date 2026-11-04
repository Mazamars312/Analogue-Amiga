# Plan B Baseline — Accelerated A1200 Support

**Status:** In progress (B1–B3: characterization)

This document records the baseline architecture and measurement environment for
Plan B. Quartus and Pocket hardware are NOT available in this environment
(`quartus`/`quartus_sh` not found; PLL and build host are on a separate Windows
machine per `LOCAL_BUILD_NOTES.md`). All characterization below is therefore
RTL/audit-based and simulation-ready; the actual FPGA fit, timing, and hardware
validation must be completed on the Quartus host.

---

## 1. Clock architecture (from core_top.v, amiga_clk.v, pll.v/qip)

| Clock | Source | Frequency | Drives |
|---|---|---|---|
| `clk_74a` | External input | 74.25 MHz | PLL reference |
| `clk_114` | PLL outclk_0 | ~113.5 MHz | SDRAM `sysclk`, `fastchip`, CPU divider (`div` counter) |
| `clk_sys` | PLL outclk_1 | ~28.375 MHz | TG68K `clk`, `minimig`, `amiga_clk`, `cpu_wrapper` |
| `c_7m` | `amiga_clk` (clk_28/4 enable) | ~7.09 MHz | SDRAM state-machine gate, chipset timing |
| `video_rgb_clock` | PLL outclk_2 | — | Video pipeline |
| `video_rgb_clock_90` | PLL outclk_3 | — | Video pipeline |

PLL: Cyclone V `altera_pll`, fractional-N, refclk=74.25 MHz, M=9 (frac), N=1,
C0=6 → outclk_0≈113.5 MHz; C1=24 → outclk_1≈28.375 MHz.

`amiga_clk` divides `clk_sys` by 4 via `clk7_cnt` to produce `clk7_en`
(~7.09 MHz pulse) and `clk_7 = clk7_cnt[1]` (~14.2 MHz square, used as enable).
`c1`/`c3` are 90°-offset derivatives of `clk_7` for chipset sampling.

---

## 2. CPU execution path (cpu_wrapper.v, TG68KdotC_Kernel.vhd)

- **Active core:** TG68KdotC_Kernel when `cpucfg[1]==1` (68020 config), fx68k
  otherwise (68000/68010 both route to fx68k — no distinct 68010 core).
- TG68K runs on `clk_sys` (~28.375 MHz) with clock enable:
  `clkena_in = ~cpu_req | chipready | ramready | fastchip_ready`
- CPU stalls when waiting for `chipready` (chip RAM), `ramready` (Fast RAM via
  SDRAM), or `fastchip_ready` (IDE/RTG/Akiko).
- **16-bit data path** through the wrapper; longword = 2 bus cycles; unaligned
  accesses split further.
- `cpu_req = (cpustate != 1)` — CPU requests bus when not in "no mem access"
  state.
- `cacr` (4 bits) drives `turbochip_d`/`turbokick_d`/`dcache_d` (cachecfg bits
  [2:0]) — chip RAM burst, kickstart burst, data cache enables.

---

## 3. SDRAM controller (sdram_ctrl.v)

- Runs on `clk_114` (~113.5 MHz), state machine gated by `c_7m` (~7 MHz).
- 16-state cycle (0–15), advances every `clk_114`, resets to 0 on `c_7m`
  rising edge → effectively one SDRAM cycle per ~4 clk_114 cycles.
- **Arbitration priority (state 0):**
  1. Chipset DMA (`~chipDMA | ~chipRW`) — highest priority
  2. CPU write buffer (`write_req`)
  3. CPU read cache (`cache_req`)
  4. Refresh (`&rcnt`)
- Chipset gets first access to every SDRAM cycle; CPU only gets the slot if
  chipset is idle.
- Cache line fill: 4 reads at states 7, 9, 11, 13 → 4 cycles per fill.
- Write buffer: single entry, lets CPU continue while write completes.
- `ramready = cache_rd_ack || write_ena` — CPU ack on cache hit or write-buffer
  available.

---

## 4. Cache (cpu_cache_new.v)

- **2-way set-associative, separate I and D caches**
- **Total: 4 KB** (2 KB instruction + 2 KB data)
- **1024 sets, 16-byte lines** (4 × 16-bit words), 18-bit tag
- Write-through, look-through, LRU replacement
- CPU-side FSM: IDLE → READ/WRITE → (miss) FILL1–FILL4 → WB/IDLE
- Fill penalty: 4 cycles (one word per `sdr_read_ack`)
- SDRAM-side FSM handles snoops (chipset writes update matching cache lines)
- Cache inhibit bit (`cpu_cache_ctrl[3]`) bypasses cache fill

---

## 5. Memory mapping (cpu_wrapper.v)

| Range | Device | Notes |
|---|---|---|
| $000000–$1FFFFF | Chip RAM | `cchip`, up to 2 MB |
| $E00000–$FFFFFF | Kickstart ROM | `ckick`, mirrored |
| $E80000–$EFFFFF | Zorro II autoconfig | `sel_autoconfig` |
| $200000–$1FFFFFF | Zorro III RAM | `cfg_z3`, up to 256 MB |
| $00DD010–$00DD01F | Gayle/IDE | `sel_dd` |
| $02xxxxxx | RTG | `sel_rtg` |

---

## 6. Bottleneck candidates (identified from RTL audit)

| # | Candidate | Evidence from RTL | Priority |
|---|---|---|---|
| 1 | SDRAM arbitration delay | Chipset DMA has absolute priority (sdram_ctrl state 0); CPU waits during display/audio/disk DMA | High |
| 2 | 16-bit data path | Wrapper routes 16-bit; longword = 2 cycles, unaligned = more | High |
| 3 | Cache capacity | 4 KB total; working-set misses → 4-cycle fill | Medium |
| 4 | Clock enables | TG68K stalls on `chipready`/`ramready`/`fastchip_ready` | Medium |
| 5 | Cache line fill latency | 4 cycles per fill at ~7 MHz effective SDRAM rate | Medium |

---

## 7. Measurement plan (simulation, local)

Quartus and Pocket hardware are unavailable here, so baseline measurement is
done with RTL simulation (`iverilog` + `verilator` available):

1. **Cache testbench** — drive `cpu_cache_new` with synthetic access streams
   (sequential, strided, random, self-modifying) at various working-set sizes;
   measure hit rate, fill cycles, stall cycles.
2. **SDRAM arbitration testbench** — drive `sdram_ctrl` with simultaneous
   chipset DMA and CPU traffic; measure CPU wait cycles under load.
3. **End-to-end wrapper testbench** — drive `cpu_wrapper` + TG68K with a small
   instruction loop; measure cycles per instruction vs. memory wait.

Each benchmark runs with cache on/off and at each advertised Fast RAM size.

---
## 7. Simulation baseline results (local, no Quartus needed)

### Cache simulation (`planB/sim/cache_tb.v`)
Cache: 2-way set-assoc, 256 sets x 2 ways x 16-byte lines = 8 KB total
       Write-through, look-through, LRU. Fill = 4 beats.

| Metric | Value |
|---|---|
| Total accesses | 3520 |
| Cache misses | 1537 (43.66%) |
| Cache hits | 1983 (56.34%) |
| Fill cycles | 10759 |
| Total cycles | 207120 |
| Cycles/access | 58.84 |

Key observations:
- Sequential code with small working set: high hit rate after warmup
- Strided access (8KB working set, stride 256): most accesses miss
  (only 8 unique cache sets indexed by stride-256 in 256-set 2-way cache)
- Cache OFF: every read goes to SDRAM
- Fill penalty: 4 cycles per cache line miss

### SDRAM arbitration simulation (`planB/sim/sdram_arb_simple_tb.v`)
Model of sdram_ctrl.v state-0 priority: `if(~chipDMA|~chipRW) slot=CHIP`

| Test | Condition | Accesses | Wait clk/access | Max stall clk |
|---|---|---|---|---|
| 1 | Periodic chip DMA (every 8 clk) | 32 | ~108 | 241 |
| 2 | Continuous chip READ DMA | 32 | ~115 | 241 |
| 3 | Burst (16 pulses) then 1 CPU read | 1 | 31 | — |
| 4 | Continuous chip READ DMA | 100 | ~122 | 247 |
| 5 | Continuous chip WRITE DMA | 32 | ~113 | 247 |

Key observations:
- CPU waits ~100-125 clk cycles (~0.9-1.1 µs at 113.5 MHz) per access under DMA
- Max stall ~241-247 clk cycles (~2.2 µs)
- Chip READ (chipDMA=1,chipRW=1): `~chipDMA|~chipRW`=0 → CPU gets state 0 priority
- Chip WRITE (chipDMA=1,chipRW=0): `~chipDMA|~chipRW`=1 → chip wins state 0
- Idle (chipDMA=0,chipRW=1): `~chipDMA|~chipRW`=1 → chip gets "priority" (wasted slot)

---

## 8. What requires the Quartus host (B1 hardware headroom)

- Full `ap_core` compile (Quartus 25.1std, Cyclone V 5CEBA4F23C8)
- FPGA resource utilization (LEs, M9K/M10K blocks, registers)
- Timing analysis (fmax per clock domain, setup/hold slack)
- Clock constraint audit (`.sdc` files)
- Bitstream generation and Pocket hardware flashing
- On-hardware benchmark execution and measurement

**Next step:** On the Quartus host, build `ap_core` and capture resource/utilization/timing baselines. Then proceed to B2 (measure current config on hardware) and B3 (identify limiting path).
