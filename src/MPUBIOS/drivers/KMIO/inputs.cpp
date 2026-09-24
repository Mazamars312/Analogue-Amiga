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




#include "inputs.h"
#include "mouse_math.h"

int interupt_amount_timer = 0;
unsigned int mouse_counter={0};
signed short x_count = 0;
signed short y_count = 0;
bool mouse_enabled = 0;
bool mouse_enabled_press = 0;
bool mouse_enabled_press_old = 0;

uint8_t keyboard_buffer[6];
bool    MOD_LALT;
bool    MOD_RALT;
bool    MOD_LSHIFT;
bool    MOD_RSHIFT;
bool    MOD_LWIN;
bool    MOD_RWIN;
bool    MOD_CTRL;
bool    MOD_CAPS_LOCK;
bool    keyboard_osd_showing = 0;
// static uint8_t timing_delay;

void input_allow_osd(bool keyboard_active){
  keyboard_osd_showing = keyboard_active;
  mainprintf ("Active %d\r\n", keyboard_osd_showing);
}

void update_mouse_inputs (){
  mouse_enabled_press = ((CONTROLLER_KEY_REG(1) >> 15) & 0x1) ; //|| ( ((CONTROLLER_KEY_REG(1)>>28) == 0x3) && ((CONTROLLER_KEY_REG(1) >> 15) & 0x1))
  if (keyboard_osd_showing == 1) {
    mouse_enabled = 0;
  } else if (mouse_enabled_press && !mouse_enabled_press_old) {
    mouse_enabled = !mouse_enabled;
    
    OsdUpdate(0);
    OsdClear();
    if (mouse_enabled){
      uint32_t tmp = CORE_OUTPUT_REGISTOR();
      tmp = tmp | 0x00000030;
      CORE_OUTPUT_REGISTOR() = tmp;
        OsdWrite("Mouse Disabled");
        OsdUpdate(0);
        OsdClear();
        
    } else {
      uint32_t tmp = CORE_OUTPUT_REGISTOR();
      tmp = tmp & 0xFFFFFFCF;
      CORE_OUTPUT_REGISTOR() = tmp;
      OsdWrite("Mouse Enabled");
    }
    
    OsdUpdate(1);
    OsdClear();
    OsdUpdate(2);
    OsdClear();
  } 
  mouse_enabled_press_old = mouse_enabled_press;
  if (keyboard_osd_showing == 0){
    if (mouse_enabled_press){
      InfoEnable(10, 10, 32, 3, 291, 31744); // Had to use the info as some of the PCE outputs are smaller
    } else {
      OsdDisable();
    }
  }
  uint16_t speed = (AFP_REGISTOR(5) & 0xffff);
  if (speed == 0) speed = 1; // avoid divide-by-zero before settings load

  static uint8_t dock_btn = 0;                     // persists across idle polls -- fixes buttons pulsing for one 2ms poll per report
  static uint8_t last_btn_sent = 0xFF;              // force the first send
  static int32_t rem_dock_x = 0, rem_dock_y = 0;    // remainder carry, dock mouse (/MOUSE_DOCK_DIV)
  static int32_t rem_stick_x = 0, rem_stick_y = 0;  // remainder carry, analog stick (/MOUSE_STICK_DIV)
  static int32_t pending_x = 0, pending_y = 0;      // shared per-poll-capped output queue

  bool dock_mouse = ((CONTROLLER_KEY_REG(4)>>28) == 0x5);

  if (dock_mouse) {
    // Consistent read: the bridge writes counter, then X, then Y as three
    // separate registers, so a poll can land mid-update and pair a new
    // counter with stale X/Y (or vice versa). Retry until two back-to-back
    // reads agree; if they never do, skip this poll entirely rather than
    // act on an unverified (possibly torn) combination -- we'll try again
    // next poll. (A previous version fell through and used the last,
    // unverified read anyway, which could pair a fresh counter with a
    // stale delta or vice versa -- most likely during fast movement, when
    // the registers are changing quickly enough that back-to-back reads
    // rarely agree, matching "fast moves jump to random places, slow moves
    // track fine.")
    uint32_t ctr = CONTROLLER_KEY_REG(4), joy = CONTROLLER_JOY_REG(4), trig = CONTROLLER_TRIG_REG(4);
    bool consistent = false;
    for (uint8_t tries = 0; tries < 4; tries++) {
      uint32_t ctr2 = CONTROLLER_KEY_REG(4), joy2 = CONTROLLER_JOY_REG(4), trig2 = CONTROLLER_TRIG_REG(4);
      if (ctr2 == ctr && joy2 == joy && trig2 == trig) { consistent = true; break; }
      ctr = ctr2; joy = joy2; trig = trig2;
    }

    uint16_t new_ctr = ctr & 0x0000FFFF;
    if (consistent && mouse_counter != new_ctr) {
      dock_btn = (joy >> 16) & 0x7;

      int8_t dx = mouse_decode_delta(joy);
      int8_t dy = mouse_decode_delta(trig);

      int32_t step_x = mouse_scale_step(&rem_dock_x, dx, speed, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
      int32_t step_y = mouse_scale_step(&rem_dock_y, dy, speed, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);

      mouse_enqueue(&pending_x, step_x);
      mouse_enqueue(&pending_y, step_y);
      mouse_counter = new_ctr;
    }

  } else if (((CONTROLLER_KEY_REG(1)>>28) == 0x3)){
    // Analog-stick mouse emulation via the dock. Both halves of the
    // original left/right and up/down split wanted the same signed
    // (x-0x80)/(y-0x80) distance from center; computed as unsigned
    // 0x80-x / x-0x80 it wrapped to a huge value on whichever side wasn't
    // the one the subtraction was written for, corrupting x_count/y_count.
    dock_btn = 0;
    signed short x = (signed short)(CONTROLLER_JOY_REG(1) & 0x000000FF);
    signed short y = (signed short)((CONTROLLER_JOY_REG(1)>>8) & 0x000000FF);

    int16_t dx = 0, dy = 0;
    if (x < 0x70 || x > 0x90) dx = (int16_t)(x - 0x80);
    if (y < 0x70 || y > 0x90) dy = (int16_t)(y - 0x80);

    rem_stick_x += (int32_t)dx * speed;
    rem_stick_y += (int32_t)dy * speed;
    int32_t step_x = rem_stick_x / MOUSE_STICK_DIV;
    int32_t step_y = rem_stick_y / MOUSE_STICK_DIV;
    rem_stick_x -= step_x * MOUSE_STICK_DIV;
    rem_stick_y -= step_y * MOUSE_STICK_DIV;

    mouse_enqueue(&pending_x, step_x);
    mouse_enqueue(&pending_y, step_y);

  } else if (mouse_enabled) {
    // Emulating the mouse on the dpad by pressing both left and right triggers
    dock_btn = 0;
    uint32_t tmp = CORE_OUTPUT_REGISTOR();
    tmp = tmp | 0x00000030;
    CORE_OUTPUT_REGISTOR() = tmp;
    int tmp_joy = CONTROLLER_KEY_REG(1);
    int32_t dx = 0, dy = 0;
    if ((tmp_joy & 0x1) == 1)       dy -= speed;
    if (((tmp_joy>>1) & 0x1) == 1)  dy += speed;
    if (((tmp_joy>>2) & 0x1) == 1)  dx -= speed;
    if (((tmp_joy>>3) & 0x1) == 1)  dx += speed;
    mouse_enqueue(&pending_x, dx);
    mouse_enqueue(&pending_y, dy);

  } else {
    dock_btn = 0;
    uint32_t tmp = CORE_OUTPUT_REGISTOR();
    tmp = tmp & 0xFFFFFFCF;
    CORE_OUTPUT_REGISTOR() = tmp;
  }

  // Always send X then Y (a lone Y write is read as a scroll-wheel delta
  // instead), draining whatever the active source queued this poll.
  x_count = x_count + (signed short)mouse_drain(&pending_x);
  y_count = y_count + (signed short)mouse_drain(&pending_y);
  HPS_spi_uio_cmd8(UIO_MOUSE_X, x_count);
  HPS_spi_uio_cmd8(UIO_MOUSE_Y, y_count);

  // Pad A/B always act as mouse buttons too (e.g. hold a drag with the pad
  // while the dock mouse keeps moving); send once per poll, only on change,
  // so a held dock button doesn't pulse for a single 2ms report.
  uint8_t pad_btn = (CONTROLLER_KEY_REG(1) >> 8) & 0x3;
  uint8_t btn = dock_btn | pad_btn;
  if (btn != last_btn_sent) {
    HPS_spi_uio_cmd8(UIO_MOUSE_BTN, btn);
    last_btn_sent = btn;
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
      if (tmp != AMIGA_UNKNOWN) HPS_spi_uio_cmd8(UIO_KEYBOARD, tmp); // we send the new keys to the HPS bus
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
      if (tmp != AMIGA_UNKNOWN) HPS_spi_uio_cmd8(UIO_KEYBOARD, 0x80 | tmp);
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
  bool    MOD_RWIN_TMP    = ((CONTROLLER_KEY_REG(3)>>15) & 0x01);
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
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_RALT);
  } else if (!MOD_RALT_TMP && MOD_RALT){
    MOD_RALT = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_RALT| 0x80));
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
  } else if (!(MOD_LCTRL_TMP | MOD_RCTRL_TMP) && MOD_CTRL){
    MOD_CTRL = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_CTRL| 0x80));
  }
  // Right Winkey (GUI) Key -- bit 15, same "meaningful byte on top" layout
  // as the other seven modifier bits, just never read.
  if (MOD_RWIN_TMP && !MOD_RWIN) {
    MOD_RWIN = 1;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, AMIGA_RAMIGA);
  } else if (!MOD_RWIN_TMP && MOD_RWIN){
    MOD_RWIN = 0;
    HPS_spi_uio_cmd8(UIO_KEYBOARD, (AMIGA_RAMIGA| 0x80));
  }
  return;
}

// THis is the placeholder for the IO updates and makes sure we are polling every 50 milliseconds
void input_update() {
    timer1Interrupts_reset();
    if (((CONTROLLER_KEY_REG(3)>>28) == 0x4)) update_keyboard_inputs(); // We want to update the keyboard 6 time every
    if ((((AFP_REGISTOR(5)>>24)) == 0x80) | ((AFP_REGISTOR(5)>>24) == 0xa0)) {
      update_mouse_inputs();
    } 
    return;
  }

void input_setup() {
    timer1Interrupts_setup(2);
  }

void input_reg_update(){
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , (AFP_REGISTOR(5)>>28));
}
