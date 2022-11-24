#ifndef SPI_H
#define SPI_H

#include <inttypes.h>
#include "hardware.h"
// #include "apf.h"
// #include "spi.h"
/* FPGA functions */


#define mister_spi_write_fpga(x) *(volatile unsigned int *)(MISTERGPOHARDWAREBASE)
#define mister_spi_read_fpga(x) *(volatile unsigned int *)(MISTERGPIHARDWAREBASE)

void mister_EnableIO();
void mister_DisableIO();
void mister_EnableFpga();
void mister_DisableFpga();

uint16_t mister_fpga_spi(uint16_t word);

uint16_t inline spi_w(uint16_t word)
{
	return mister_fpga_spi(word);
};

#endif // SPI_H
