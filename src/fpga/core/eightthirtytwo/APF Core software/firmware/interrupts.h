#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define INTERRUPTBASE 0xffffffb0
#define HW_INTERRUPT(x) *(volatile unsigned int *)(INTERRUPTBASE+x)

// Interrupt control register
// Write a '1' to the low bit to enable interrupts, '0' to disable.
// Reading returns a set bit for each interrupt that has been triggered since
// the last read, and also clears the register.

#define REG_INTERRUPT_CTRL 0x0

#ifdef __cplusplus
extern "C" {
#endif

void SetIntHandler(void(*handler)());
void EnableInterrupts();
void DisableInterrupts();
volatile int GetInterrupts();

#ifdef __cplusplus
}
#endif

#endif

