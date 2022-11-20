# DeMiSTify
Support code intended to assist in the porting of MiST FPGA cores
to other target boards, specifically the Turbo Chameleon 64 cartridges,
though the board does now support other targets, too.
Copyright (c) 2021 by Alastair M. Robinson

## Target boards
Currently DeMiSTify contains target files for the following boards:
* Turbo Chameleon 64
* Turbo Chameleon 64 V2
* DE10 Lite (a debugging rather than end-user target -
external PS/2 keyboard + SD card interface required.
* DECA - experimental, external SDRAM, PS/2 keyboard required, optional
external VGA.  Thanks to rampa069 and somhic
* Neptuno - thanks to rampa069 and somhic.
* UnAmiga Reloaded - thanks to rampa069 and somhic
* SiDi - thanks to rampa069
* Atlas Cyc1000 - thanks to somhic

## Please note
If you use this project to port a MiST FPGA core to a new board, please
respect the license of this project and also of the core being ported.

Generally, this means you are obliged to supply on request the full
source and project files which correspond to any binary release (whether
that binary takes the form of a file, or is pre-programmed into a chip)
to anyone who has received the binary - or better yet, if the binary is
available for download make the source available for download from the
same place.

## License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with with this program.
	If not, see <https://www.gnu.org/licenses/>.
