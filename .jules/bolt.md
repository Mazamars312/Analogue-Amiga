# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Four bolt PRs opened, no remaining targets
**State:** All four bolt branches now have PRs open against `ollisulopuisto/Analogue-Amiga`:
- #17 `bolt/perf-osd-keyboard-mmio-reads`: 12→2 / 23→2 MMIO reads in OSD keyboard poll
- #18 `bolt/perf-keyboard-register-snapshot`: 44→3 MMIO reads per keyboard poll
- #19 `bolt/perf-apf-timeout-spin`: 101→1 / 10001→1 APF handshake polls
- #20 `bolt/perf-fdd-sector-pointer`: 5632→0 bytes copied per floppy track read

**Remaining candidate evaluation:** `core_poll_io()` has 2 discarded `spi_w(0)` reads per idle mainloop poll. FPGA analysis suggests they may be vestigial (each `spi_w` is an independent strobe transaction; FPGA resets `has_cmd`/`bcnt` on each Enable/Disable cycle; `IO_WAIT` unassigned in userio.v). However, `core.cpp` cannot be host-compiled, host-tested, or hardware-verified, and a mistake breaks floppy disk I/O. Rejected as protocol-risky/untestable. `ide_check()` caching rejected (unsafe, async IDE status). OSD SPI hardware-limited (1 byte/strobe, menu-only). `mouse_math.h`/`pocket_shortcuts.h` marginal.
**Decision:** No safe measurable targets remain on `main`.
