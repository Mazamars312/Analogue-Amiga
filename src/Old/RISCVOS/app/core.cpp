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



//  #include <stdint.h>
//  #include <unistd.h>
//  #include <stdio.h>
//  #include <stdlib.h>
// #include <inttypes.h>


#include "core.h"

// Sends a reset via the HPS Bus
void core_reset(int reset){
	HPS_spi_uio_cmd8(UIO_MM2_RST, reset);
	printf("reset %d\r\n",reset);
};

void full_core_reset(){
	printf("starting the reset\r\n");
	core_reset(0); // this sends the reset command to core - This one uses the AFP registor for this as the Amiga has 3 types of reset.
	riscusleep(200);
	AFP_REGISTOR(0) = 0;	// This makes the reset registor on the APF bus go back to zero for the user to see - need to do a read-modify-write back here
	riscusleep(20000);
	minimig_restart_first();
	core_update_dataslots();
	core_reset(1);	// activate the core
	riscusleep(500);
	printf("Completed the reset\r\n");
}
// Update dataslots

void core_update_dataslots(){
	int tmp = DATASLOT_UPDATE_REG(1);
	printf("dataslot Update %d\r\n", tmp);
	if (tmp == DATASLOT_BIOS_BASE){
		core_reset(7);
		riscusleep(200);
		minimig_restart_first();
		riscusleep(200);
		core_reset(0);
	} else if (tmp >= DATASLOT_FDD_BASE && tmp <= DATASLOT_FDD_BASE + 3 ) {
		minimig_fdd_update();
	}
};

void core_poll_io(){
      // Here is where you do your polling on the core - eg Floppy and CDROM data

      unsigned char  c1, c2;
  		HPS_EnableFpga();
  		uint16_t tmp = spi_w(0);
  		c1 = (uint8_t)(tmp >> 8); // cmd request and drive number
  		c2 = (uint8_t)tmp;      // track number
  		spi_w(0);
  		spi_w(0);
  		HPS_DisableFpga();
      HandleFDD(c1, c2);

			// HandleHDD( );

};

void core_reg_update(){
	// This can be used for polling the APF regs from the interaction menu to change core settings
	// Region setup
	if (AFP_REGISTOR(1) & 0x1) cue.SetRegion(1);
	else cue.SetRegion(0);
	// if (old_region != AFP_REGISTOR(1) & 0x1) full_core_reset();
	// Arcade Setup

	if (AFP_REGISTOR(1) & 0x2) CORE_OUTPUT_REGISTOR() = 0x2;
	else CORE_OUTPUT_REGISTOR() = 0x0;
	// if (old_arcade != AFP_REGISTOR(1) & 0x2) full_core_reset();

};



void core_restart_first(){
	// what to do to start up the core if required
};

void core_restart_running_core() {
	// this can be used for restarting the core

};

void core_input_setup() {
	// this is used for controller setup
	input_setup();
};

void core_input_update() {
	// this is called via the interrupts.c for controller updates to the core
	input_update();
};

void core_interupt_update(){

};
