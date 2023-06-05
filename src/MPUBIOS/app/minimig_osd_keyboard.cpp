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
#include "minimig_osd_keyboard.h"

/*
    So This will be simple on how to draw the keyboard
    Each line gets enabled on the y number to a bool
    then the x gets send to each line and a test is done
    to invert the selected item

    Might look at a timer to flash the invert........
    Na, lets just get it working first. Im already at 64K of memory now.

*/


void OSD_Keyboard_1_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "E", active);
    OsdWriteDirect(i++, "S", active);
    OsdWriteDirect(i++, "C", active);
    active = 0;

    OsdWriteDirect(i++, " ");

    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "1", active);
    active = 0;
    if (enable && num == 2) active = 1;

    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "2", active);
    active = 0;
    if (enable && num == 3) active = 1;

    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "3", active);
    active = 0;
    if (enable && num == 4) active = 1;

    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "4", active);
    active = 0;
    if (enable && num == 5) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "5", active);
    active = 0;
    if (enable && num == 6) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "6", active);
    active = 0;
    if (enable && num == 7) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "7", active);
    active = 0;
    if (enable && num == 8) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "8", active);
    active = 0;
    if (enable && num == 9) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "9", active);
    active = 0;
    if (enable && num == 10) active = 1;
    
    OsdWriteDirect(i++, "F", active);
    OsdWriteDirect(i++, "1", active);
    OsdWriteDirect(i++, "0", active);
    active = 0;
    if (enable && num == 11) active = 1;

    OsdWriteDirect(i++, " ");
    
    if (enable && num == 12) active = 1;
    OsdWriteDirect(i++, "H", active);
    OsdWriteDirect(i++, "E", active);
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "P", active);
    active = 0;

    OsdUpdate(1);   
}

void OSD_Keyboard_2_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "~", active);
    active = 0;
    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "1", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 2) active = 1;
    OsdWriteDirect(i++, "2", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 3) active = 1;
    OsdWriteDirect(i++, "3", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 4) active = 1;
    OsdWriteDirect(i++, "4", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 5) active = 1;
    OsdWriteDirect(i++, "5", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 6) active = 1;
    OsdWriteDirect(i++, "6", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 7) active = 1;
    OsdWriteDirect(i++, "7", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 8) active = 1;
    OsdWriteDirect(i++, "8", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 9) active = 1;
    OsdWriteDirect(i++, "9", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 10) active = 1;
    OsdWriteDirect(i++, "0", active);
    active = 0;
    if (enable && num == 11) active = 1;
    OsdWriteDirect(i++, "-", active);
    active = 0;
    if (enable && num == 12) active = 1;
    OsdWriteDirect(i++, "+", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 13) active = 1;
    OsdWriteDirect(i++, "B", active);
    OsdWriteDirect(i++, "S", active);
    OsdWriteDirect(i++, "P", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 14) active = 1;
    OsdWriteDirect(i++, "D", active);
    OsdWriteDirect(i++, "E", active);
    OsdWriteDirect(i++, "L", active);
    active = 0;

    OsdUpdate(2);   
}

void OSD_Keyboard_3_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "T", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "B", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "Q", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 2) active = 1;
    OsdWriteDirect(i++, "W", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 3) active = 1;
    OsdWriteDirect(i++, "E", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 4) active = 1;
    OsdWriteDirect(i++, "R", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 5) active = 1;
    OsdWriteDirect(i++, "T", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 6) active = 1;
    OsdWriteDirect(i++, "Y", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 7) active = 1;
    OsdWriteDirect(i++, "U", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 8) active = 1;
    OsdWriteDirect(i++, "I", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 9) active = 1;
    OsdWriteDirect(i++, "O", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 10) active = 1;
    OsdWriteDirect(i++, "P", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 11) active = 1;
    OsdWriteDirect(i++, "{", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 12) active = 1;
    OsdWriteDirect(i++, "}", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 13) active = 1;
    OsdWriteDirect(i++, "|", active);
    active = 0;
    OsdWriteDirect(i++, " ");

    OsdUpdate(3);   
}

void OSD_Keyboard_4_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "C", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "P", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "A", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 2) active = 1;
    OsdWriteDirect(i++, "S", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 3) active = 1;
    OsdWriteDirect(i++, "D", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 4) active = 1;
    OsdWriteDirect(i++, "F", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 5) active = 1;
    OsdWriteDirect(i++, "G", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 6) active = 1;
    OsdWriteDirect(i++, "H", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 7) active = 1;
    OsdWriteDirect(i++, "J", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 8) active = 1;
    OsdWriteDirect(i++, "K", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 9) active = 1;
    OsdWriteDirect(i++, "L", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 10) active = 1;
    OsdWriteDirect(i++, ":", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 11) active = 1;
    OsdWriteDirect(i++, "'", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 12) active = 1;
    OsdWriteDirect(i++, "R", active);
    OsdWriteDirect(i++, "E", active);
    OsdWriteDirect(i++, "T", active);
    active = 0;
    OsdWriteDirect(i++, " ");

    OsdUpdate(4);   
}

void OSD_Keyboard_5_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "S", active);
    OsdWriteDirect(i++, "F", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "Z", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 2) active = 1;
    OsdWriteDirect(i++, "X", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 3) active = 1;
    OsdWriteDirect(i++, "C", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 4) active = 1;
    OsdWriteDirect(i++, "V", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 5) active = 1;
    OsdWriteDirect(i++, "B", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 6) active = 1;
    OsdWriteDirect(i++, "N", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 7) active = 1;
    OsdWriteDirect(i++, "M", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 8) active = 1;
    OsdWriteDirect(i++, ",", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 9) active = 1;
    OsdWriteDirect(i++, ".", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 10) active = 1;
    OsdWriteDirect(i++, "?", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 11) active = 1;
    OsdWriteDirect(i++, "R", active);
    OsdWriteDirect(i++, "S", active);
    OsdWriteDirect(i++, "F", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    OsdWriteDirect(i++, " ");
    if (enable && num == 12) active = 1;
    OsdWriteDirect(i++, "\x12", active);
    active = 0;
    OsdWriteDirect(i++, " ");

    OsdUpdate(5);   
}

void OSD_Keyboard_6_line(bool enable, int num){
    uint16_t i = 0;
    bool active = 0;
    OsdClear();
    if (enable && num == 0) active = 1;
    OsdWriteDirect(i++, "C", active);
    OsdWriteDirect(i++, "T", active);
    OsdWriteDirect(i++, "R", active);
    OsdWriteDirect(i++, "L", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 1) active = 1;
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "T", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 2) active = 1;
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "M", active);
    active = 0;
    if (enable && num == 3) active = 1;
    OsdWriteDirect(i++, " ", active);
    OsdWriteDirect(i++, "S", active);
    OsdWriteDirect(i++, "P", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "C", active);
    OsdWriteDirect(i++, "E", active);
    OsdWriteDirect(i++, " ", active);
    active = 0;
    if (enable && num == 4) active = 1;
    OsdWriteDirect(i++, "R", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "M", active);
    active = 0;
    OsdWriteDirect(i++, " ");
    if (enable && num == 5) active = 1;
    OsdWriteDirect(i++, "R", active);
    OsdWriteDirect(i++, "A", active);
    OsdWriteDirect(i++, "L", active);
    OsdWriteDirect(i++, "T", active);
    active = 0;
    if (enable && num == 6) active = 1;
    OsdWriteDirect(i++, "\x10", active);
    active = 0;
    if (enable && num == 7) active = 1;
    OsdWriteDirect(i++, "\x13", active);
    active = 0;
    if (enable && num == 8) active = 1;
    OsdWriteDirect(i++, "\x11", active);
    active = 0;
        // down Arrow is (const char *)0x4200
    OsdUpdate(6);   
}

// Here is the process of the menu for the keyboard
// I want to keep these values in the function and store them


int x = 14;
int y = 1;
bool keyboard_toggle_button_pressed = 0;
bool keyboard_toggle_up_button_pressed = 0;
bool keyboard_toggle_down_button_pressed = 0;
bool keyboard_toggle_left_button_pressed = 0;
bool keyboard_toggle_right_button_pressed = 0;
bool keyboard_toggle_a_button_pressed = 0;
bool keyboard_a_pressed = 0;

bool keyboard_is_showing;


void OSD_Keyboard_process(){
    bool updated_keyboard = 0;
    bool keyboard_up_pressed = 0;
    bool keyboard_down_pressed = 0;
    bool keyboard_left_pressed = 0;
    bool keyboard_right_pressed = 0;
    // toggle of the keyboard menu
    // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00004000) == 0x00004000) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_toggle_button_pressed == 0){
        if (keyboard_is_showing == 0) {
            keyboard_is_showing = 1;
            updated_keyboard = 1;
            input_allow_osd(1);
        }
        else {
            keyboard_is_showing = 0;
            input_allow_osd(0);
        }
    }
    // this is a debouncer for this button
    if (((CONTROLLER_KEY_REG(1) & 0x00004000) == 0x00004000) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4){
        keyboard_toggle_button_pressed = 1;
    } else {
        keyboard_toggle_button_pressed = 0;
    }

    // toggle of the up button
    // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00000001) == 0x00000001) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_toggle_up_button_pressed == 0){
        if (keyboard_is_showing == 1) {
            keyboard_up_pressed = 1;
            updated_keyboard = 1;
        }
        else {
            keyboard_up_pressed = 0;
            
        }
    }

    if (((CONTROLLER_KEY_REG(1) & 0x00000001) == 0x00000001) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4){
        keyboard_toggle_up_button_pressed = 1;
    } else {
        keyboard_toggle_up_button_pressed = 0;
    }

    // toggle of the Down button
    // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00000002) == 0x00000002) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_toggle_down_button_pressed == 0){
        if (keyboard_is_showing == 1) {
            keyboard_down_pressed = 1;
            updated_keyboard = 1;
        }
        else {
            keyboard_down_pressed = 0;
            
        }
    }

    if (((CONTROLLER_KEY_REG(1) & 0x00000002) == 0x00000002) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4){
        keyboard_toggle_down_button_pressed = 1;
    } else {
        keyboard_toggle_down_button_pressed = 0;
    }

    // toggle of the left button
    // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00000004) == 0x00000004) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_toggle_left_button_pressed == 0){
        if (keyboard_is_showing == 1) {
            keyboard_left_pressed = 1;
            updated_keyboard = 1;
        }
        else {
            keyboard_left_pressed = 0;
            
        }
    }

    if (((CONTROLLER_KEY_REG(1) & 0x00000004) == 0x00000004) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4){
        keyboard_toggle_left_button_pressed = 1;
    } else {
        keyboard_toggle_left_button_pressed = 0;
    }

    // toggle of the right button
    // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00000008) == 0x00000008) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_toggle_right_button_pressed == 0){
        if (keyboard_is_showing == 1) {
            keyboard_right_pressed = 1;
            updated_keyboard = 1;
        }
        else {
            keyboard_right_pressed = 0;
            
        }
    } else {
        keyboard_right_pressed = 0;
        
    }   

    if (((CONTROLLER_KEY_REG(1) & 0x00000008) == 0x00000008) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4){
        keyboard_toggle_right_button_pressed = 1;
    } else {
        keyboard_toggle_right_button_pressed = 0;
    }

  // toggle of the a button
  // A simple debouncer and toggle system
    if (((CONTROLLER_KEY_REG(1) & 0x00000010) == 0x00000010) && (CONTROLLER_KEY_REG(4) >> 28) != 0x4 && keyboard_is_showing == 1){
        keyboard_a_pressed = 1;
    } else {
        keyboard_a_pressed = 0;
    }

    

    /**********************************************************************
     *  There is some custom movement we need to do on the x and y locations
     *  Then from there I need to look up the array and send that to the core
     *  Seams simple.......
     *  Way toooo much custom code tho
     * 
    *****************************************************************************/

    int old_x = x;
    int old_y = y;
    // first we test the up and down
    if (keyboard_up_pressed){
        if (y > 0) y = y - 1;
        else y = 0;
        if (y == 0 && old_y == 1){
            if      (x == 0) x = 0;
            else if (x == 1) x = 0;
            else if (x == 2) x = 0; 
            else if (x == 3) x = 1;
            else if (x == 4) x = 2;
            else if (x == 5) x = 3; 
            else if (x == 6) x = 4;
            else if (x == 7) x = 5;
            else if (x == 8) x = 6; 
            else if (x == 9) x = 7;
            else if (x == 10) x = 8;
            else if (x == 11) x = 9; 
            else if (x == 12) x = 9;
            else if (x == 13) x = 10;
            else if (x == 14) x = 11; 
        }
        else if (y == 1 && old_y == 2){
            if      (x == 0) x = 1;
            else if (x == 1) x = 2;
            else if (x == 2) x = 3; 
            else if (x == 3) x = 4;
            else if (x == 4) x = 5;
            else if (x == 5) x = 6; 
            else if (x == 6) x = 7;
            else if (x == 7) x = 8;
            else if (x == 8) x = 9; 
            else if (x == 9) x = 11;
            else if (x == 10) x = 12;
            else if (x == 11) x = 13; 
            else if (x == 12) x = 13;
            else if (x == 13) x = 14; 
        }
        // else if (y == 2 && old_y == 3){
            
        // }
        // else if (y == 3 && old_y == 4){
            
        // }
        else if (y == 4 && old_y == 5){
            if      (x == 0) x = 0;
            else if (x == 1) x = 2;
            else if (x == 2) x = 4; 
            else if (x == 3) x = 7;
            else if (x == 4) x = 9;
            else if (x == 5) x = 11; 
            else if (x == 6) x = 12;
            else if (x == 7) x = 12;
            else if (x == 8) x = 12;
        }
    } else if (keyboard_down_pressed){
        if (y < 5) y = y + 1;
        else y = 5;
        if (y == 1 && old_y == 0){
            if      (x == 0) x = 1;
            else if (x == 1) x = 3;
            else if (x == 2) x = 4; 
            else if (x == 3) x = 5;
            else if (x == 4) x = 6;
            else if (x == 5) x = 7; 
            else if (x == 6) x = 8;
            else if (x == 7) x = 9;
            else if (x == 8) x = 10; 
            else if (x == 9) x = 11;
            else if (x == 10) x = 13;
            else if (x == 11) x = 14; 
        }
        if (y == 2 && old_y == 1){
            if      (x == 0) x = 0;
            else if (x == 1) x = 0;
            else if (x == 2) x = 1; 
            else if (x == 3) x = 2;
            else if (x == 4) x = 3;
            else if (x == 5) x = 4; 
            else if (x == 6) x = 5;
            else if (x == 7) x = 6;
            else if (x == 8) x = 7; 
            else if (x == 9) x = 8;
            else if (x == 10) x = 9;
            else if (x == 11) x = 9; 
            else if (x == 12) x = 9;
            else if (x == 13) x = 11; 
            else if (x == 14) x = 13;
        }
        else if (y == 5 && old_y == 4){
            if      (x == 0) x = 0;
            else if (x == 1) x = 0;
            else if (x == 2) x = 1; 
            else if (x == 3) x = 1;
            else if (x == 4) x = 2;
            else if (x == 5) x = 2; 
            else if (x == 6) x = 3;
            else if (x == 7) x = 3;
            else if (x == 8) x = 3;
            else if (x == 9) x = 4;
            else if (x == 10) x = 4;
            else if (x == 11) x = 5;
            else if (x == 12) x = 7;
        }
    }
    // then we do the left and right and make sure we do not go over
    if (keyboard_left_pressed){
        if (x > 0) x = x - 1;
        else x = 0;
    } else if (keyboard_right_pressed){
        if (y == 1){
            if (x < 14) x = x + 1;
            else x = 14;
        } else if (y == 2){
            if (x < 13) x = x + 1;
            else x = 13;
        } else if (y == 5){
            if (x < 8) x = x + 1;
            else x = 8;
        } else {
            if (x < 12) x = x + 1;
            else x = 12;
        }
    } else {
        if (y == 1){
            if (x > 14) x = 14;
        } else if (y == 2){
            if (x > 13) x = 13;
        } else if (y == 5){
            if (x > 8) x = 8;
        } else {
            if (x > 12) x = 12;
        }
    }
    // Final we do some special up and down movements so the curser does not fly around
    // First we do the down actions


    if(keyboard_is_showing){
        uint32_t tmp = CORE_OUTPUT_REGISTOR();
        tmp = tmp | 0x00000010;
        CORE_OUTPUT_REGISTOR() = tmp;
        bool enable = 0;
        if(updated_keyboard){
            OsdClear();
            OsdUpdate(0);
            if (y == 0) enable = 1;
            OSD_Keyboard_1_line(enable,x);
            enable = 0;
            if (y == 1) enable = 1;
            OSD_Keyboard_2_line(enable,x);
            enable = 0;
            if (y == 2) enable = 1;
            OSD_Keyboard_3_line(enable,x);
            enable = 0;
            if (y == 3) enable = 1;
            OSD_Keyboard_4_line(enable,x);
            enable = 0;
            if (y == 4) enable = 1;
            OSD_Keyboard_5_line(enable,x);
            enable = 0;
            if (y == 5) enable = 1;
            OSD_Keyboard_6_line(enable,x);
            enable = 0;
            InfoEnable(10, 10, 32, 8, 0xffffffff, 0); // Had to use the info as some of the PCE outputs are smaller
        }
        uint8_t tmpk;
        if (keyboard_a_pressed == 1 && keyboard_toggle_a_button_pressed == 0) {
            tmpk = osdKeyboardlocater[y][x]; // To unpress we send the MSB to 1 and the same code back to say unpressed
            HPS_spi_uio_cmd8(UIO_KEYBOARD, tmpk);
        } else if (keyboard_a_pressed == 0 && keyboard_toggle_a_button_pressed == 1) {
            tmpk = osdKeyboardlocater[y][x]; // To unpress we send the MSB to 1 and the same code back to say unpressed
            tmpk = 0x80 | tmpk;
            HPS_spi_uio_cmd8(UIO_KEYBOARD, tmpk);
        }
    } else {    
        OsdDisable();
        uint32_t tmp = CORE_OUTPUT_REGISTOR();
        tmp = tmp & 0xFFFFFFEF;
        CORE_OUTPUT_REGISTOR() = tmp;
    }

    // this allows me to hold the state of the key press after the next time the function passes like a debounce
    if (keyboard_a_pressed == 1) keyboard_toggle_a_button_pressed = 1;
    else keyboard_toggle_a_button_pressed = 0;
}

