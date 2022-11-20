
#include "spi.h"


void spi_write(const char *data,int len)
{
	while(len--)
		SPI(*data++);
}


void spi_read(char *data,int len)
{
	while(len--)
		*data++=SPI(0xff);
}

