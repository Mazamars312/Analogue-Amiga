# Bolt's Journal - Critical Learnings Only

## 2026-11-05 - Redundant SPI transfers in MPU BIOS firmware mainloop
**Learning:** In softcore firmware architectures (such as the 50MHz VexRiscv in this core), the main polling loop runs continuously without an RTOS or frame delay. Synchronous SPI bus commands (like `HPS_spi_uio_cmd8`) incur bus setup, transfer latency, and `riscusleep` busy-waits. Unconditionally transmitting configuration register values across the SPI bridge on every loop iteration saturates the bus with redundant commands.
**Action:** Always cache the previously transmitted register state and only trigger bus transactions when the value has changed.
