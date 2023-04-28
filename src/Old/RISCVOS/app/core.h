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

#ifndef CORE_H
#define CORE_H

#include "cue.h"
#include "minimig_config.h"
#include "printf.h"
#include "hardware.h"
#include "hardware.h"
#include "apf.h"
#include "cue.h"
#include "timer.h"
#include "inputs.h"
#include "spi.h"
#include "fdd.h"

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

#define DATASLOT_FDD_BASE 320 // These are just made up numbers that make sure that these data slots are used for FDD's and HDD's
#define DATASLOT_HDD_BASE 310
#define DATASLOT_CD_BASE 100 // These are just made up numbers that make sure that these data slots are used for CD's
#define DATASLOT_BIOS_BASE 210 // BIOS Update

void core_reset(int reset);
void core_update_dataslots();
void core_fdd_update();
void core_poll_io();
void core_reg_update();
void core_restart_first();
void core_restart_running_core();
void core_input_setup();
void core_input_update();
void core_interupt_update();

void full_core_reset();
#endif
