// Host-side stub of src/MPUBIOS/drivers/osd/osd.h for unit tests.
#ifndef MOCK_OSD_H
#define MOCK_OSD_H

void OsdUpdate(int n);
void OsdClear();
void OsdWrite(const char* s);
void OsdDisable();
void InfoEnable(int x, int y, int width, int height, int background_color, int text_color);

#endif
