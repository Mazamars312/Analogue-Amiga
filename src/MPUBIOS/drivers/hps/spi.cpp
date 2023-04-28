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


#define SSPI_STROBE  (1<<17)
#define SSPI_ACK     SSPI_STROBE
#define SSPI_FPGA_EN (1<<18)
#define SSPI_OSD_EN  (1<<19) // This is not used
#define SSPI_IO_EN   (1<<20)
static int fio_size = 1;

#define SWAPW(a) ((((a)<<8)&0xff00)|(((a)>>8)&0x00ff))


// These will emulated the HPS
void HPS_fpga_gpo_write(uint32_t value){
 	HPS_spi_write_fpga(0) = value;
};

uint32_t HPS_fpga_gpo_read(){

 	return (HPS_spi_write_fpga(0));
};

uint32_t HPS_fpga_gpi_read(){
 	return (HPS_spi_read_fpga(0));
};

void HPS_fpga_spi_en(uint32_t mask, uint32_t en)
{
	uint32_t gpo = HPS_fpga_gpo_read() | 0x80000000;

  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	HPS_fpga_gpo_write(en ? gpo | mask : gpo & ~mask);
}

void HPS_EnableFpga()
{
  DisableInterrupts();
	HPS_fpga_spi_en(SSPI_FPGA_EN, 1);

}

void HPS_DisableFpga()
{
	HPS_fpga_spi_en(SSPI_FPGA_EN, 0);
  EnableInterrupts();
}

void HPS_EnableIO()
{
  DisableInterrupts();
	HPS_fpga_spi_en(SSPI_IO_EN, 1);
}

void HPS_DisableIO()
{
	HPS_fpga_spi_en(SSPI_IO_EN, 0);
  EnableInterrupts();
}

uint16_t HPS_fpga_spi(uint16_t word){
  uint32_t gpo_temp = HPS_fpga_gpo_read();
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	uint32_t gpo = (gpo_temp & ~(0xFFFF | SSPI_STROBE)) | word;
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	HPS_fpga_gpo_write(gpo);
	HPS_fpga_gpo_write(gpo | SSPI_STROBE);
	int gpi;
	do
	{
		gpi = HPS_fpga_gpi_read();
    asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
    asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	} while (!(gpi & SSPI_ACK));

	HPS_fpga_gpo_write(gpo);

	do
	{
		gpi = HPS_fpga_gpi_read();
    asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
    asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	} while (gpi & SSPI_ACK);

	return (uint16_t)gpi;
}

void HPS_spi_read(uint8_t *addr, uint32_t len, int wide)
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

void HPS_spi_write(const uint8_t *addr, uint32_t len, int wide)
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

void HPS_spi_block_read(uint8_t *addr, int wide, int sz)
{
	// if (wide) fpga_spi_fast_block_read((uint16_t*)addr, sz/2);
	// else fpga_spi_fast_block_read_8(addr, sz);
}

void HPS_spi_block_write(const uint8_t *addr, int wide, int sz)
{
	// if (wide) fpga_spi_fast_block_write((const uint16_t*)addr, sz/2);
	// else fpga_spi_fast_block_write_8(addr, sz);
}

uint32_t spi32_w(uint32_t parm)
{
	uint32_t res;
	res = spi_w(parm);
	res |= (spi_w(parm>>16))<<16;
	return res;
}
/* User_io related SPI functions */
uint16_t HPS_spi_uio_cmd_cont(uint16_t cmd)
{
	HPS_EnableIO();
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	return spi_w(cmd);
}

uint16_t HPS_spi_uio_cmd(uint16_t cmd)
{
	uint16_t res = HPS_spi_uio_cmd_cont(cmd);
	HPS_DisableIO();
	return res;
}

uint8_t HPS_spi_uio_cmd8_cont(uint8_t cmd, uint8_t parm)
{
	HPS_EnableIO();
  riscusleep(1);
	spi_b(cmd);
  riscusleep(1);
	return spi_b(parm);
}

uint8_t HPS_spi_uio_cmd8(uint8_t cmd, uint8_t parm)
{

	uint8_t res = HPS_spi_uio_cmd8_cont(cmd, parm);
  // riscprintf("HPS %0.4x\r\n", res);
  riscusleep(1);
	HPS_DisableIO();
	return res;
}

uint16_t HPS_spi_uio_cmd16(uint8_t cmd, uint16_t parm)
{
	HPS_spi_uio_cmd_cont(cmd);
  riscusleep(2);
	uint16_t res = spi_w(parm);
  riscusleep(2);
	HPS_DisableIO();
	return res;
}

void HPS_spi_uio_cmd32(uint8_t cmd, uint32_t parm, int wide)
{
	HPS_EnableIO();
  riscusleep(1);
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
	HPS_DisableIO();
}

void HPS_io_set_index(unsigned char index)
{
	HPS_EnableFpga();
  riscusleep(1);
	spi_b(FIO_FILE_INDEX);
	spi_b(index);
	HPS_DisableFpga();
}

void HPS_io_set_download(unsigned char enable, int addr)
{
	HPS_EnableFpga();
  riscusleep(1);
	spi_b(FIO_FILE_TX);
	spi_b(enable ? 0xff : 0);
	if (enable && addr)
	{
		spi_w(addr);
		spi_w(addr >> 16);
	}
	HPS_DisableFpga();
}

void HPS_io_file_tx_data(const uint8_t *addr, uint32_t len, int cmd)
{
	HPS_EnableIO();
  riscusleep(1);
	spi_b(cmd);
	HPS_spi_write(addr, len, fio_size);
	HPS_DisableIO();
}

void HPS_fpga_spi_write_fast(uint16_t word){
  uint32_t gpo_temp = HPS_fpga_gpo_read();
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	uint32_t gpo = (gpo_temp & ~(0xFFFF | SSPI_STROBE)) | word;
  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	HPS_fpga_gpo_write(gpo);
	HPS_fpga_gpo_write(gpo | SSPI_STROBE);
}

void HPS_spi_write_fast(const uint8_t *addr, uint32_t len, int wide)
{

  // uint32_t total = 0;
	if (wide)
	{
		uint32_t len16 = len >> 1;
		uint16_t *a16 = (uint16_t*)addr;
		while (len16--) {
      HPS_fpga_spi_write_fast(*a16++);
    }
		if(len & 1) HPS_fpga_spi_write_fast(*((uint8_t*)a16));
  	}
  	else
  	{
		while (len--) HPS_fpga_spi_write_fast(*addr++);
	   }
}

uint16_t HPS_fpga_spi_fast(uint16_t word)
{
	uint32_t gpo = (HPS_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE)) | word;
	HPS_fpga_gpo_write(gpo);
	HPS_fpga_gpo_write(gpo | SSPI_STROBE);
	HPS_fpga_gpo_write(gpo);
	return (uint16_t)HPS_fpga_gpi_read();
}

void HPS_fpga_spi_fast_block_write(const uint16_t *buf, uint32_t length)
{
	uint32_t gpoH = (HPS_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t gpo = gpoH;

	while (length--)
	{
		gpo = gpoH | *buf++;
		HPS_fpga_gpo_write(gpo);
		HPS_fpga_gpo_write(gpo | SSPI_STROBE);
	}
	HPS_fpga_gpo_write(gpo);
}

void HPS_fpga_spi_fast_block_read(uint16_t *buf, uint32_t length, bool printtxt)
{
	uint32_t gpo = (HPS_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	HPS_fpga_gpo_write(gpo);
	while (length--)
	{
		HPS_fpga_gpo_write(gpo | SSPI_STROBE);
		HPS_fpga_gpo_write(gpo);
		*buf++ = (uint16_t)HPS_fpga_gpi_read();
		// if(printtxt) mainprintf("receiving data %0.4x\r\n", (uint16_t)HPS_fpga_gpi_read());
	}
}

void HPS_fpga_spi_fast_block_write_be(const uint16_t *buf, uint32_t length)
{
	uint32_t gpoH = (HPS_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t gpo = gpoH;

	// should be optimized for speed by compiler automatically
	while (length--)
	{
		uint16_t tmp = *buf++;
		tmp = (tmp << 8) | (tmp >> 8);
		gpo = gpoH | tmp;
		HPS_fpga_gpo_write(gpo);
		HPS_fpga_gpo_write(gpo | SSPI_STROBE);
	}
	HPS_fpga_gpo_write(gpo);
}

void HPS_fpga_spi_fast_block_read_be(uint16_t *buf, uint32_t length)
{
	uint32_t gpo = (HPS_fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));

	// should be optimized for speed by compiler automatically
	while (length--)
	{
		HPS_fpga_gpo_write(gpo | SSPI_STROBE);
		HPS_fpga_gpo_write(gpo);
		uint16_t tmp = (uint16_t)HPS_fpga_gpi_read();
		*buf++ = (tmp << 8) | (tmp >> 8);
	}
}


static int osd_target = OSD_ALL;

void HPS_EnableOsd_on(int target)
{
	if (!(target & OSD_ALL)) target = OSD_ALL;
	osd_target = target;
}


void HPS_EnableOsd()
{
	osd_target = OSD_ALL;

	uint32_t mask = SSPI_OSD_EN | SSPI_IO_EN | SSPI_FPGA_EN;

	HPS_fpga_spi_en(mask, 1);
}

void HPS_DisableOsd()
{
	HPS_fpga_spi_en(SSPI_OSD_EN | SSPI_IO_EN | SSPI_FPGA_EN, 0);
}

void HPS_spi_osd_cmd_cont(uint8_t cmd)
{
	HPS_EnableOsd();
	spi_b(cmd);
}

void HPS_spi_osd_cmd(uint8_t cmd)
{
	HPS_spi_osd_cmd_cont(cmd);
	HPS_DisableOsd();
}

void HPS_spi_osd_cmd8_cont(uint8_t cmd, uint8_t parm)
{
	HPS_EnableOsd();
	spi_b(cmd);
	spi_b(parm);
}

void HPS_spi_osd_cmd8(uint8_t cmd, uint8_t parm)
{
	HPS_spi_osd_cmd8_cont(cmd, parm);
	HPS_DisableOsd();
}
