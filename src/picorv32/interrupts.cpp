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

#include <stdio.h>
#include "interrupts.h"

#if 0
static void dummy_handler()
{
	GetInterrupts();
}
#endif

void SetIntHandler(void(*handler)())
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=0;
	*(void **)13=(void *)handler;
//	puts("Set handler\n");
}

#if 0
__constructor(100.interrupts) void intconstructor()
{
//	puts("In interrupt constructor\n");
	SetIntHandler(dummy_handler);
}
#endif

volatile int GetInterrupts()
{
	return(HW_INTERRUPT(REG_INTERRUPT_CTRL));
}


void EnableInterrupts()
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=1;
}


void DisableInterrupts()
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=0;
}

