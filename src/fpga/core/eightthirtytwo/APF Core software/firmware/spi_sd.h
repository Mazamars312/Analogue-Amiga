#ifndef SPI_SD_H
#define SPI_SD_H

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

int sd_init();
int sd_read_sector(unsigned long lba,unsigned char *buf);
int sd_write_sector(unsigned long lba,unsigned char *buf); // FIXME - stub
int sd_get_size();

extern int spi_checksum;
extern int sd_is_sdhc;
extern int sd_size;

#ifdef __cplusplus
}
#endif

#endif
