/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef SPI_H
#define SPI_H

#include <inttypes.h>
#include "hardware.h"
/* FPGA functions */


#define HPS_spi_write_fpga(x) *(volatile unsigned int *)(MISTERGPOHARDWAREBASE)
#define HPS_spi_read_fpga(x) *(volatile unsigned int *)(MISTERGPIHARDWAREBASE)

void HPS_EnableIO();
void HPS_DisableIO();
void HPS_EnableFpga();
void HPS_DisableFpga();

uint16_t HPS_fpga_spi(uint16_t word);

uint16_t inline spi_w(uint16_t word)
{
	return HPS_fpga_spi(word);
};

uint8_t  inline spi_b(uint8_t parm)
{
	return (uint8_t)HPS_fpga_spi(parm);
}

void HPS_spi_read(uint8_t *addr, uint32_t len, int wide);
void HPS_spi_write(const uint8_t *addr, uint32_t len, int wide);
void HPS_spi_block_read(uint8_t *addr, int wide, int sz = 512);
void HPS_spi_block_write(const uint8_t *addr, int wide, int sz = 512);

uint16_t HPS_spi_uio_cmd_cont(uint16_t cmd);
uint16_t HPS_spi_uio_cmd(uint16_t cmd);
uint8_t HPS_spi_uio_cmd8_cont(uint8_t cmd, uint8_t parm);
uint8_t HPS_spi_uio_cmd8(uint8_t cmd, uint8_t parm);
uint16_t HPS_spi_uio_cmd16(uint8_t cmd, uint16_t parm);
void HPS_spi_uio_cmd32(uint8_t cmd, uint32_t parm, int wide);
void HPS_spi_uio_cmd32_cont(uint8_t cmd, uint32_t parm);

#endif // SPI_H
