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

#ifndef MINIMIG_H
#define MINIMIG_H
extern uint8_t rstval;
#include "minimig_fdd.h"
#include "minimig_config.h"
#include "minimig_inputs.h"

// Add your operating regs here from the bus
#define UIO_STATUS      0x00
#define UIO_BUT_SW      0x01
#define UIO_MOUSE_BTN   0x02
#define UIO_MOUSE_X	    0x03
#define UIO_MOUSE_Y	    0x04
#define UIO_KEYBOARD    0x05

// ao486 direct memory access
#define UIO_DMA_WRITE   0x61
#define UIO_DMA_READ    0x62
#define UIO_DMA_SDIO    0x63

void minigmig_reset(int reset);
void minimig_update_dataslots();
void minimig_fdd_update();
void minimig_poll_io();

#endif
