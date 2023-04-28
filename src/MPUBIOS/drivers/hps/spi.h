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

// #include <inttypes.h>
#include "hardware.h"
#include "interrupts.h"
#include "timer.h"
#include "riscprintf.h"
/* FPGA functions */
#define OSD_HDMI 1
#define OSD_VGA  2
#define OSD_ALL  (OSD_VGA|OSD_HDMI)

#define HPS_spi_write_fpga(x) *(volatile unsigned int *)(MISTERGPOHARDWAREBASE)
#define HPS_spi_read_fpga(x) *(volatile unsigned int *)(MISTERGPIHARDWAREBASE)

// HPS commands
#define UIO_SET_SDSTAT  0x33  // set sd card status
#define UIO_CD_GET      0x34
#define UIO_CD_SET      0x35
#define UIO_CD_DATA     0x36
#define UIO_SET_SDINFO  0x32  // send info about mounted image

#define UIO_DMA_WRITE	0x61
#define UIO_DMA_READ	0x62
#define UIO_DMA_SDIO 	0x63

// ---- Minimig v2 constants -------
#define UIO_MM2_RST     0xF0
#define UIO_MM2_AUD     0xF1
#define UIO_MM2_CHIP    0xF2
#define UIO_MM2_CPU     0xF3
#define UIO_MM2_MEM     0xF4
#define UIO_MM2_VID     0xF5
#define UIO_MM2_FLP     0xF6
#define UIO_MM2_HDD     0xF7
#define UIO_MM2_JOY     0xF8
#define UIO_MM2_MIR     0xF9

// codes as used by 8bit for file loading from OSD
#define FIO_FILE_TX     0x53
#define FIO_FILE_TX_DAT 0x54
#define FIO_FILE_INDEX  0x55
#define FIO_FILE_INFO   0x56

#define spi8(x) spi_b(x)

uint32_t spi32_w(uint32_t parm);

void HPS_EnableIO();
void HPS_DisableIO();
void HPS_EnableFpga();
void HPS_DisableFpga();
void HPS_EnableOsd();
void HPS_DisableOsd();

/* OSD related SPI functions */
void HPS_EnableOsd_on(int target);
void HPS_spi_osd_cmd_cont(uint8_t cmd);
void HPS_spi_osd_cmd(uint8_t cmd);
void HPS_spi_osd_cmd8_cont(uint8_t cmd, uint8_t parm);
void HPS_spi_osd_cmd8(uint8_t cmd, uint8_t parm);
void OSD_HPS_spi_write(const uint8_t *addr, uint32_t len, int wide);

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

void HPS_EnableOsd_on(int target);
void HPS_spi_osd_cmd_cont(uint8_t cmd);
void HPS_spi_osd_cmd(uint8_t cmd);
void HPS_spi_osd_cmd8_cont(uint8_t cmd, uint8_t parm);
void HPS_spi_osd_cmd8(uint8_t cmd, uint8_t parm);

uint16_t HPS_spi_uio_cmd_cont(uint16_t cmd);
uint16_t HPS_spi_uio_cmd(uint16_t cmd);
uint8_t HPS_spi_uio_cmd8_cont(uint8_t cmd, uint8_t parm);
uint8_t HPS_spi_uio_cmd8(uint8_t cmd, uint8_t parm);
uint16_t HPS_spi_uio_cmd16(uint8_t cmd, uint16_t parm);
void HPS_spi_uio_cmd32(uint8_t cmd, uint32_t parm, int wide);
void HPS_spi_uio_cmd32_cont(uint8_t cmd, uint32_t parm);
void HPS_io_set_index(unsigned char index);
void HPS_io_set_download(unsigned char enable, int addr = 0);
void HPS_io_file_tx_data(const uint8_t *addr, uint32_t len, int cmd);
uint16_t HPS_fpga_spi_fast(uint16_t word);
void HPS_spi_write_fast(const uint8_t *addr, uint32_t len, int wide);
void HPS_fpga_spi_fast_block_write(const uint16_t *buf, uint32_t length);
void HPS_fpga_spi_fast_block_write_be(const uint16_t *buf, uint32_t length);
void HPS_fpga_spi_fast_block_read(uint16_t *buf, uint32_t length, bool printtxt);
void HPS_fpga_spi_fast_block_read_be(uint16_t *buf, uint32_t length);


#endif // SPI_H
