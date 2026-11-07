# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Bolt PR #22 opened: dataslot_size() size cache
**New PR:** #22 `bolt/perf-dataslot-size-cache` — cache `dataslot_size()` size value to eliminate remaining MMIO read. Extends PR #21's index cache to also cache the size word: baseline 33 reads → PR #21 hit 1 read → this iteration hit 0 reads.

All bolt PRs against `ollisulopuisto/Analogue-Amiga`:
- #13 `bolt/perf-osd-keyboard-mmio-reads` (merged): 12→2 / 23→2 MMIO reads in OSD keyboard poll
- #14 `bolt/perf-keyboard-register-snapshot` (merged): 44→3 MMIO reads per keyboard poll
- #15 `bolt/perf-apf-timeout-spin` (merged): 101→1 / 10001→1 APF handshake polls (~90×)
- #16 `bolt/perf-fdd-sector-pointer` (merged): 5632→0 bytes copied per floppy track read
- #21 `bolt/perf-dataslot-search-cache` (merged): up to 128→0 MMIO reads per dataslot lookup
- #22 `bolt/perf-dataslot-size-cache` (open): 33→0 MMIO reads per dataslot_size() call

Remaining candidate evaluation: `core_poll_io()` has 2 discarded `spi_w(0)` reads per idle mainloop poll — rejected as protocol-risky/untestable. `ide_check()` caching rejected (unsafe). OSD SPI hardware-limited. `mouse_math.h`/`pocket_shortcuts.h` marginal. `put_lba()` CHS divisions only affect non-LBA mode (rare).
**Decision:** No safe measurable targets remain on `main` beyond PR #22.
