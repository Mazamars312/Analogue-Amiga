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

#ifndef HARDWARE_H
#define HARDWARE_H

#include <inttypes.h>
#include <stdio.h>

#define RESTARTBASE               0xFFFFFFA4
#define AFP_REGISTOR_BASE         0xffffff00
#define IO_CORE_REGISTOR_BASE     0xffffff50
#define CONTROLLER_KEY_BASE       0xffffff20
#define CONTROLLER_JOY_BASE       0xffffff30
#define CONTROLLER_TRIG_BASE      0xffffff40
#define MISTERGPOHARDWAREBASE     0xffffffd0
#define MISTERGPIHARDWAREBASE     0xffffffd4
#define DATASLOT_BRAM_BASE        0xffff0000

#define RAMBUFFER_BASE            0x00008000

#define SYS_CLOCK  7425 // This is the CPU clock speed in a int size 74.2mhz
#define UART_RATE  4608 // This is the UART Rate shifted right by 2

#define RESTARTBASE 0xFFFFFFA4

#define RESET_CORE(x) *(volatile unsigned int *)(RESTARTBASE+x)

#define AFP_REGISTOR(x) *(volatile unsigned int *)(AFP_REGISTOR_BASE+(x<<2))


#define CORE_OUTPUT_REGISTOR() *(volatile unsigned int *)(IO_CORE_REGISTOR_BASE+(4))
#define CORE_INPUT_REGISTOR()  *(volatile unsigned int *)(IO_CORE_REGISTOR_BASE+(0))

#define CONTROLLER_KEY_REG(x)  *(volatile unsigned int *)(CONTROLLER_KEY_BASE +((x-1)<<2))
#define CONTROLLER_JOY_REG(x)  *(volatile unsigned int *)(CONTROLLER_JOY_BASE +((x-1)<<2))
#define CONTROLLER_TRIG_REG(x) *(volatile unsigned int *)(CONTROLLER_TRIG_BASE+((x-1)<<2))

#define RAMBUFFER(x) *(volatile unsigned int *)(RAMBUFFER_BASE + (x<<2))

#define DATASLOT_BRAM(x) *(volatile unsigned int *)(DATASLOT_BRAM_BASE+(x<<2))

#endif // SPI_H
