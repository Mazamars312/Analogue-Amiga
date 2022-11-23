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
//#include <strings.h>

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
#include "spi.h"
#include "apf.h"
#include "printf.h"
// const char buffer;


void init()
{
  EnableInterrupts();
	return;
}

void mainloop()
{
	while(!CheckTimer(10000)){

	}
	printf("\r\n Startup \r\n");
	printf("RISC MPU Startup core\r\n");
  printf("Mazamars312 \r\n");
	while(!CheckTimer(20000)){

	}
	while(true){
		printf("n: %d \r\n", GetTimer(0));
    printf("n: %d \r\n", CheckTimer(4000));
	}

}

int main()
{

	init();
	mainloop();

	return(0);
}
