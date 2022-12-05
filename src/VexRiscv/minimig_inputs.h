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

#ifndef MINIMIG_INPUTS_H
#define MINIMIG_INPUTS_H

#include "minimig.h"
#include "hardware.h"
#include "timer.h"
#include "printf.h"
#include "spi.h"

#define NONE         0xFF
#define LCTRL        0x000100
#define LSHIFT       0x000200
#define LALT         0x000400
#define LGUI         0x000800
#define RCTRL        0x001000
#define RSHIFT       0x002000
#define CAPS_TOGGLE  0x040000
#define RALT         0x004000
#define RGUI         0x008000
#define MODMASK      0x00FF00

extern unsigned int keytable[16];
extern unsigned int mouse_counter;
extern signed short x_count;
extern signed short y_count;

void minimig_input_update();
void minimig_input_setup();
void minimig_joystick_reg_update();

// amiga keycodes (transcribed from amiga developer cd 2.1)
#define AMIGA_BACKTICK  0x00 // backtick / shifted tilde
#define AMIGA_ONE       0x01 // 1 / shifted exclaim
#define AMIGA_TWO       0x02 // 2 / shifted at
#define AMIGA_THREE     0x03 // 3 / shifted hash
#define AMIGA_FOUR      0x04 // 4 / shifted dollar
#define AMIGA_FIVE      0x05 // 5 / shifted percent
#define AMIGA_SIX       0x06 // 6 / shifted caret
#define AMIGA_SEVEN     0x07 // 7 / shifted ampersand
#define AMIGA_EIGHT     0x08 // 8 / shifted asterisk
#define AMIGA_NINE      0x09 // 9 / shifted open parens
#define AMIGA_ZERO      0x0a // 0 / shifted close parens
#define AMIGA_DASH      0x0b // dash / shifted underscore
#define AMIGA_EQUALS    0x0c // equals / shifted plus
#define AMIGA_BACKSLASH 0x0d // backslash / shifted pipe
#define AMIGA_SPARE1    0x0e
#define AMIGA_KPZERO    0x0f
#define AMIGA_Q         0x10
#define AMIGA_W         0x11
#define AMIGA_E         0x12
#define AMIGA_R         0x13
#define AMIGA_T         0x14
#define AMIGA_Y         0x15
#define AMIGA_U         0x16
#define AMIGA_I         0x17
#define AMIGA_O         0x18
#define AMIGA_P         0x19
#define AMIGA_OSQPARENS 0x1a // open square parens / shifted open curly parens
#define AMIGA_CSQPARENS 0x1b // close square parents / shifted close curly parens
#define AMIGA_SPARE2    0x1c
#define AMIGA_KPONE     0x1d
#define AMIGA_KPTWO     0x1e
#define AMIGA_KPTHREE   0x1f
#define AMIGA_A         0x20
#define AMIGA_S         0x21
#define AMIGA_D         0x22
#define AMIGA_F         0x23
#define AMIGA_G         0x24
#define AMIGA_H         0x25
#define AMIGA_J         0x26
#define AMIGA_K         0x27
#define AMIGA_L         0x28
#define AMIGA_SEMICOLON 0x29 // semicolon / shifted colon
#define AMIGA_QUOTE     0x2a // quote / shifted doublequote
#define AMIGA_INTLRET   0x2b // international only, return
#define AMIGA_SPARE3    0x2c
#define AMIGA_KPFOUR    0x2d
#define AMIGA_KPFIVE    0x2e
#define AMIGA_KPSIX     0x2f
#define AMIGA_INTLSHIFT 0x30 // international only, left shift
#define AMIGA_Z         0x31
#define AMIGA_X         0x32
#define AMIGA_C         0x33
#define AMIGA_V         0x34
#define AMIGA_B         0x35
#define AMIGA_N         0x36
#define AMIGA_M         0x37
#define AMIGA_COMMA     0x38 // comma / shifted less than
#define AMIGA_PERIOD    0x39 // period / shifted greater than
#define AMIGA_SLASH     0x3a // slash / shifted question mark
#define AMIGA_SPARE7    0x3b
#define AMIGA_KPPERIOD  0x3c
#define AMIGA_KPSEVEN   0x3d
#define AMIGA_KPEIGHT   0x3e
#define AMIGA_KPNINE    0x3f
#define AMIGA_SPACE     0x40
#define AMIGA_BACKSP    0x41
#define AMIGA_TAB       0x42
#define AMIGA_KPENTER   0x43
#define AMIGA_RETURN    0x44
#define AMIGA_ESC       0x45
#define AMIGA_DELETE    0x46
#define AMIGA_SPARE4    0x47
#define AMIGA_SPARE5    0x48
#define AMIGA_SPARE6    0x49
#define AMIGA_KPDASH    0x4a
// 0x4b absent
#define AMIGA_UP        0x4c
#define AMIGA_DOWN      0x4d
#define AMIGA_RIGHT     0x4e
#define AMIGA_LEFT      0x4f
#define AMIGA_F1        0x50
#define AMIGA_F2        0x51
#define AMIGA_F3        0x52
#define AMIGA_F4        0x53
#define AMIGA_F5        0x54
#define AMIGA_F6        0x55
#define AMIGA_F7        0x56
#define AMIGA_F8        0x57
#define AMIGA_F9        0x58
#define AMIGA_F10       0x59
#define AMIGA_KPOPAREN  0x5a // open bracket
#define AMIGA_KPCPAREN  0x5b
#define AMIGA_KPSLASH   0x5c
#define AMIGA_KPAST     0x5d // asterisk abbreviated
#define AMIGA_KPPLUS    0x5e
#define AMIGA_HELP      0x5f
#define AMIGA_LSHIFT    0x60 // modifier
#define AMIGA_RSHIFT    0x61 // modifier
#define AMIGA_CAPSLOCK  0x62 // modifier
#define AMIGA_CTRL      0x63 // modifier
#define AMIGA_LALT      0x64 // modifier
#define AMIGA_RALT      0x65 // modifier
#define AMIGA_LAMIGA    0x66 // modifier
#define AMIGA_RAMIGA    0x67 // modifier

#define AMIGA_UNKNOWN   0xff

static const uint8_t mapHidToAmiga[256] = {
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_A,         AMIGA_B,         AMIGA_C,         AMIGA_D,         // 0x00 (position of first key on line)
    AMIGA_E,         AMIGA_F,         AMIGA_G,         AMIGA_H,         AMIGA_I,         AMIGA_J,         AMIGA_K,         AMIGA_L,         // 0x08
    AMIGA_M,         AMIGA_N,         AMIGA_O,         AMIGA_P,         AMIGA_Q,         AMIGA_R,         AMIGA_S,         AMIGA_T,         // 0x10
    AMIGA_U,         AMIGA_V,         AMIGA_W,         AMIGA_X,         AMIGA_Y,         AMIGA_Z,         AMIGA_ONE,       AMIGA_TWO,       // 0x18
    AMIGA_THREE,     AMIGA_FOUR,      AMIGA_FIVE,      AMIGA_SIX,       AMIGA_SEVEN,     AMIGA_EIGHT,     AMIGA_NINE,      AMIGA_ZERO,      // 0x20
    AMIGA_RETURN,    AMIGA_ESC,       AMIGA_BACKSP,    AMIGA_TAB,       AMIGA_SPACE,     AMIGA_DASH,      AMIGA_EQUALS,    AMIGA_OSQPARENS, // 0x28
    AMIGA_CSQPARENS, AMIGA_BACKSLASH, AMIGA_UNKNOWN,   AMIGA_SEMICOLON, AMIGA_QUOTE,     AMIGA_BACKTICK,  AMIGA_COMMA,     AMIGA_PERIOD,    // 0x30
    AMIGA_SLASH,     AMIGA_CAPSLOCK,  AMIGA_F1,        AMIGA_F2,        AMIGA_F3,        AMIGA_F4,        AMIGA_F5,        AMIGA_F6,        // 0x38
    AMIGA_F7,        AMIGA_F8,        AMIGA_F9,        AMIGA_F10,       AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x40
    AMIGA_UNKNOWN,   AMIGA_HELP,      AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_DELETE,    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_RIGHT,     // 0x48
    AMIGA_LEFT,      AMIGA_DOWN,      AMIGA_UP,        AMIGA_UNKNOWN,   AMIGA_KPSLASH,   AMIGA_KPAST,     AMIGA_KPDASH,    AMIGA_KPPLUS,    // 0x50
    AMIGA_KPENTER,   AMIGA_KPONE,     AMIGA_KPTWO,     AMIGA_KPTHREE,   AMIGA_KPFOUR,    AMIGA_KPFIVE,    AMIGA_KPSIX,     AMIGA_KPSEVEN,   // 0x58
    AMIGA_KPEIGHT,   AMIGA_KPNINE,    AMIGA_KPZERO,    AMIGA_KPPERIOD,  AMIGA_UNKNOWN,   AMIGA_RAMIGA,    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x60
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x68
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x70
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x78
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x80
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x88
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x90
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0x98
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xa0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xa8
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xb0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xb8
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xc0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xc8
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xd0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xd8
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xe0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xe8
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   // 0xf0
    AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN,   AMIGA_UNKNOWN    // 0xf8
};

#endif
