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
#include <strings.h>
#include <inttypes.h>

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "interrupts.h"
#include "hardware.h"
#include "timer.h"
#include "uart.h"
#include "apf.h"
#include "printf.h"
#include "spi.h"
#include "minimig.h"


// these values help with both the UART and System clock setups
uint32_t sys_clock = 284; // This is the CPU clock speed in a int size 28.6mhz
uint32_t uart_rate = 1152; // This is the UART Rate shifted right by 2

void init()
{
	// This setups the timers and the CPU clock rate on the system.
  DisableInterrupts();
	SetTimer(sys_clock);
	SetUART(sys_clock, uart_rate);
	ResetTimer();

	// This is where you setup the core startup dataslots that the APT loads up for your core.
  minimig_fdd_update();

	return;
}

void mainloop()
{
  // We like to see over the UART that hey I did something :-)
	usleep(100);
	printf("RISC MPU Startup core\r\n");
  printf("Created By Mazamars312\r\n");
  printf("Make in 2022\r\n");
	usleep(5000);
	minigmig_reset(1);
	// I do enjoy reseting this timer.

	// This loop is what keeps the checking of the core interface
	while(true){
		if(DATASLOT_UPDATE_REG(0)){
				// This subprogram is for the program to check what dataslots are updated
			minimig_fdd_update();
      // printf("updated fdd\r\n");
		}
		// Im waiting to introduce a delay in floppy drive changes in the APF framwork
		// THis will read the timer and will wait to mount the disk. the value is in seconds
		minimig_timer_update();

    // printf("updated timer_update\r\n");
		// This does the polling of the core and will then run the processes required for it
		minimig_poll_io();

    // printf("updated poll_io\r\n");
	};
}

int main()
{
	usleep(2000);
	// printf ("\r\n%d\r\n",CheckTimer(0));
  minigmig_reset(0);
	init();
	mainloop();

	return(0);
}
