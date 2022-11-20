/*
-- OSD drawing code, borrowed from MiST's firmware.
-- Copyright (c) 2005 - 2020 by Dennis van Weeren, Jakub Bednarski,
-- Till Harbaum and others.

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

#include "osd.h"
#include "spi.h"
#include "font.h"

void spi_osd_cmd_cont(unsigned char cmd) {
  EnableOsd();
  SPI(cmd);
}

void spi_osd_cmd(unsigned char cmd) {
  spi_osd_cmd_cont(cmd);
  DisableOsd();
}

void spi_osd_cmd8_cont(unsigned char cmd, unsigned char parm) {
  EnableOsd();
  SPI(cmd);
  SPI(parm);
}

void spi_osd_cmd8(unsigned char cmd, unsigned char parm) {
  spi_osd_cmd8_cont(cmd, parm);
  DisableOsd();
}

static int osd_col;
static int osd_inv;
static int osd_stipple;
static int osd_writing;

void OsdWriteStart(int row,int inverse,int stipple)
{
	if(osd_writing)
		OsdWriteEnd();
    EnableOsd();
    SPI(MM1_OSDCMDWRITE|row);
	osd_col=0;
	osd_inv=inverse ? 255 : 0;
	osd_stipple=stipple ? 0x5555 : 0xffff;
	osd_writing=1;
}

void OsdPutChar(unsigned char c)
{
	int stipplemask=osd_stipple;
	int i;
    const unsigned char *p;
	if(c)
		p=&font[c-FONT_OFFSET][0];
	else
		p=&font[0][0];
	for(i=0;i<8;++i)
	{
        SPI((*p++&stipplemask)^osd_inv);
		stipplemask>>=1;
	}
	osd_col+=8;
}

void OsdPuts(unsigned char *s)
{
	unsigned char c;
	while(c=*s++)
		OsdPutChar(c);
}

void OsdWriteEnd()
{
	int i;
	while(osd_col++<OSDLINELEN)
		SPI(osd_inv);
	DisableOsd();
	osd_writing=0;
}

