# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Bolt PR #21 opened: dataslot_search_id cache
**New PR:** #21 `bolt/perf-dataslot-search-cache` — cache `dataslot_search_id()` index to eliminate redundant MMIO reads. Baseline: up to 128 volatile MMIO reads per linear search; optimized: 0 reads on cache hit. Totally different approach from previous iterations (algorithmic caching in apf.cpp, not MMIO read elimination in keyboard/APF/fdd code).

All five bolt PRs are now open against `ollisulopuisto/Analogue-Amiga`:
- #17 `bolt/perf-osd-keyboard-mmio-reads`: 12→2 / 23→2 MMIO reads in OSD keyboard poll
- #18 `bolt/perf-keyboard-register-snapshot`: 44→3 MMIO reads per keyboard poll
- #19 `bolt/perf-apf-timeout-spin`: 101→1 / 10001→1 APF handshake polls
- #20 `bolt/perf-fdd-sector-pointer`: 5632→0 bytes copied per floppy track read
- #21 `bolt/perf-dataslot-search-cache`: up to 128→0 MMIO reads per dataslot lookup

Remaining candidate evaluation: `core_poll_io()` has 2 discarded `spi_w(0)` reads per idle mainloop poll — rejected as protocol-risky/untestable (no host test, no riscv toolchain, breaks floppy I/O if wrong). `ide_check()` caching rejected (unsafe). OSD SPI hardware-limited. `mouse_math.h`/`pocket_shortcuts.h` marginal.
**Decision:** No safe measurable targets remain on `main` beyond PR #21.
