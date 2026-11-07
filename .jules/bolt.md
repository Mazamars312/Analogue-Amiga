# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.

## 2026-11-07 - Four bolt branches ready, PRs not yet opened
**State:** Branches `bolt/perf-osd-keyboard-mmio-reads`, `bolt/perf-keyboard-register-snapshot`, `bolt/perf-apf-timeout-spin`, `bolt/perf-fdd-sector-pointer` exist locally and on `origin` with committed optimizations and passing host tests, but no PRs were opened against `ollisulopuisto/Analogue-Amiga`. Opening them one per iteration.
