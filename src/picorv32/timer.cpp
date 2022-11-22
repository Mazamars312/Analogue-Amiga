#include "timer.h"
#include "hardware.h"

unsigned int GetTimer(unsigned int offset)
{
    unsigned int systimer = HW_TIMER(REG_MILLISECONDS);
    systimer += offset;
    return (systimer);
}

unsigned int CheckTimer(unsigned int time)
{
    unsigned int systimer = HW_TIMER(REG_MILLISECONDS);
    time -= systimer;
    return(time > (1UL << 31));
}
