# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Bolt PR #25 opened: ide_check() dead code removal
**New PR:** #25 `bolt/perf-ide-check-dead-code` — remove provably dead `spi_w(0)` from `ide_check()`. Cross-examined C source, compiled RISC-V assembly, AND FPGA Verilog (`hps_ext.v`) to prove that `spi_w(UIO_DMA_SDIO)` returns `0xE000 | ide_req` (bit 15 always set), making `if (!res)` always false and the second `spi_w(0)` unreachable. Removes a branch instruction and dead code from every mainloop iteration.

**Approach:** SUPER BOLT cross-layer verification — C + RISC-V asm + FPGA Verilog to prove code is provably dead, not just count MMIO reads.

**Verification:** Idiot-savant level — host model tests verify identical behavior for all 64 `ide_req` values; bit extraction for `ide_io()` and `ide.cpp` wait loops unchanged; second `spi_w(0)` never called. All host tests pass.

All bolt PRs against `ollisulopuisto/Analogue-Amiga`:
- #13 `bolt/perf-osd-keyboard-mmio-reads` (merged): 12→2 / 23→2 MMIO reads in OSD keyboard poll
- #14 `bolt/perf-keyboard-register-snapshot` (merged): 44→3 MMIO reads per keyboard poll
- #15 `bolt/perf-apf-timeout-spin` (merged): 101→1 / 10001→1 APF handshake polls (~90×)
- #16 `bolt/perf-fdd-sector-pointer` (merged): 5632→0 bytes copied per floppy track read
- #21 `bolt/perf-dataslot-search-cache` (merged): up to 128→0 MMIO reads per dataslot lookup
- #22 `bolt/perf-dataslot-size-cache` (open): 33→0 MMIO reads per dataslot_size() call
- #23 `bolt/perf-core-poll-io-dummy-reads` (open): 3→1 spi_w transactions per mainloop iteration
- #24 `bolt/perf-input-reg-poll-interval` (open): 1→0.25 MMIO reads per mainloop iteration (75% reduction)
- #25 `bolt/perf-ide-check-dead-code` (open): provably dead spi_w(0) removed from ide_check()

Remaining candidate evaluation: OSD SPI hardware-limited (1 byte/strobe, menu-only). `mouse_math.h`/`pocket_shortcuts.h` marginal. `put_lba()` CHS divisions only affect non-LBA mode (rare).
**Decision:** No safe measurable targets remain on `main` beyond PR #25.

## 2026-11-08 - MPU firmware can be built and disassembled locally
**Learning:** The riscv toolchain is NOT unavailable: CI's xPack `riscv-none-elf-gcc` 15.2.0-1 tarball (see `.github/workflows/ci.yml`) unpacks into `~/opt` and builds `amiga_mpu_bios` with the same `make GCC_PREFIX=riscv-none-elf- EXTRA_CFLAGS=...` line. Build in a copy of the repo, because `src/MPUBIOS/app/{build,dep,obj}.riscv.*` are committed. `riscv-none-elf-objdump -d` of the ELF gives exact per-iteration instruction counts for the real target, which is a better metric than host timing.
**Action:** For firmware CPU-bound loops, measure baseline/after as target instruction counts from the disassembly. Use host timing only as a secondary signal.

## 2026-11-08 - Remaining candidates rejected after PR #26
**Learning:** These remaining targets are protocol-risky or marginal:
- `HPS_spi_write()` → `HPS_spi_write_fast()` on the CD/HDD data path: the original author left the fast, ACK-less variant commented out, and the `asm volatile("nop")` padding in `HPS_fpga_spi()` is load-bearing.
- `SendSector()` odd/even MFM loops (16-bit packing): dominated by the per-word `spi_w()` handshake, and `fdd.cpp` is already touched by PR #26.
- `lib/memset.c` is a byte loop, but its only callers are a one-time init, a 32-byte buffer, and the rare IDE null path.
- `cue_t::GetTrackByLBA()` runs a linear search of at most 99 tracks, once per command rather than per sector.
**Action:** Revisit these only with on-hardware timing.
