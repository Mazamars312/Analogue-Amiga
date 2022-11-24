#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "minimig_fdd.h"
#include "printf.h"
#include "hardware.h"
#include "apf.h"
#include "spi.h"
#define DATASLOT_FDD_BASE 320
#define DATASLOT_HDD_BASE 310
void minigmig_reset(int reset){
    RESET_CORE(0) = reset;
};
// here is the floppy Drive

void minigmig_startup_dataslot_updates() {
  int i = 0;
  while (i != 4)
  {
    if (dataslot_size(i+DATASLOT_FDD_BASE) != 0){

      InsertFloppy(&df[i], dataslot_size(i+DATASLOT_FDD_BASE), i+DATASLOT_FDD_BASE);
    }
    i++;
  }

  UpdateDriveStatus();
};

void minimig_fdd_update(){
  int i = 0;
  while (i != 4)
  {
    if (dataslot_size(i+DATASLOT_FDD_BASE) != 0){
      InsertFloppy(&df[i], dataslot_size(i+DATASLOT_FDD_BASE), i+DATASLOT_FDD_BASE);
    }
    i++;
  }
  UpdateDriveStatus();
};
void minimig_timer_update(){

};
void minimig_poll_io(){
      unsigned char  c1, c2;
  		mister_EnableFpga();
  		uint16_t tmp = spi_w(0);
  		c1 = (uint8_t)(tmp >> 8); // cmd request and drive number
  		c2 = (uint8_t)tmp;      // track number
  		spi_w(0);
  		spi_w(0);
  		mister_DisableFpga();
      HandleFDD(c1, c2);
};
