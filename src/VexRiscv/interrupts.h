/*
-- EightThirtyTwo interrupt handling code
-- Copyright (c) 2020 by Alastair M. Robinson

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty
-- of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "hardware.h"
extern "C" void _handle_trap();
#define INTERRUPTBASE 0xFFFFFFF4
//
#define HW_INTERRUPT(x) *(volatile unsigned int *)(INTERRUPTBASE+x)
//
// // Interrupt control register
// // Write a '1' to the low bit to enable interrupts, '0' to disable.
// // Reading returns a set bit for each interrupt that has been triggered since
// // the last read, and also clears the register.
//
#define REG_INTERRUPT_CTRL 0x0
//
//
// void SetIntHandler(void(*handler)());
void EnableInterrupts();
void DisableInterrupts();
// volatile int GetInterrupts();


#endif
