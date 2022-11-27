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

#include "spi.h"
#include "printf.h"
#include "apf.h"

#define SSPI_STROBE  (1<<17)
#define SSPI_ACK     SSPI_STROBE
#define SSPI_FPGA_EN (1<<18)
#define SSPI_OSD_EN  (1<<19) // This is not used
#define SSPI_IO_EN   (1<<20)

#define SWAPW(a) ((((a)<<8)&0xff00)|(((a)>>8)&0x00ff))


// These will emulated the Mister
void mister_fpga_gpo_write(uint32_t value){
 	mister_spi_write_fpga(0) = value;
};

uint32_t mister_fpga_gpo_read(){
 	return (mister_spi_write_fpga(0));
};

uint32_t mister_fpga_gpi_read(){
 	return (mister_spi_read_fpga(0));
};

void mister_fpga_spi_en(uint32_t mask, uint32_t en)
{
	uint32_t gpo = mister_fpga_gpo_read() | 0x80000000;
	mister_fpga_gpo_write(en ? gpo | mask : gpo & ~mask);
}

void mister_EnableFpga()
{
	mister_fpga_spi_en(SSPI_FPGA_EN, 1);
}

void mister_DisableFpga()
{
	mister_fpga_spi_en(SSPI_FPGA_EN, 0);
}

void mister_EnableIO()
{
	mister_fpga_spi_en(SSPI_IO_EN, 1);
}

void mister_DisableIO()
{
	mister_fpga_spi_en(SSPI_IO_EN, 0);
}

uint16_t mister_fpga_spi(uint16_t word)
{
	uint32_t gpo = (mister_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE)) | word;

	mister_fpga_gpo_write(gpo);
	mister_fpga_gpo_write(gpo | SSPI_STROBE);

	int gpi;
	do
	{
		gpi = mister_fpga_gpi_read();
	} while (!(gpi & SSPI_ACK));

	mister_fpga_gpo_write(gpo);

	do
	{
		gpi = mister_fpga_gpi_read();
	} while (gpi & SSPI_ACK);

	return (uint16_t)gpi;
}

void mister_spi_read(uint8_t *addr, uint32_t len, int wide)
{
	if (wide)
	{
		uint32_t len16 = len >> 1;
		uint16_t *a16 = (uint16_t*)addr;
		while (len16--) *a16++ = spi_w(0);
		if (len & 1) *((uint8_t*)a16) = spi_w(0);
	}
	else
	{
		while (len--) *addr++ = spi_b(0);
	}
}

void mister_spi_write(const uint8_t *addr, uint32_t len, int wide)
{
	if (wide)
	{
		uint32_t len16 = len >> 1;
		uint16_t *a16 = (uint16_t*)addr;
		while (len16--) spi_w(*a16++);
		if(len & 1) spi_w(*((uint8_t*)a16));
	}
	else
	{
		while (len--) spi_b(*addr++);
	}
}

void mister_spi_block_read(uint8_t *addr, int wide, int sz)
{
	// if (wide) fpga_spi_fast_block_read((uint16_t*)addr, sz/2);
	// else fpga_spi_fast_block_read_8(addr, sz);
}

void mister_spi_block_write(const uint8_t *addr, int wide, int sz)
{
	// if (wide) fpga_spi_fast_block_write((const uint16_t*)addr, sz/2);
	// else fpga_spi_fast_block_write_8(addr, sz);
}


/* User_io related SPI functions */
uint16_t mister_spi_uio_cmd_cont(uint16_t cmd)
{
	mister_EnableIO();
	return spi_w(cmd);
}

uint16_t mister_spi_uio_cmd(uint16_t cmd)
{
	uint16_t res = mister_spi_uio_cmd_cont(cmd);
	mister_DisableIO();
	return res;
}

uint8_t mister_spi_uio_cmd8_cont(uint8_t cmd, uint8_t parm)
{
	mister_EnableIO();
	spi_b(cmd);
	return spi_b(parm);
}

uint8_t mister_spi_uio_cmd8(uint8_t cmd, uint8_t parm)
{
	uint8_t res = mister_spi_uio_cmd8_cont(cmd, parm);
	mister_DisableIO();
	return res;
}

uint16_t mister_spi_uio_cmd16(uint8_t cmd, uint16_t parm)
{
	mister_spi_uio_cmd_cont(cmd);
	uint16_t res = spi_w(parm);
	mister_DisableIO();
	return res;
}

void mister_spi_uio_cmd32(uint8_t cmd, uint32_t parm, int wide)
{
	mister_EnableIO();
	spi_b(cmd);
	if (wide)
	{
		spi_w((uint16_t)parm);
		spi_w((uint16_t)(parm >> 16));
	}
	else
	{
		spi_b(parm);
		spi_b(parm >> 8);
		spi_b(parm >> 16);
		spi_b(parm >> 24);
	}
	mister_DisableIO();
}
