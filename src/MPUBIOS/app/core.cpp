


 #include "core.h"

 // Sends a reset via the HPS Bus
 void core_reset(int reset){
 	HPS_spi_uio_cmd8(UIO_MM2_RST, reset);
 	mainprintf("\033[36;5;4mreset %d\033[0m\r\n",reset);
 };

 void full_core_reset(){
 	mainprintf("starting the reset\r\n");
 	riscusleep(200);
 	AFP_REGISTOR(0) = 0;	// This makes the reset registor on the APF bus go back to zero for the user to see - need to do a read-modify-write back here
 	riscusleep(20000);
 	minimig_restart_first();
 	core_update_dataslots();
 	riscusleep(500);
 	mainprintf("Completed the reset\r\n");
 }
 // Update dataslots

 void core_update_dataslots(){
 	int tmp = DATASLOT_UPDATE_REG(1);
 	mainprintf("dataslot Update %d\r\n", tmp);
 	if (tmp == DATASLOT_BIOS_BASE){
		core_reset(6);
 		riscusleep(200);
 		minimig_restart_first();
 		riscusleep(200);
		core_reset(0);
 	} else if (tmp >= DATASLOT_FDD_BASE && tmp <= DATASLOT_FDD_BASE + 3 ) {
 		minimig_fdd_update();
	} else if (tmp >= DATASLOT_HDD_BASE && tmp <= DATASLOT_HDD_BASE + 3 ) {
 		minimig_hdd_update();
	}
 };

 void core_poll_io(){
       // Here is where you do your polling on the core - eg Floppy and CDROM data

       unsigned char  c1, c2;
   		HPS_EnableFpga();
   		uint16_t tmp = spi_w(0);
   		c1 = (uint8_t)(tmp >> 8); // cmd request and drive number
   		c2 = (uint8_t)tmp;      // track number
   		spi_w(0);
   		spi_w(0);
   		HPS_DisableFpga();
        HandleFDD(c1, c2);
		uint16_t sd_req = ide_check();
		if ((sd_req >> 3) == 5) mainprintf ("\033[38;1;5m %d sdreq5\033[0m\r\n", sd_req >> 3);
		if ((sd_req >> 3) == 7) mainprintf ("\033[38;1;5m %d sdreq7\033[0m\r\n", sd_req >> 3);
		if ((sd_req >> 3) == 6) mainprintf ("\033[38;1;5m %d sdreq6\033[0m\r\n", sd_req >> 3);
		if (sd_req & 0x8000)
		{
			ide_io(0, sd_req & 7);
			ide_io(1, (sd_req >> 3) & 7);
		}
 };

 void core_reg_update(){
	uint32_t tmp = CORE_OUTPUT_REGISTOR();
    tmp = tmp & 0xFFFFFFF0;
    CORE_OUTPUT_REGISTOR() = tmp | (AFP_REGISTOR(7) & 0x0000000f);

 };

 void core_restart_first(){
 	// what to do to start up the core if required
	core_reset(6);
 };

 void core_restart_running_core() {
 	// this can be used for restarting the core

 };

 void core_input_setup() {
 	// this is used for controller setup
 	input_setup();
 };

 void core_input_update() {
 	// this is called via the interrupts.c for controller updates to the core
 	input_update();
 };

 void core_interupt_update(){
	input_setup();
 };
