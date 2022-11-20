#include <stdio.h>
#include <string.h>

#include "menu.h"

void activate(int row)
{


}

static char romfilenames[7][30];
static struct menu_entry menu[]=
{
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[0],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[1],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[2],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[3],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[4],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[5],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[6],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[7],MENU_ACTION(&activate)},
	{MENU_ENTRY_NULL,0,0,0,MENU_ACTION(&scrollmenu)}
};


