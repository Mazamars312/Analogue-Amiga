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

#include "timer.h"
#include "hardware.h"

// this will get the clock timer and respone back with a millisecond_counter - this is used for the timer interupt
unsigned int GetTimer()
{
    unsigned int systimer = HW_TIMER1(REG_MILLISECONDS);
    return (systimer);
}

// A true or false condition. place the millisecond_counter required and this will test it - this is used for the timer interupt
unsigned int CheckTimer(unsigned int time)
{
    unsigned int systimer = HW_TIMER1(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}

void Set_interrupt_Timer(unsigned int time)
{
    HW_TIMER1_RESET(REG_MILLISECONDS) = time;
}

// will reset the counter THere is a wait of about 10 clocks to make sure this happens - this is used for the timer interupt

void reset_interrupt_Timer()
{
  int i = 0;
  HW_TIMER1(0) = 1;
  while (i <= 100){
    i++;
    asm volatile(""); // needed as loops do not work in G++ RISCV compilers
  }
  HW_TIMER1(0) = 0;
}


void SetTimer() // makes a time length - this is used for the timer interupt
{
  HW_SYSCLOCK(0) = SYS_CLOCK * 100;
};

void riscusleep(int time)
{
  for (int i = 0; i < time; i++) {
    asm volatile(""); // needed as loops do not work in G++ RISCV compilers
  };
};

// These are used for the for the user for timing if required.

unsigned int GetTimer2()
{
    unsigned int systimer = HW_TIMER2(REG_MILLISECONDS);
    return (systimer);
}

unsigned int GetTimer2_fast() // as the per clock amount since reset.
{
    unsigned int systimer = HW_TIMER2_FAST(REG_MILLISECONDS);
    return (systimer);
}

uint32_t RISCGetTimer(uint32_t offset)
{
	uint32_t res;
	res = HW_TIMER2_FAST(REG_MILLISECONDS);
	res = (res / (SYS_CLOCK * 10));
	return (uint32_t)(res + offset);
}

uint32_t RISCCheckTimer(uint32_t time)
{
	return (!time) || (RISCGetTimer(0) >= time);
}

unsigned int CheckTimer2(unsigned int time)
{
    unsigned int systimer = HW_TIMER2(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}

void ResetTimer2()
{
  int i = 0;
  HW_TIMER2(0) = 1;
  while (i <= 10){
    i++;
    asm volatile(""); // needed as loops do not work in G++ RISCV compilers
  }
  HW_TIMER2(0) = 0;
};
