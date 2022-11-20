#ifndef FONT_H
#define FONT_H

#define FONT_OFFSET 32
#define FONT_MAX 135

extern unsigned char font[1+FONT_MAX-FONT_OFFSET][8];
#define FONT_ARROW_LEFT 128
#define FONT_ARROW_RIGHT 129
#define FONT_ARROW_UP 130
#define FONT_ARROW_DOWN 131
#define FONT_CHECKMARK 132
#define FONT_CROSS 133
#define FONT_CYCLE 134
#define FONT_ELLIPSIS 135

#endif
