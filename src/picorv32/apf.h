#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "uart.h"

#ifndef APF_H
#define APF_H

#define DATASLOT_ID_BASE              0xFFFF0000
#define DATASLOT_ID_BASE_END          0x1000

#define TARGET_DATASLOT_ID            0xFFFFFF80
#define TARGET_DATASLOT_BRIDGE_ADD    0xFFFFFF84
#define TARGET_DATASLOT_LENGTH        0xFFFFFF88
#define TARGET_DATASLOT_OFFSET        0xFFFFFF8C
#define TARGET_DATASLOT_CONTROL       0xFFFFFF90

#define DATASLOT_UPDATE_FLAG_REG      0xFFFFFFB0

#define TARGET_DATASLOT_READ_REG      0x1
#define TARGET_DATASLOT_WRITE_REG     0x2


#define APF_ADDRESS_OFFSET      0x80000000
#define APF_ACK 0x10
#define APF_DONE 0x8
#define APF_ERROR 0x7

#define DATASLOT_RAM_ACCESS(x) *(volatile unsigned int *)(DATASLOT_ID_BASE+(x))

#define DATASLOT_UPDATE_REG(x) *(volatile unsigned int *)(DATASLOT_UPDATE_FLAG_REG+(x))

#define WRITE_TARGET_DATASLOT_ID(x) *(volatile unsigned int *)(TARGET_DATASLOT_ID+(x))
#define WRITE_TARGET_DATASLOT_BRIDGE_ADD(x) *(volatile unsigned int *)(TARGET_DATASLOT_BRIDGE_ADD+(x))
#define WRITE_TARGET_DATASLOT_LENGTH(x) *(volatile unsigned int *)(TARGET_DATASLOT_LENGTH+(x))
#define WRITE_TARGET_DATASLOT_OFFSET(x) *(volatile unsigned int *)(TARGET_DATASLOT_OFFSET+(x))
#define WRITE_TARGET_DATASLOT_CONTROL(x) *(volatile unsigned int *)(TARGET_DATASLOT_CONTROL+(x))

#define READ_TARGET_DATASLOT_ID(x) *(volatile unsigned int *)(TARGET_DATASLOT_ID+(x))
#define READ_TARGET_DATASLOT_BRIDGE_ADD(x) *(volatile unsigned int *)(TARGET_DATASLOT_BRIDGE_ADD+(x))
#define READ_TARGET_DATASLOT_LENGTH(x) *(volatile unsigned int *)(TARGET_DATASLOT_LENGTH+(x))
#define READ_TARGET_DATASLOT_OFFSET(x) *(volatile unsigned int *)(TARGET_DATASLOT_OFFSET+(x))
#define READ_TARGET_DATASLOT_CONTROL(x) *(volatile unsigned int *)(TARGET_DATASLOT_CONTROL+(x))


uint32_t dataslot_search_id(uint16_t value);
uint32_t dataslot_size(uint16_t value);
void dataslot_search_active(uint16_t value);
// bool dataslot_updated();
uint32_t dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length);
uint32_t dataslot_write(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length);


#endif
