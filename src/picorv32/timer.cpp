#include "timer.h"
#include "hardware.h"

// this will get the clock timer and respone back with a millisecond_counter
unsigned int GetTimer()
{
    unsigned int systimer = HW_TIMER(REG_MILLISECONDS);
    return (systimer);
}

// A true or false condition. place the millisecond_counter required and this will test it
unsigned int CheckTimer(unsigned int time)
{
    unsigned int systimer = HW_TIMER(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}


// will reset the counter THere is a wait of about 10 clocks to make sure this happens
void ResetTimer()
{
  int i = 0;
  HW_TIMER(0) = 1;
  while (i <= 10){
    i++;
  }
  HW_TIMER(0) = 0;
};

void SetTimer(unsigned int time)
{
  HW_SYSCLOCK(0) = time * 100;
};

void usleep(unsigned int time)
{
  ResetTimer();
  while (!CheckTimer(time)){};
  return;
};
