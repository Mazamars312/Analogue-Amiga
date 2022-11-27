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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "minimig.h"
#include "printf.h"
#include "hardware.h"
#include "apf.h"
#include "spi.h"

void minimig_restart_first() {
  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , AFP_REGISTOR(1));
  // Get the chip setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, AFP_REGISTOR(2));
  // Get the Cpu setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CPU , AFP_REGISTOR(3));
  // Get the MEM setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_MEM , AFP_REGISTOR(4));
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , AFP_REGISTOR(5));
};

void minimig_restart_running_core() {
  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , AFP_REGISTOR(1));
  // Get the chip setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, AFP_REGISTOR(2));
  // Get the Cpu setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CPU , AFP_REGISTOR(3));
  // Get the MEM setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_MEM , AFP_REGISTOR(4));
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , AFP_REGISTOR(5));
};

void minimig_joystick_update(){

  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , AFP_REGISTOR(5));
}
