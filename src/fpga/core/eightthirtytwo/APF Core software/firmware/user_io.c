/*
-- Userio code from mist-firmware

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

#include "user_io.h"
#include "spi.h"
#include "minfat.h"

/* User_io related SPI functions */
void spi_uio_cmd_cont(unsigned char cmd) {
  EnableIO();
  SPI(cmd);
}

void spi_uio_cmd(unsigned char cmd) {
  spi_uio_cmd_cont(cmd);
  DisableIO();
}

void spi_uio_cmd8_cont(unsigned char cmd, unsigned char parm) {
  EnableIO();
  SPI(cmd);
  SPI(parm);
}

void spi_uio_cmd8(unsigned char cmd, unsigned char parm) {
  spi_uio_cmd8_cont(cmd, parm);
  DisableIO();
}

#if 0
void spi_uio_cmd32(unsigned char cmd, unsigned long parm) {
  EnableIO();
  SPI(cmd);
  SPI(parm);
  SPI(parm>>8);
  SPI(parm>>16);
  SPI(parm>>24);
  DisableIO();
}
#endif

void user_io_digital_joystick_ext(unsigned char joystick, uint16_t map) {
	spi_uio_cmd8(UIO_JOYSTICK0_EXT + joystick, map);
}

void user_io_digital_joystick(unsigned char joystick, unsigned char map) {
	if(joystick<2)
		joystick+=UIO_JOYSTICK0;
	else
		joystick+=UIO_JOYSTICK2-2;
	spi_uio_cmd8(joystick, map);
}

void user_io_analogue_joystick(unsigned char joystick, int *map) {
	spi_uio_cmd8_cont(UIO_ASTICK, joystick);
	SPI((*map++)>>8);
	SPI((*map++)>>8);
	DisableIO();
}


void user_io_send_rtc(char *buf){
	int i=8;
	spi_uio_cmd_cont(UIO_SET_RTC);
	while(i--)
		SPI(*buf++);
	DisableIO();
}

