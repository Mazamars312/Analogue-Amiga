#ifndef STATUSWORD_H
#define STATUSWORD_H

extern unsigned int statusword; /* Support 32-bit status word initially - need to be 64-bit in the long run */

__weak void sendstatus();
void statusword_cycle(unsigned int shift,unsigned int mask,unsigned int limit);
unsigned int statusword_get(unsigned int shift,unsigned int mask);

#endif

