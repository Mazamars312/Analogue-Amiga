/*
 * Copyright 2018 Jacob Lifshay
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
//#include <string.h>
//#include <stdlib.h>
//#include <math.h>
#include <strings.h>

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
// #include "minimig_fdd.h"
#include "spi.h"


// these values help with both the UART and System clock setups
uint32_t sys_clock = 284; // This is the CPU clock speed in a int size 28.6mhz
uint32_t uart_rate = 1152; // This is the UART Rate shifted right by 2

void init()
{
	// This setups the timers and the CPU clock rate on the system.
	SetTimer(sys_clock);
	SetUART(sys_clock, uart_rate);
	ResetTimer();
  DisableInterrupts();
	return;
}

void mainloop()
{

	usleep(100);
	printf("\r\n Startup \r\n");
	printf("RISC MPU Startup core\r\n");
  printf("Created By Mazamars312\r\n");
	usleep(500);
	ResetTimer();
	while(true){
		unsigned char  c1, c2;
		EnableFpga();
		uint16_t tmp = spi_w(0);
		c1 = (uint8_t)(tmp >> 8); // cmd request and drive number
		c2 = (uint8_t)tmp;      // track number
		spi_w(0);
		spi_w(0);
		DisableFpga();
		// HandleFDD(c1, c2);
	}

}

int main()
{

	init();
	mainloop();

	return(0);
}
