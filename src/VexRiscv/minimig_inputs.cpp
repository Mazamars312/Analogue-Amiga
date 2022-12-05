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

uint8_t keyboard_buffer[6];
bool    MOD_LALT;
bool    MOD_RALT;
bool    MOD_LSHIFT;
bool    MOD_RSHIFT;
bool    MOD_LWIN;
bool    MOD_CTRL;
bool    MOD_CAPS_LOCK;
// static uint8_t timing_delay;

void update_mouse_inputs (){

  uint16_t speed = (AFP_REGISTOR(5) & 0xffff);
  if (((CONTROLLER_KEY_REG(4)>>28) == 0x5) && (mouse_counter != (CONTROLLER_KEY_REG(4) & 0x0000FFFF))){
    // the normal mouse access via the dock
    // Right now this is just a shifter Will change this to something more accurate as this causes an issue with the emulated mouse
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

void update_keyboard_inputs (){
  // printf("%0.4x %0.4x %0.4x \r\n", CONTROLLER_KEY_REG(3), CONTROLLER_TRIG_REG(3), CONTROLLER_JOY_REG(3));
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t old_buf_len = 6;
  uint8_t tmp;

  uint8_t keyboard_buffer_updated[6]={0};
  uint8_t keyboard_buffer_input[6]={0};
  bool failed_key;

  for (i = 0; i < old_buf_len; i++) {
    keyboard_buffer_updated[i] = 0;
    keyboard_buffer_input[i] = 0;
  };
  // For we check for pressed down and currently pressed keys
  do {
  keyboard_buffer_input[0] = ((CONTROLLER_JOY_REG(3)>>24) & 0xff);
  keyboard_buffer_input[1] = ((CONTROLLER_JOY_REG(3)>>16) & 0xff);
  keyboard_buffer_input[2] = ((CONTROLLER_JOY_REG(3)>>8) & 0xff);
  keyboard_buffer_input[3] = (CONTROLLER_JOY_REG(3) & 0xff);
  keyboard_buffer_input[4] = ((CONTROLLER_TRIG_REG(3)>>8) & 0xff);
  keyboard_buffer_input[5] = (CONTROLLER_TRIG_REG(3) & 0xff);
    // check if a key is new to the buffer or current
    i = 0;
    do {
      failed_key = true;

      if ((keyboard_buffer_input[j] == keyboard_buffer[i]) && (keyboard_buffer_input[j] != 0)) {
        failed_key = false;
        break;
      } else if (keyboard_buffer_input[j] == 0){
        failed_key = false;
        break;
      }
      i++;
    } while (i < old_buf_len);
    keyboard_buffer_updated[j] = keyboard_buffer_input[j];  // Store the Pressed keys and the current keys down still to a temp buffer for now.
    if (failed_key) {
      tmp = mapHidToAmiga[keyboard_buffer_input[j]]; // Convert to the Amiga standard
      HPS_spi_uio_cmd8(UIO_KEYBOARD, tmp); // we send the new keys to the HPS bus
    }

    j++;
  } while (j < old_buf_len);

  // Now we test what keys have been un-pressed
  j = 0;
  do {
    i = 0;
    do {
      failed_key = true;
      // check if a key is removed
      if ((keyboard_buffer[j] == keyboard_buffer_updated[i]) && (keyboard_buffer[j] != 0)) {
        failed_key = false;
        break;
      } else if (keyboard_buffer[j] == 0){
        failed_key = false;
        break;
      }
      i++;
    } while (i < old_buf_len);
    if (failed_key) {
      tmp = mapHidToAmiga[keyboard_buffer[j]]; // To unpress we send the MSB to 1 and the same code back to say unpressed
      tmp = 0x80 | tmp;
      HPS_spi_uio_cmd8(UIO_KEYBOARD, tmp);
    }

    j++;
  } while (j < old_buf_len);
  for (i = 0; i < old_buf_len; i++) keyboard_buffer[i] = keyboard_buffer_updated[i];
  // Now for the controll keys
  bool    MOD_LALT_TMP    = ((CONTROLLER_KEY_REG(3)>>10) & 0x01);
  bool    MOD_RALT_TMP    = ((CONTROLLER_KEY_REG(3)>>14) & 0x01);
  bool    MOD_LSHIFT_TMP  = ((CONTROLLER_KEY_REG(3)>>9) & 0x01);
  bool    MOD_RSHIFT_TMP  = ((CONTROLLER_KEY_REG(3)>>13) & 0x01);
  bool    MOD_LWIN_TMP    = ((CONTROLLER_KEY_REG(3)>>11) & 0x01);
  bool    MOD_LCTRL_TMP   = ((CONTROLLER_KEY_REG(3)>>8)  & 0x01);
  bool    MOD_RCTRL_TMP   = ((CONTROLLER_KEY_REG(3)>>12) & 0x01);
  // Left Alt Key
  if (MOD_LALT_TMP && !MOD_LALT) {
    MOD_LALT = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_LALT);
  } else if (!MOD_LALT_TMP && MOD_LALT){
    MOD_LALT = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_LALT| 0x80));
  }
// Right  Alt Key
  if (MOD_RALT_TMP && !MOD_RALT) {
    MOD_RALT = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_RSHIFT);
  } else if (!MOD_RALT_TMP && MOD_RALT){
    MOD_RALT = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_RSHIFT| 0x80));
  }
  // Left Shift Key
  if (MOD_LSHIFT_TMP && !MOD_LSHIFT) {
    MOD_LSHIFT = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_LSHIFT);
  } else if (!MOD_LSHIFT_TMP && MOD_LSHIFT){
    MOD_LSHIFT = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_LSHIFT| 0x80));
  }
  // Right Shift Key
  if (MOD_RSHIFT_TMP && !MOD_RSHIFT) {
    MOD_RSHIFT = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_RSHIFT);
  } else if (!MOD_RSHIFT_TMP && MOD_RSHIFT){
    MOD_RSHIFT = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_RSHIFT| 0x80));
  }
  // Left Winkey Key
  if (MOD_LWIN_TMP && !MOD_LWIN) {
    MOD_LWIN = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_LAMIGA);
  } else if (!MOD_LWIN_TMP && MOD_LWIN){
    MOD_LWIN = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_LAMIGA| 0x80));
  }
  // Control Key, But this does both of them on the keybopard
  if ((MOD_LCTRL_TMP | MOD_RCTRL_TMP) && !MOD_CTRL) {
    MOD_CTRL = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_CTRL);
  } else if (!MOD_LWIN_TMP && MOD_CTRL){
    MOD_CTRL = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_CTRL| 0x80));
  }
  return;
}

// THis is the placeholder for the IO updates and makes sure we are polling every 50 milliseconds
void minimig_input_update() {
    // int tmp = HW_TIMERCHECKBASE(0);
    // printf("%d\r\n", tmp);
    ResetTimer();
    if (((CONTROLLER_KEY_REG(3)>>28) == 0x4)) update_keyboard_inputs(); // We want to update the keyboard 6 time every
    if ((((AFP_REGISTOR(5)>>24)) == 0x80) | ((AFP_REGISTOR(5)>>24) == 0xa0)) update_mouse_inputs();
    minimig_input_setup();
  }

  void minimig_input_setup() {
      Set_interrupt_Timer(10);
    }

void minimig_joystick_reg_update(){
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , (AFP_REGISTOR(5)>>24));
}
