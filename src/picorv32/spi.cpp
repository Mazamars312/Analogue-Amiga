#include "spi.h"
#include "printf.h"
#include "apf.h"

#define SSPI_STROBE  (1<<17)
#define SSPI_ACK     SSPI_STROBE
#define SSPI_FPGA_EN (1<<18)
#define SSPI_OSD_EN  (1<<19)
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
	printf("gpoout %.4x\r\n", gpo);
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
