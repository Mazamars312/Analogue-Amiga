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

void minimig_bios (){
  int tmp;
  tmp = dataslot_size(210);
  if (tmp == 0x100000) {
    printf("\r\n1M Rom\r\n");
    dataslot_read(210, 0x00F00000, 0, 0x100000);

  }
  else if (tmp == 0x80000){
    printf("\r\n512M Rom\r\n");
    dataslot_read(210, 0x00F00000, 0, 0x80000);
    dataslot_read(210, 0x00F80000, 0, 0x80000);

  }
  else if (tmp ==  0x40000){
    printf("\r\n256M Rom\r\n");
    dataslot_read(210, 0x00f00000, 0, 0x40000);
    dataslot_read(210, 0x00f40000, 0, 0x40000);
    dataslot_read(210, 0x00f80000, 0, 0x40000);
    dataslot_read(210, 0x00fc0000, 0, 0x40000);
  }
  printf("DONE\r\n");
};

void minimig_restart_first() {
  // Bios size
  minimig_bios();
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
  // Update the Floppy controller
  minimig_fdd_update();
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
