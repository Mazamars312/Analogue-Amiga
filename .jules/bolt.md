# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Bolt PR #24 opened: input_reg_update() poll interval
**New PR:** #24 `bolt/perf-input-reg-poll-interval` — poll joystick mode (`AFP_REGISTOR(5)`) every 4th mainloop iteration instead of every iteration. Totally different approach: time-based polling instead of value-based caching. 75% reduction in `AFP_REGISTOR(5)` MMIO reads with imperceptible detection delay (within 4 iterations).

**Approach:** SUPER BOLT with psychedelic imagination — examined compiled RISC-V machine code and FPGA Verilog cross-layer, identified that `input_reg_update()` was reading a rarely-changing register on every mainloop iteration, and replaced per-iteration polling with interval-based polling.

**Verification:** Idiot-savant level — host model tests verify: changes detected within POLL_INTERVAL polls, SPI commands sent on every change, no spurious SPI when stable, exactly 75% read reduction over 10,000 iterations. All host tests pass.

All bolt PRs against `ollisulopuisto/Analogue-Amiga`:
- #13 `bolt/perf-osd-keyboard-mmio-reads` (merged): 12→2 / 23→2 MMIO reads in OSD keyboard poll
- #14 `bolt/perf-keyboard-register-snapshot` (merged): 44→3 MMIO reads per keyboard poll
- #15 `bolt/perf-apf-timeout-spin` (merged): 101→1 / 10001→1 APF handshake polls (~90×)
- #16 `bolt/perf-fdd-sector-pointer` (merged): 5632→0 bytes copied per floppy track read
- #21 `bolt/perf-dataslot-search-cache` (merged): up to 128→0 MMIO reads per dataslot lookup
- #22 `bolt/perf-dataslot-size-cache` (open): 33→0 MMIO reads per dataslot_size() call
- #23 `bolt/perf-core-poll-io-dummy-reads` (open): 3→1 spi_w transactions per mainloop iteration
- #24 `bolt/perf-input-reg-poll-interval` (open): 1→0.25 MMIO reads per mainloop iteration (75% reduction)

Remaining candidate evaluation: `ide_check()` caching rejected (unsafe, async IDE status). OSD SPI hardware-limited (1 byte/strobe, menu-only). `mouse_math.h`/`pocket_shortcuts.h` marginal. `put_lba()` CHS divisions only affect non-LBA mode (rare).
**Decision:** No safe measurable targets remain on `main` beyond PR #24.
