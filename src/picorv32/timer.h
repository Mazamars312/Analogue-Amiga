#ifndef TIMER_H

#define TIMERBASE 0xffffffc8
#define SYSCLOCKBASE 0xffffff98
#define HW_TIMER(x) *(volatile unsigned int *)(TIMERBASE+x)
#define HW_SYSCLOCK(x) *(volatile unsigned int *)(SYSCLOCKBASE+x)
#define REG_MILLISECONDS 0

unsigned int GetTimer();
unsigned int CheckTimer(unsigned int time);
// Will reset the timer
void ResetTimer();
void SetTimer(unsigned int time);
void usleep(unsigned int time);

#endif
