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
#include "printf.h"
#include "inputs.h"


extern "C" void _handle_trap(){
	printf("HELLO%s\r\n");
	// input_update();
	return;
};

void EnableInterrupts()
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=0x001;
}


void DisableInterrupts()
{
	HW_INTERRUPT(REG_INTERRUPT_CTRL)=0x000;
}
