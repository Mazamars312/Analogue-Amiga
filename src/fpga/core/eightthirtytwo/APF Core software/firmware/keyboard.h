#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_EXT 0xe0
#define KEY_KEYUP 0xf0

#define KEY_F1  0x5
#define KEY_F2 	0x6
#define KEY_F3 	0x4
#define KEY_F4 	0x0C 
#define KEY_F5 	0x3
#define KEY_F6 	0x0B 
#define KEY_F7 	0x83
#define KEY_F8 	0x0A 
#define KEY_F9 	0x1
#define KEY_F10 0x9
#define KEY_F11 0x78
#define KEY_F12 0x7

#define KEY_NKENTER 0xDA

#define KEY_CAPSLOCK 0x58
#define KEY_NUMLOCK 0x77
#define KEY_SCROLLLOCK 0x7e
#define KEY_LEFTARROW 0xeb
#define KEY_RIGHTARROW 0xf4
#define KEY_UPARROW 0xf5
#define KEY_DOWNARROW 0xf2
#define KEY_ENTER 0x5a
#define KEY_PAGEUP 0xfd
#define KEY_PAGEDOWN 0xfa
#define KEY_SPACE 0x29
#define KEY_ESC 0x76

#define KEY_K 0x42
#define KEY_L 0x4B
#define KEY_SEMICOLON 0x4C
#define KEY_APOSTROPHE 0x52
#define KEY_Z 0x1A
#define KEY_X 0x22
#define KEY_C 0x21
#define KEY_V 0x2A
#define KEY_B 0x32
#define KEY_N 0x31
#define KEY_M 0x3A

#define KEY_A 0x1c
#define KEY_D 0x23
#define KEY_W 0x1d
#define KEY_S 0x1b
#define KEY_LSHIFT 0x12
#define KEY_RSHIFT 0x59
#define KEY_LCTRL 0x14
#define KEY_RCTRL 0x94
#define KEY_ALT	0x11
#define KEY_ALTGR 0x91
#define KEY_LGUI 0x9f
#define KEY_RGUI 0xa7
#define KEY_MENU 0xaf

#define KEY_COMMA 0x41
#define KEY_PERIOD 0x49
#define KEY_SLASH 0x4a

#define KEY_1 0x16
#define KEY_2 0x1e
#define KEY_3 0x26
#define KEY_4 0x25
#define KEY_5 0x2E
#define KEY_6 0x36
#define KEY_7 0x3D
#define KEY_8 0x3E
#define KEY_9 0x46
#define KEY_INS 0xf0
#define KEY_0 0x45
#define KEY_HOME 0xe6

#define KEY_R 0x2D
#define KEY_T 0x2C
#define KEY_NUMLOCK 0x77
#define KEY_Y 0x35
#define KEY_U 0x3C
#define KEY_NKASTERISK 0x7C
#define KEY_I 0x43
#define KEY_MINUS 0x7B
#define KEY_O 0x44
#define KEY_NK7 0x6C
#define KEY_P 0x4D
#define KEY_NK8 0x75
#define KEY_LEFTBRACE 0x54
#define KEY_NK9 0x7D
#define KEY_RIGHTBRACE 0x5B
#define KEY_NKPLUS 0x79
#define KEY_HASH 0x5D // BACKSLASH on US keyboards
#define KEY_NK4 0x6B

#define KEY_NK6 0x74
#define KEY_NK1 0x69
#define KEY_NK2 0x72
#define KEY_NK3 0x7A
#define KEY_NK0 0x70
#define KEY_NKPERIOD 0x71

#define KEY_E 0x24
#define KEY_F 0x2B
#define KEY_G 0x34
#define KEY_H 0x33
#define KEY_J 0x3B
#define KEY_Q 0x15

#define KEY_BACKSPACE 0x66
#define KEY_NK5 0x73
#define KEY_NKSLASH 0xCA
#define KEY_EQUALS 0x55
#define KEY_NKPOINT 0x71
#define KEY_NKMINUS 0x84

#define KEY_TICK 0x0E
#define KEY_DELETE 0xf1
#define KEY_BACKSLASH 0x61  // Not present on US keyboards
#define KEY_TAB 0x0D

#define KEY_PRTSCRN 0xfc // By rights should be 0xe0 0x12 0xe0 0x7c, so 0x92 0x7c, but the BBC core only checks for 0xe0 0x7c.
	
int UpdateKeys(int blockkeys);
int HandlePS2RawCodes(int blockkeys);
void ClearKeyboard();

int TestKey(int rawcode);

// Each keytable entry has two bits: bit 0 - currently pressed, bit 1 - pressed since last test
extern unsigned int keytable[16];

#endif

