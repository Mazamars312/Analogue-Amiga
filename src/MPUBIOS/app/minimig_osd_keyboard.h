/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


#ifndef minimig_osd_keyboard_H
#define minimig_osd_keyboard_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "riscprintf.h"
#include "hardware.h"
#include "spi.h"
#include "osd.h"
#include "inputs.h"
void OSD_Keyboard_process();

static const uint8_t osdKeyboardlocater[6][15] = {
    {AMIGA_ESC,       AMIGA_F1,        AMIGA_F2,        AMIGA_F3,        AMIGA_F4,        AMIGA_F5,        AMIGA_F6,        AMIGA_F7,        AMIGA_F8,        AMIGA_F9,        AMIGA_F10,       AMIGA_HELP,       AMIGA_UNKNOWN,   AMIGA_UNKNOWN,    AMIGA_UNKNOWN},
    {AMIGA_BACKTICK,  AMIGA_ONE,       AMIGA_TWO,       AMIGA_THREE,     AMIGA_FOUR,      AMIGA_FIVE,      AMIGA_SIX,       AMIGA_SEVEN,     AMIGA_EIGHT,     AMIGA_NINE,      AMIGA_ZERO,      AMIGA_DASH,       AMIGA_EQUALS,    AMIGA_BACKSP,     AMIGA_DELETE},
    {AMIGA_TAB,       AMIGA_Q,         AMIGA_W,         AMIGA_E,         AMIGA_R,         AMIGA_T,         AMIGA_Y,         AMIGA_U,         AMIGA_I,         AMIGA_O,         AMIGA_P,         AMIGA_OSQPARENS,  AMIGA_CSQPARENS, AMIGA_BACKSLASH,    AMIGA_UNKNOWN},
    {AMIGA_CAPSLOCK,  AMIGA_A,         AMIGA_S,         AMIGA_D,         AMIGA_F,         AMIGA_G,         AMIGA_H,         AMIGA_J,         AMIGA_K,         AMIGA_L,         AMIGA_SEMICOLON, AMIGA_QUOTE,      AMIGA_RETURN,    AMIGA_UNKNOWN,    AMIGA_UNKNOWN},
    {AMIGA_LSHIFT,    AMIGA_Z,         AMIGA_X,         AMIGA_C,         AMIGA_V,         AMIGA_B,         AMIGA_N,         AMIGA_M,         AMIGA_COMMA,     AMIGA_PERIOD,  AMIGA_SLASH,     AMIGA_RSHIFT,     AMIGA_UP,        AMIGA_UNKNOWN,    AMIGA_UNKNOWN},
    {AMIGA_CTRL,      AMIGA_LALT,      AMIGA_LAMIGA,    AMIGA_SPACE,     AMIGA_RAMIGA,    AMIGA_RALT,      AMIGA_LEFT,      AMIGA_DOWN,      AMIGA_RIGHT,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,    AMIGA_UNKNOWN,   AMIGA_UNKNOWN},
};

#endif
