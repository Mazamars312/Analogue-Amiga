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
#include "core.h"
#include "riscprintf.h"


extern "C" void _handle_trap(){
	DisableInterrupts();
	core_input_update();
	mainprintf("mouse \r\n");
	HW_TIMER1CLOCK(0) = 1;
	EnableInterrupts();
	return;
};

void EnableInterrupts(){
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=1;
}

void timer1Interrupts_setup(uint32_t timer){
	HW_TIMER1CLOCK(1) = timer;
}
void timer1Interrupts_reset(){
	HW_TIMER1CLOCK(0) = 1;
}

void DisableInterrupts()
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=0;
}
