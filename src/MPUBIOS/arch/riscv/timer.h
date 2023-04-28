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
#include <stdio.h>

// We have two timers one for User use and the other for the Keyboard and mouse checks
#define TIMERBASE1 0xffffffc4
#define TIMERBASE1_RESET 0xffffffc8
#define TIMERBASE2_RESET 0xffffffcc
#define TIMERBASE2 0xffffffcc
#define TIMERBASE1_FAST 0xfffffff8
#define TIMERBASE2_FAST 0xfffffffc
#define SYSCLOCKBASE 0xffffff98
#define TIMERCHECKBASE 0xffffffF4
#define HW_TIMER1(x) *(volatile unsigned int *)(TIMERBASE1+x)
#define HW_TIMER1_RESET(x) *(volatile unsigned int *)(TIMERBASE1_RESET+x)
#define HW_TIMER2_RESET(x) *(volatile unsigned int *)(TIMERBASE2+x)
#define HW_TIMER2(x) *(volatile unsigned int *)(TIMERBASE2+x)
#define HW_TIMER1_FAST(x) *(volatile unsigned int *)(TIMERBASE1_FAST+x)
#define HW_TIMER2_FAST(x) *(volatile unsigned int *)(TIMERBASE2_FAST+x)
#define HW_SYSCLOCK(x) *(volatile unsigned int *)(SYSCLOCKBASE+x)
#define HW_TIMERCHECKBASE(x) *(volatile unsigned int *)(TIMERCHECKBASE+x)
#define REG_MILLISECONDS 0

unsigned int GetTimer();
unsigned int CheckTimer(unsigned int time);
// Will reset the timer
void ResetTimer();
void SetTimer();
void riscusleep(int time);
// void usleep(unsigned int time);
void Set_interrupt_Timer(unsigned int time);

unsigned int GetTimer2();
unsigned int GetTimer2_fast();
unsigned int CheckTimer2(unsigned int time);
uint32_t RISCGetTimer1(uint32_t offset, int fraction);
uint32_t RISCCheckTimer1(uint32_t time);
uint32_t RISCGetTimer2(uint32_t offset, int fraction);
uint32_t RISCCheckTimer2(uint32_t time);
void reset_interrupt_Timer();
// Will reset the timer
void ResetTimer1();
void ResetTimer2();

#endif
