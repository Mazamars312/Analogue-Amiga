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

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
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

// APF commands and address locations
#define APF_ADDRESS_OFFSET      0x00000000 // We want to make sure we advise the APF where the BRAM is of the CPU
#define APF_ACK 0x10
#define APF_DONE 0x8
#define APF_ERROR 0x7

#define DATASLOT_RAM_ACCESS(x) *(volatile unsigned int *)(DATASLOT_ID_BASE+(x))

#define DATASLOT_UPDATE_REG(x) *(volatile unsigned int *)(DATASLOT_UPDATE_FLAG_REG+(x<<2))

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
uint32_t dataslot_read_lba_set_fast(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length);
uint32_t dataslot_read_lba_set(uint16_t dataslot, uint32_t address, uint32_t offset);
uint32_t dataslot_read_lba(uint32_t length);
uint32_t dataslot_read_lba_fast(uint32_t length,  bool wait_update);
uint32_t dataslot_write_lba_set(uint16_t dataslot, uint32_t address, uint32_t offset);
uint32_t dataslot_write_lba(uint32_t length,  bool update_address);
bool      dataslot_ready();
uint8_t   dataslot_status();

#endif
