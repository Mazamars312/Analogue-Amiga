/*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski
Copyright 2022 Murray Aickin

This file is part of Minimig

Minimig is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Minimig is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 2009-11-14   - adapted gap size
// 2009-12-24   - updated sync word list
//              - fixed sector header generation
// 2010-01-09   - support for variable number of tracks
// 2022-11-25   - Added Pocket APF access

#ifndef MINIMIG_CONFIG_H
#define MINIMIG_CONFIG_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// ---- Minimig v2 constants -------
#define UIO_MM2_RST     0xF0
#define UIO_MM2_AUD     0xF1
#define UIO_MM2_CHIP    0xF2
#define UIO_MM2_CPU     0xF3
#define UIO_MM2_MEM     0xF4
#define UIO_MM2_VID     0xF5
#define UIO_MM2_FLP     0xF6
#define UIO_MM2_HDD     0xF7
#define UIO_MM2_JOY     0xF8
#define UIO_MM2_MIR     0xF9

void minimig_restart_first();
void minimig_restart_running_core();

#ifdef __cplusplus
}
#endif
#endif
