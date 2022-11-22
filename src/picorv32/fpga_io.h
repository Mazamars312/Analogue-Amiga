#include <stdint.h>
#include "hardware.h"
#include "apf.h"

#ifndef FPGAIO_H
#define FPGAIO_H

int fpga_io_init();

void fpga_spi_en(uint32_t mask, uint32_t en);
uint16_t fpga_spi(uint16_t word);
uint16_t fpga_spi_fast(uint16_t word);

void fpga_spi_fast_block_write(const uint16_t *buf, uint32_t length);
void fpga_spi_fast_block_read(uint16_t *buf, uint32_t length);
void fpga_spi_fast_block_write_8(const uint8_t *buf, uint32_t length);
void fpga_spi_fast_block_read_8(uint8_t *buf, uint32_t length);
void fpga_spi_fast_block_write_be(const uint16_t *buf, uint32_t length);
void fpga_spi_fast_block_read_be(uint16_t *buf, uint32_t length);

int fpga_get_io_type();

void fpga_core_reset(int reset);
void fpga_core_write(uint32_t offset, uint32_t value);
uint32_t fpga_core_read(uint32_t offset);

void reboot(int cold);

#endif
