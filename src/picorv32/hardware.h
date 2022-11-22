#ifndef HARDWARE_H
#define HARDWARE_H

#include <inttypes.h>
#include <stdio.h>

#define SPI_RST_USR         0x1
#define SPI_RST_CPU         0x2
#define SPI_CPU_HLT         0x4

#define INTERRUPTBASE 0xffffffb0
#define SPIHARDWAREBASE 0xffffffd0

#endif // SPI_H
