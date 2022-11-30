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

// this will get the clock timer and respone back with a millisecond_counter
unsigned int GetTimer()
{
    unsigned int systimer = HW_TIMER2(REG_MILLISECONDS);
    return (systimer);
}

// A true or false condition. place the millisecond_counter required and this will test it
unsigned int CheckTimer(unsigned int time)
{
    unsigned int systimer = HW_TIMER2(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}


// will reset the counter THere is a wait of about 10 clocks to make sure this happens
void ResetTimer()
{
  int i = 0;
  HW_TIMER2(0) = 1;
  while (i <= 100){
    i++;
  }
  HW_TIMER2(0) = 0;
};

void SetTimer(unsigned int time)
{
  HW_SYSCLOCK(0) = time * 100;
};

void usleep(unsigned int time)
{
  uint32_t i = 0;
  while (i <= time){
    i++;
  }
};

unsigned int GetTimer1()
{
    unsigned int systimer = HW_TIMER1(REG_MILLISECONDS);
    return (systimer);
}

unsigned int CheckTimer1(unsigned int time)
{
    unsigned int systimer = HW_TIMER1(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}

void ResetTimer1()
{
  int i = 0;
  HW_TIMER1(0) = 1;
  while (i <= 10){
    i++;
  }
  HW_TIMER1(0) = 0;
};
