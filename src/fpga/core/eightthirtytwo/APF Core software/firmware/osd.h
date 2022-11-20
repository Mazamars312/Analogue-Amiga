
#ifndef OSD_H_INCLUDED
#define OSD_H_INCLUDED

/*constants*/
#define OSDCTRLUP        0x01        /*OSD up control*/
#define OSDCTRLDOWN      0x02        /*OSD down control*/
#define OSDCTRLSELECT    0x04        /*OSD select control*/
#define OSDCTRLMENU      0x08        /*OSD menu control*/
#define OSDCTRLRIGHT     0x10        /*OSD right control*/
#define OSDCTRLLEFT      0x20        /*OSD left control*/

// some constants
#define OSDNLINE         8           // number of lines of OSD
#define OSDLINELEN       256         // single line length in bytes

#define MM1_OSDCMDREAD     0x00      // OSD read controller/key status
#define MM1_OSDCMDWRITE    0x20      // OSD write video data command
#define MM1_OSDCMDENABLE   0x41      // OSD enable command
#define MM1_OSDCMDDISABLE  0x40      // OSD disable command
#define MM1_OSDCMDRST      0x80      // OSD reset command
#define MM1_OSDCMDAUTOFIRE 0x84      // OSD autofire command
#define MM1_OSDCMDCFGSCL   0xA0      // OSD settings: scanlines effect
#define MM1_OSDCMDCFGIDE   0xB0      // OSD enable HDD command
#define MM1_OSDCMDCFGFLP   0xC0      // OSD settings: floppy config
#define MM1_OSDCMDCFGCHP   0xD0      // OSD settings: chipset config
#define MM1_OSDCMDCFGFLT   0xE0      // OSD settings: filter
#define MM1_OSDCMDCFGMEM   0xF0      // OSD settings: memory config
#define MM1_OSDCMDCFGCPU   0xFC      // OSD settings: CPU config

#define DISABLE_KEYBOARD 0x02        // disable keyboard while OSD is active

#define REPEATDELAY      500         // repeat delay in 1ms units
#define REPEATRATE       50          // repeat rate in 1ms units
#define BUTTONDELAY      20          // repeat rate in 1ms units


#include <sys/types.h>

void spi_osd_cmd_cont(unsigned char cmd);
void spi_osd_cmd(unsigned char cmd);
void spi_osd_cmd8_cont(unsigned char cmd, unsigned char parm);
void spi_osd_cmd8(unsigned char cmd, unsigned char parm);

#define OsdShowHide(ena) {spi_osd_cmd(MM1_OSDCMDDISABLE+(ena&1));}

/*functions*/
void OsdSetTitle(char *s,int arrow);	// arrow > 0 = display right arrow in bottom right, < 0 = display left arrow
void OsdWrite(int n, char *s, int invert, int stipple);
void OsdClear(void);
void OsdShow(unsigned char mode);
void OsdHide(void);
void OsdWaitVBL(void);
void OsdReset(unsigned char boot);
void ConfigFilter(unsigned char lores, unsigned char hires);
void OsdReconfig(); // Reset to Chameleon core.
// deprecated functions from Minimig 1
void MM1_ConfigFilter(unsigned char lores, unsigned char hires);
void MM1_ConfigScanlines(unsigned char scanlines);
void ConfigVideo(unsigned char hires, unsigned char lores, unsigned char scanlines);
void ConfigMemory(unsigned char memory);
void ConfigCPU(unsigned char cpu);
void ConfigChipset(unsigned char chipset);
void ConfigFloppy(unsigned char drives, unsigned char speed);
void ConfigIDE(unsigned char gayle, unsigned char master, unsigned char slave);
void ConfigAutofire(unsigned char autofire);
unsigned char OsdGetCtrl(void);
void OsdDisableMenuButton(unsigned char disable);
unsigned char GetASCIIKey(unsigned char c);
void OSD_PrintText(unsigned char line, char *text, unsigned long start, unsigned long width, unsigned long offset, unsigned char invert);
void OsdWriteDoubleSize(unsigned char n, char *s, unsigned char pass);
//void OsdDrawLogo(unsigned char n, char row);
void OsdDrawLogo(unsigned char n, char row,char superimpose);
void ScrollText(char n, const char *str, int len, int max_len, unsigned char invert, int len_offset);
void ScrollReset();
void StarsInit();
void StarsUpdate();

void OsdKeySet(unsigned char);
unsigned char OsdKeyGet();

// get/set core currently loaded
void OsdCoreNameSet(const char* str);
char* OsdCoreName();


void OsdWriteStart(int row,int inverse,int stipple);
void OsdPutChar(unsigned char c);
void OsdPuts(unsigned char *s);
void OsdWriteEnd();

#endif

