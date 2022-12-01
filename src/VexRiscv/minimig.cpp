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
#include "timer.h"
#include "spi.h"
#define DATASLOT_FDD_BASE 320 // These are just made up numbers that make sure that these data slots are used for FDD's and HDD's
#define DATASLOT_HDD_BASE 310
#define DATASLOT_BIOS_BASE 210 // BIOS Update


void minigmig_reset(int reset){
		HPS_spi_uio_cmd8(UIO_MM2_RST, reset);
		printf("reset %d\r\n",reset);
};
// Update dataslots

void minimig_update_dataslots(){
  int tmp = DATASLOT_UPDATE_REG(1);
	printf("dataslot Update %d\r\n", tmp);
	if (tmp == DATASLOT_BIOS_BASE){
		minigmig_reset(7);
		usleep(200);
		minimig_restart_first();
		usleep(200);
		minigmig_reset(0);
	} else if (tmp >= DATASLOT_FDD_BASE && tmp <= DATASLOT_FDD_BASE + 3 ) {
		minimig_fdd_update();
	}
};

void minimig_fdd_update(){
  int i = 0;
  while (i != 4)
  {
    if (dataslot_size(i+DATASLOT_FDD_BASE) != 0){
      InsertFloppy(&df[i], dataslot_size(i+DATASLOT_FDD_BASE), i+DATASLOT_FDD_BASE);
    }
    i++;
  }
  UpdateDriveStatus();
}

void minimig_poll_io(){
			// Get the mouse port updated

			minimig_input_update();

			// Get the keyboard port updated
			// minimig_keyboard_update();

      unsigned char  c1, c2;
  		HPS_EnableFpga();
  		uint16_t tmp = spi_w(0);
  		c1 = (uint8_t)(tmp >> 8); // cmd request and drive number
  		c2 = (uint8_t)tmp;      // track number
  		spi_w(0);
  		spi_w(0);
  		HPS_DisableFpga();
      HandleFDD(c1, c2);


};
