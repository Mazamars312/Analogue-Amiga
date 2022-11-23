#ifndef TIMER_H

#define TIMERBASE 0xffffffc8
#define HW_TIMER(x) *(volatile unsigned int *)(TIMERBASE+x)
#define REG_MILLISECONDS 0

unsigned int GetTimer();
unsigned int CheckTimer(unsigned int time);
// Will reset the timer
void ResetTimer();

#endif
