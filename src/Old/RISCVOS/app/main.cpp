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
 // #include <stdint.h>
 // #include <unistd.h>
 // #include <stdio.h>
 // #include <stdlib.h>

#include "interrupts.h"
#include "hardware.h"
#include "timer.h"
#include "uart.h"
#include "apf.h"
#include "printf.h"
#include "spi.h"
#include "core.h"
#include "inputs.h"

void init()
{
  // this makes the core go in to the reset state and halts the bus so we can upload the bios if required
  core_reset(7);
	// This setups the timers and the CPU clock rate on the system.
  DisableInterrupts();
  // Setup the core to know what MHZ the CPU is running at.
	SetTimer();
	SetUART();
	reset_interrupt_Timer();
  riscusleep(500000);
  printf("\033[0m");
  // printf_register(riscputc);
  // This is where you setup the core startup dataslots that the APT loads up for your core.
  core_restart_first(); // we want to setup the core now :-) but with no startup yet
}

void mainloop()
{
  // We like to see over the UART that hey I did something :-)
	riscusleep(9000);
	printf("RISC MPU Startup core\r\n");
	printf("Created By Mazamars312\r\n");
	printf("Make in 2022\r\n");
	printf("I hate this debugger LOL\r\n");
	// I do enjoy reseting this timer.
  minimig_restart_first();
  core_update_dataslots();
	riscusleep(2000);
	core_reset(0); // Turns off all the resets via the HPS bus
	core_input_setup();
  EnableInterrupts();
	// This loop is what keeps the checking of the core interface
  printf("about to go in baby\r\n");
  while(true){

		if(DATASLOT_UPDATE_REG(0)){
		// 		// This subprogram is for the program to check what dataslots are updated by the APF
			DisableInterrupts(); // we want to make sure when updating the dataslots - interrupts are stopped.
      printf("Data Slot Update\r\n");
      core_update_dataslots();
      printf("Data Slot done\r\n");
			EnableInterrupts(); // Re-enable interrupts
		}

	  core_poll_io();
    input_reg_update();
    core_reg_update();

    // This checks if we are wanting to do a reboot of the core from the interaction menu. We want to do this last so nothing is held in the buffers
    if (AFP_REGISTOR(0) & 0x3) { // This has 2 bits for reseting the core
      DisableInterrupts();
      printf("starting the reset\r\n");
      core_reset(AFP_REGISTOR(0));
      riscusleep(200);
      AFP_REGISTOR(0) = 0;
      minimig_restart_running_core(); // we now do a reset of the core and update the regs if changed
      riscusleep(2000);
      core_reset(0);
      riscusleep(500);
      RemoveDriveStatus();
      riscusleep(3000000);
      UpdateDriveStatus();
      printf("Completed the reset\r\n");
      EnableInterrupts();
    }
	};
}

int main()
{

	init();
	riscusleep(20000000); // I want the core to wait a bit.
	mainloop();
	return(0);
}

void irqCallback(){
}
