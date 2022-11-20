#ifndef SETTINGS_H

/* If settings files are supported, we provide a default implementation of loading and saving configs,
   which can be replaced in overrides.c */

int configtocore(char *buf);
void coretoconfig(char *buf);
int loadsettings(const char *filename);
int savesettings(const char *filename);

#define SCANDOUBLE_OFF 1
#define SCANDOUBLE_ON 0
#define AUTOSCANDOUBLER_FILENAME "15KHZ   CFG"
void ToggleScandoubler();
void AutoScandoubler();
#define GetScandouble() scanddouble

#endif

