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

 #ifndef FILEIO_H
 #define FILEIO_H

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #include <inttypes.h>

// #include <stdlib.h>
#include <string.h>
// #include <inttypes.h>
#include <stdio.h>
#include "hardware.h"
#include "apf.h"
#include "spi.h"
#include "printf.h"
#include "timer.h"
#include "hardware.h"


#define MEM_BUFFER_SIZE_C (16 * 1024)
#define MEM_BUFFER_SIZE_TOTAL (32 * 1024)

void clearbufferram();
uint32_t RISCFileSeek(uint16_t dataslot, uint32_t offset, uint32_t size); // This is the buffer setup
uint32_t RISCFileReadAdv(int dataslot, uint8_t cmd0, uint8_t cmd1, uint8_t cmd_hps, uint32_t offset); // Will read and then send the data to the HPS bus - Great for CDrom sending data
uint32_t RISCFileReadSec(uint32_t buffer, uint16_t dataslot, uint32_t offset, uint32_t size); // Will read a buffer and place it on the buffer location
uint32_t RISCFileWriteSec(uint32_t buffer, uint16_t dataslot, uint32_t offset, uint32_t size); // Will write a buffer and place it on the buffer location

#endif
