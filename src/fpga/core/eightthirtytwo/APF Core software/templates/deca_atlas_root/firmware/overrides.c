// Remove anything that is not needed

// // Initial ROM [Not needed if already defined in config.h]
// const char *bootrom_name="NEXT186 ROM";

//Note the filename must be in 8/3 format with no dot and capital letters. 
//If the name have less than 8 letters then leave spaces so total characters must be 11.

// // Initial VHD
// const char *bootvhd_name="NEXT186 VHD";
// char *autoboot()
// {
// 	char *result=0;
// 	if(!LoadROM(bootrom_name))
// 		result="ROM loading failed";
// 	loadimage(bootvhd_name,0);
// 	return(result);
// }

//NOTE: if having problems loading VHD you could try to load first VHD and then the ROM


// Initial ROM   //Hack to show OSD at core bootup
const char *bootrom_name="AUTOBOOTNES";
extern unsigned char romtype=0;

char *autoboot()
{
	char *result=0;
	romtype=0;
//	loadimage("NEXT186 VHD",0);
	if(!LoadROM(bootrom_name))
		result="Show/hide OSD = key F12";
	return(result);
}


/*    worked for BBC-micro, methods above didn't

#include "diskimg.h"

const char *bootvhd_name="BBC     VHD";
const char *bootrom_name="BBC     ROM";

char *autoboot()
{
    char *result=0;
    diskimg_mount(bootvhd_name,0);
	if(!LoadROM(bootrom_name))
		result="ROM loading failed";
    return(result);
}
*/

