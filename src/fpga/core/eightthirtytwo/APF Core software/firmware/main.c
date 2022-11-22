/*
-- Minimalist MiST-compatibile controller firmware
-- Copyright (c) 2021 by Alastair M. Robinson

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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"
// #include "spi.h"
#include "interrupts.h"
// #include "user_io.h"
// #include "cue_parser.h"
// #include "pcecd.h"
// #include "diskimg.h"



fileTYPE file;

__weak void init()
{
	EnableInterrupts();
}


__weak void mainloop()
{
	while(1)
	{
		puts("Testing\n");
		// diskimg_poll();
	}
}

__weak int main(int argc,char **argv)
{
	init();
	mainloop();
	return(0);
}
