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



 #include "minimig_config.h"


void minimig_bios (){
  int tmp;
  tmp = dataslot_size(210);
  if (tmp == 0x100000) {
    mainprintf("\r\n1M Rom\r\n");
    dataslot_read(210, 0x90F00000, 0, 0x100000);

  }
  else if (tmp == 0x80000){
    mainprintf("\r\n512M Rom\r\n");
    dataslot_read(210, 0x90F00000, 0, 0x80000);
    dataslot_read(210, 0x90F80000, 0, 0x80000);

  }
  else if (tmp ==  0x40000){
    mainprintf("\r\n256M Rom\r\n");
    dataslot_read(210, 0x90f00000, 0, 0x40000);
    dataslot_read(210, 0x90f40000, 0, 0x40000);
    dataslot_read(210, 0x90f80000, 0, 0x40000);
    dataslot_read(210, 0x90fc0000, 0, 0x40000);
  }
  mainprintf("DONE BIOS LOAD\r\n");
};

void minimig_restart_first() {
  // Bios size
  
		
  minimig_bios();

  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , AFP_REGISTOR(1));
  // Get the chip setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, AFP_REGISTOR(2));
  // Get the Cpu setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CPU , AFP_REGISTOR(3));
  // Get the MEM setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_MEM , AFP_REGISTOR(4));
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , AFP_REGISTOR(5));
  // Update the Floppy controller
  minimig_fdd_update();
  minimig_hdd_update();
  mainprintf("DONE Core Setup\r\n");
};

void minimig_restart_running_core() {
  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , AFP_REGISTOR(1));
  // Get the chip setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, AFP_REGISTOR(2));
  // Get the Cpu setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_CPU , AFP_REGISTOR(3));
  // Get the MEM setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_MEM , AFP_REGISTOR(4));
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , AFP_REGISTOR(5));
  mainprintf("DONE Core Setup\r\n");
};

void minimig_fdd_update(){
  int i = 0;
  while (i != 4)
  {
    if (dataslot_size(i+DATASLOT_FDD_BASE) != 0){
			UnsertFloppy(&df[i]);
      riscusleep(600000);
      InsertFloppy(&df[i], dataslot_size(i+DATASLOT_FDD_BASE), i+DATASLOT_FDD_BASE);
    }
    i++;
  }
  UpdateDriveStatus_fdd();
}

void minimig_hdd_update(){
  mainprintf("Harddrive are being setup in the core\r\n");
    int temp = 0;
    if (dataslot_size(310) != 0){
      ide_open(0,310);
      temp = temp | 0x23;
    }
		if (dataslot_size(311) != 0){
      ide_open(1,311);
      temp = temp | 0x29;

    }
		if (dataslot_size(312) != 0){
      ide_open(2,312);
      temp = temp | 0x25;

    }
		if (dataslot_size(313) != 0){
      ide_open(3,313);
      temp = temp | 0x31;

    }
    
  // Get the HDD setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_HDD , temp);
  mainprintf("Harddrive setups %0.4x \r\n", temp);
}
