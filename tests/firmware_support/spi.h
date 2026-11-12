// Host-side stub of src/MPUBIOS/drivers/hps/spi.h for unit tests.
#ifndef MOCK_SPI_H
#define MOCK_SPI_H

#include <stdint.h>

// Only the commands the firmware under test uses; values match the real header.
#define UIO_MM2_JOY 0xF8

uint8_t HPS_spi_uio_cmd8(uint8_t cmd, uint8_t parm);

#endif
