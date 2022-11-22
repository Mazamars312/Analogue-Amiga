#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "uart.h"
#include "fpga_io.h"

#define MAP_ADDR(x) (volatile uint32_t*)(&map_base[(((uint32_t)(x)) & 0xFFFFFF)>>2])
#define IS_REG(x) (((((uint32_t)(x))-1)>=(FPGA_REG_BASE - 1)) && ((((uint32_t)(x))-1)<(FPGA_REG_BASE + FPGA_REG_SIZE - 1)))

#define fatal(x) munmap((void*)map_base, FPGA_REG_SIZE); close(fd); exit(x)

static uint32_t *map_base;

#define writel(val, reg) *MAP_ADDR(reg) = val
#define readl(reg) *MAP_ADDR(reg)

#define clrsetbits_le32(addr, clear, set) writel((readl(addr) & ~(clear)) | (set), addr)
#define setbits_le32(addr, set)           writel( readl(addr) | (set), addr)
#define clrbits_le32(addr, clear)         writel( readl(addr) & ~(clear), addr)

/* Timeout count */
#define FPGA_TIMEOUT_CNT		0x1000000

static uint32_t gpo_copy = 0;
void inline fpga_gpo_write(uint32_t value)
{
	gpo_copy = value;
	writel(value, (void*)(SPIHARDWAREBASE));
}

#define fpga_gpo_writeN(value) writel((value), (void*)(SPIHARDWAREBASE))
#define fpga_gpo_read() gpo_copy //readl((void*)(SOCFPGA_MGR_ADDRESS + 0x10))
#define fpga_gpi_read() (int)readl((void*)(SPIHARDWAREBASE))

// void reboot(int cold)
// {
// 	fpga_core_reset(1);
//
// 	// usleep(500000);
//
// 	void* buf = shmem_map(0x1FFFF000, 0x1000);
// 	if (buf)
// 	{
// 		volatile uint32_t* flg = (volatile uint32_t*)buf;
// 		flg += 0xF08/4;
// 		*flg = cold ? 0 : 0xBEEFB001;
// 		shmem_unmap(buf, 0x1000);
// 	}
//
// 	writel(1, &reset_regs->ctrl);
// 	while (1) sleep(1);
// }

#define SSPI_STROBE  (1<<17)
#define SSPI_ACK     SSPI_STROBE

void fpga_spi_en(uint32_t mask, uint32_t en)
{
	uint32_t gpo = fpga_gpo_read() | 0x80000000;
	fpga_gpo_write(en ? gpo | mask : gpo & ~mask);
}

// void fpga_wait_to_reset()
// {
// 	printf("FPGA is not ready. JTAG uploading?\n");
// 	printf("Waiting for FPGA to be ready...\n");
//
// 	fpga_core_reset(1);
//
// 	while (!is_fpga_ready(0))
// 	{
// 		sleep(1);
// 	}
// 	reboot(0);
// }

uint16_t fpga_spi(uint16_t word)
{
	uint32_t gpo = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE)) | word;

	fpga_gpo_write(gpo);
	fpga_gpo_write(gpo | SSPI_STROBE);

	int gpi;
	do
	{
		gpi = fpga_gpi_read();
		if (gpi < 0)
		{
			printf("GPI[31]==1. FPGA is uninitialized?\n");
			// fpga_wait_to_reset();
			return 0;
		}
	} while (!(gpi & SSPI_ACK));

	fpga_gpo_write(gpo);

	do
	{
		gpi = fpga_gpi_read();
		if (gpi < 0)
		{
			printf("GPI[31]==1. FPGA is uninitialized?\n");
			// fpga_wait_to_reset();
			return 0;
		}
	} while (gpi & SSPI_ACK);

	return (uint16_t)gpi;
}

uint16_t fpga_spi_fast(uint16_t word)
{
	uint32_t gpo = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE)) | word;
	fpga_gpo_write(gpo);
	fpga_gpo_write(gpo | SSPI_STROBE);
	fpga_gpo_write(gpo);
	return (uint16_t)fpga_gpi_read();
}

void fpga_spi_fast_block_write(const uint16_t *buf, uint32_t length)
{
	uint32_t gpoH = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t gpo = gpoH;

	// should be optimized for speed by compiler automatically
	while (length--)
	{
		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);
	}
	fpga_gpo_write(gpo);
}

void fpga_spi_fast_block_read(uint16_t *buf, uint32_t length)
{
	uint32_t gpo = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t rem = length % 16;
	length /= 16;

	// not optimized by compiler automatically
	// so do manual optimization for speed.
	while (length--)
	{
		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();
	}

	while (rem--)
	{
		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint16_t)fpga_gpi_read();
	}
}

void fpga_spi_fast_block_write_8(const uint8_t *buf, uint32_t length)
{
	uint32_t gpoH = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t gpo = gpoH;
	uint32_t rem = length % 16;
	length /= 16;

	// not optimized by compiler automatically
	// so do manual optimization for speed.
	while (length--)
	{
		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);

		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);
	}

	while (rem--)
	{
		gpo = gpoH | *buf++;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);
	}

	fpga_gpo_write(gpo);
}

void fpga_spi_fast_block_read_8(uint8_t *buf, uint32_t length)
{
	uint32_t gpo = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t rem = length % 16;
	length /= 16;

	// not optimized by compiler automatically
	// so do manual optimization for speed.
	while (length--)
	{
		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();

		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();
	}

	while (rem--)
	{
		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		*buf++ = (uint8_t)fpga_gpi_read();
	}
}

void fpga_spi_fast_block_write_be(const uint16_t *buf, uint32_t length)
{
	uint32_t gpoH = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));
	uint32_t gpo = gpoH;

	// should be optimized for speed by compiler automatically
	while (length--)
	{
		uint16_t tmp = *buf++;
		tmp = (tmp << 8) | (tmp >> 8);
		gpo = gpoH | tmp;
		fpga_gpo_writeN(gpo);
		fpga_gpo_writeN(gpo | SSPI_STROBE);
	}
	fpga_gpo_write(gpo);
}

void fpga_spi_fast_block_read_be(uint16_t *buf, uint32_t length)
{
	uint32_t gpo = (fpga_gpo_read() & ~(0xFFFF | SSPI_STROBE));

	// should be optimized for speed by compiler automatically
	while (length--)
	{
		fpga_gpo_writeN(gpo | SSPI_STROBE);
		fpga_gpo_writeN(gpo);
		uint16_t tmp = (uint16_t)fpga_gpi_read();
		*buf++ = (tmp << 8) | (tmp >> 8);
	}
}
