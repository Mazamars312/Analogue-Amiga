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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "minimig_inputs.h"

unsigned int mouse_counter={0};
signed short x_count = 0;
signed short y_count = 0;

void minimig_input_update() {

  if (((CONTROLLER_KEY_REG(4)>>28) == 0x5) && CheckTimer1(10) && (mouse_counter != (CONTROLLER_KEY_REG(4) & 0x0000FFFF))){
    signed short x = (short)((CONTROLLER_JOY_REG(4) & 0x0000FFFF))>>8;
    signed short y = (short)((CONTROLLER_TRIG_REG(4) & 0x0000FFFF))>>8;
    x_count = x_count + ((x < -127) ? -127 : (x > 127) ? 127 : x);
    y_count = y_count + ((y < -127) ? -127 : (y > 127) ? 127 :y);
    HPS_spi_uio_cmd8(UIO_MOUSE_X, x_count);
    HPS_spi_uio_cmd8(UIO_MOUSE_Y, y_count);
    ResetTimer1();
    // printf("x_count %0.4x y_count %0.4x\r\n",x_count,y_count);
    mouse_counter = (CONTROLLER_KEY_REG(4) & 0x0000FFFF);
  } else if (((CONTROLLER_KEY_REG(4)>>28) == 0x5) && CheckTimer1(50)){
    ResetTimer1();
  }
}
