#ifndef OSD_H_INCLUDED
#define OSD_H_INCLUDED
#include <inttypes.h>
// #include <inttypes.h>
// some constants
#define DISABLE_KEYBOARD 0x02        // disable keyboard while OSD is active
#define OSD_INFO         0x04        // display info
#define OSD_MSG          0x08        // display message window

#define REPEATDELAY      500         // repeat delay in 1ms units
#define REPEATRATE       50          // repeat rate in 1ms units

#define OSD_ARROW_LEFT   1
#define OSD_ARROW_RIGHT  2

/*functions*/
void OsdWrite(const char *s="", unsigned char inver=0, unsigned char stipple=0, char usebg = 0, int maxinv = 32, int mininv = 0);
void OsdWriteOffset(unsigned char n, const char *s, unsigned char inver, unsigned char stipple, char offset, char leftchar, char usebg = 0, int maxinv = 32, int mininv = 0); // Used for scrolling "Exit" text downwards...
void OsdWriteDirect(uint16_t n, const char * s, bool invert = 0, unsigned char stipple = 0, int maxinv = 32, int mininv = 0);
void OsdClear();
void OsdEnable(unsigned char mode);
void InfoEnable(int x, int y, int width, int height, int background_color = 219, int text_color = 65535);
void OsdDisable();
void OsdUpdate(int line);
void OsdSetSize(int n);
int OsdGetSize();

#define OsdIsBig (OsdGetSize()>8)

#endif
