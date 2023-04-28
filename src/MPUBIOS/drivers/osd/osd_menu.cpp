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

#include "osd_menu.h"

static uint32_t timer_wait_hold;
static uint32_t timer_wait_value;
char s[64];

uint8_t nstore1;
uint8_t nstore2;
uint8_t nstore3;
uint8_t frame_counter = 0;


void osd_display_error(int data_error){
  error_system = 1;
  OsdWrite("------------NO FILES-------E00");
  OsdUpdate(0);
  OsdClear();
  OsdWrite("     Please check the JSON    ");
  OsdUpdate(1);
  OsdClear();
  OsdWrite("    Files could be found in   ");
  OsdUpdate(2);
  OsdClear();
  OsdWrite("        the APF Search.       ");
  OsdUpdate(3);
  OsdClear();
  OsdWrite(" Check Directory Locations in ");
  OsdUpdate(4);
  OsdClear();
  OsdWrite("        The JSON File 			");
  OsdUpdate(5);
  OsdClear();
  OsdWrite("   Also check file names are  ");
  OsdUpdate(6);
  OsdClear();
  OsdWrite("     below 100 Charractors    ");
  OsdUpdate(7);
  OsdClear();
  sprintf (s, "         dataslot %d        ", data_error);
  OsdWrite(s);
  OsdUpdate(8);
  OsdClear();

  OsdWrite("------------------------------");
  OsdUpdate(9);
  OsdClear();
  InfoEnable(5, 80, 30, 10, 192, 65535); // Had to use the info as some of the PCE outputs are smaller
  frame_counter = 20;
}

void osd_display_error_apf(){

};

void osd_display_info(){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){

    OsdClear();
    sprintf (s, "T M:S : %d %d %d", nstore1,  nstore2 , nstore3);
    OsdWrite(s);
    OsdUpdate(0);
    OsdClear();
    sprintf (s, "Delay: %d", timer_wait_value);
    OsdWrite(s);
    OsdUpdate(1);
    OsdClear();
    OsdUpdate(2);
    OsdUpdate(3);
    OsdUpdate(4);

    if (frame_counter != 0) {
      InfoEnable(10, 10, 16, 2, 291, 31744); // Had to use the info as some of the PCE outputs are smaller
      ResetTimer2();
      timer_wait_hold  = RISCGetTimer2(0, 148500000);
      frame_counter = frame_counter - 1;
    }
    else {
      InfoEnable(10, 10, 16, 2, 291, 65535); // Had to use the info as some of the PCE outputs are smaller
      timer_wait_value = 0;
    }
  }
  else if (error_system != 0){

  }else {
    OsdDisable();
  }
}

void osd_display_info_update(int timer_wait){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){
      timer_wait_value = timer_wait_value + timer_wait;
      if (timer_wait > 0) frame_counter = 2;
  }
}

void osd_display_timing (uint8_t n1, uint8_t n2, uint8_t n3){
  if ((error_system == 0) && (AFP_REGISTOR(2) == 1)){
    nstore1 = n1;
    nstore2 = n2;
    nstore3 = n3;
  }
}

void osd_display_error_dataslot (int data_error){
  error_system = 1;
  // switch(data_error)
  // {
  // case 4:
  sprintf (s, "-------Dataslot Error----%d", data_error);
  OsdWrite(s);
  OsdUpdate(0);
  OsdClear();
  OsdWrite("     Please check the file   ");
  OsdUpdate(1);
  OsdClear();
  OsdWrite("     size or the cue times   ");
  OsdUpdate(2);
  OsdClear();
  OsdWrite("        the APF Search.       ");
  OsdUpdate(3);
  OsdClear();
  OsdWrite(" Check Directory Locations in ");
  OsdUpdate(4);
  OsdClear();
  OsdWrite("        The JSON File 			");
  OsdUpdate(5);
  OsdClear();
  OsdWrite("   Also check file names are  ");
  OsdUpdate(6);
  OsdClear();
  OsdWrite("     below 100 Charractors    ");
  OsdUpdate(7);
  OsdClear();
  sprintf (s, "         dataslot %d        ", data_error);
  OsdWrite(s);
  OsdUpdate(8);
  OsdClear();

  OsdWrite("------------------------------");
  OsdUpdate(9);
  OsdClear();
  InfoEnable(5, 80, 30, 10, 192, 65535); // Had to use the info as some of the PCE outputs are smaller

}
