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

static uint8_t keyboard_buffer[6];
static uint8_t timing_delay;

void update_mouse_inputs (){
  // the normal mouse access via the dock
  uint16_t speed = (AFP_REGISTOR(5) & 0xffff);
  if (((CONTROLLER_KEY_REG(4)>>28) == 0x5) && (mouse_counter != (CONTROLLER_KEY_REG(4) & 0x0000FFFF))){
    signed short x = (short)((CONTROLLER_JOY_REG(4) & 0x0000FFFF))>>speed;
    signed short y = (short)((CONTROLLER_TRIG_REG(4) & 0x0000FFFF))>>speed;
    x_count = x_count + ((x < -127) ? -127 : (x > 127) ? 127 : x);
    y_count = y_count + ((y < -127) ? -127 : (y > 127) ? 127 :y);
    HPS_spi_uio_cmd8(UIO_MOUSE_X, x_count);
    HPS_spi_uio_cmd8(UIO_MOUSE_Y, y_count);
    HPS_spi_uio_cmd8(UIO_MOUSE_BTN, ((CONTROLLER_JOY_REG(4)>>16) & 0x7));
    mouse_counter = (CONTROLLER_KEY_REG(4) & 0x0000FFFF);
  } else if (!((CONTROLLER_KEY_REG(4)>>28) == 0x5)  && (((CONTROLLER_KEY_REG(1)>>8) & 0x3) == 0x3)){
    // Emulating the mouse on the dpad by pressing both left and right triggers
    int tmp_joy = CONTROLLER_KEY_REG(1);
    if ((tmp_joy & 0x1) == 1) y_count = y_count - speed;
    if (((tmp_joy>>1) & 0x1) == 1) y_count = y_count + speed;
    if (((tmp_joy>>2) & 0x1) == 1) x_count = x_count - speed;
    if (((tmp_joy>>3) & 0x1) == 1) x_count = x_count + speed;
    HPS_spi_uio_cmd8(UIO_MOUSE_X, x_count);
    HPS_spi_uio_cmd8(UIO_MOUSE_Y, y_count);
    HPS_spi_uio_cmd8(UIO_MOUSE_BTN, ((CONTROLLER_KEY_REG(1)>>4) & 0x7));
  }
  return;
};

void update_keyboard_inputs (uint8_t number){
  printf("%0.4x %0.4x %0.4x \r\n", CONTROLLER_KEY_REG(3), CONTROLLER_TRIG_REG(3), CONTROLLER_JOY_REG(3));



  return;
};

// THis is the placeholder for the IO updates and makes sure we are polling every 50 milliseconds
void minimig_input_update() {
  if (CheckTimer1(10)){
    timing_delay++;
    if (((CONTROLLER_KEY_REG(3)>>28) == 0x4))update_keyboard_inputs(timing_delay); // We want to update the keyboard 6 time every
    if (timing_delay == 5){
      timing_delay = 0;
      if ((((AFP_REGISTOR(5)>>24)) == 0x80) | ((AFP_REGISTOR(5)>>24) == 0xa0)) {
        update_mouse_inputs();
      }
      ResetTimer1();
    }
  }
};

void minimig_joystick_reg_update(){
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , (AFP_REGISTOR(5)>>24));
}
