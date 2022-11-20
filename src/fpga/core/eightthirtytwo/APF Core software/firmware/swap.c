/*
-- Endian-handling code
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

unsigned int SwapBBBB(unsigned int i)
{
	unsigned int result=(i>>24)&0xff;
	result|=(i>>8)&0xff00;
	result|=(i<<8)&0xff0000;
	result|=(i<<24)&0xff000000;
	return(result);
}

unsigned int SwapBB(unsigned int i)
{
	unsigned short result=(i>>8)&0xff;
	result|=(i<<8)&0xff00;
	return(result);
}

unsigned long SwapWW(unsigned long i)
{
	unsigned int result=(i>>16)&0xffff;
	result|=(i<<16)&0xffff0000;
	return(result);
}
