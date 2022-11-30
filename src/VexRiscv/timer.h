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

#ifndef TIMER_H

// We have two timers one for User use and the other for the Keyboard and mouse checks
#define TIMERBASE1 0xffffffc4
#define TIMERBASE2 0xffffffc8
#define SYSCLOCKBASE 0xffffff98
#define HW_TIMER1(x) *(volatile unsigned int *)(TIMERBASE1+x)
#define HW_TIMER2(x) *(volatile unsigned int *)(TIMERBASE2+x)
#define HW_SYSCLOCK(x) *(volatile unsigned int *)(SYSCLOCKBASE+x)
#define REG_MILLISECONDS 0

unsigned int GetTimer();
unsigned int CheckTimer(unsigned int time);
// Will reset the timer
void ResetTimer();
void SetTimer(unsigned int time);
void usleep(unsigned int time);

unsigned int GetTimer1();
unsigned int CheckTimer1(unsigned int time);
// Will reset the timer
void ResetTimer1();

#endif
