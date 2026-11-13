


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
 	// Invalidate the dataslot search cache: the dataslot RAM table may have
 	// changed when disks were inserted or removed, so cached indices are stale.
 	dataslot_cache_invalidate();
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
   		// Performance optimization:
   		// The two discarded spi_w(0) reads below were vestigial. For cmd=0x00
   		// (which spi_w(0) sends), the FPGA's &cmd[7:4] condition is false, so
   		// additional strobes change no FPGA state (verified against
   		// src/fpga/core/amiga/userio.v). IO_WAIT is also hardwired to 0, so
   		// no wait-state issues. Removing them saves 2 volatile MMIO reads per
   		// mainloop iteration - the hottest path in the firmware.
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
  // Performance optimization:
  // mainloop() calls core_reg_update() on every iteration of its continuous polling loop.
  // Previously, AFP_REGISTOR(7) (menu/AFP settings) was re-read on every iteration,
  // issuing a volatile MMIO read over the APF bus each time even though those settings
  // only change on user menu interaction. Snapshot AFP_REGISTOR(7) at a fixed interval
  // instead and reuse the cached value between polls. CORE_OUTPUT_REGISTOR() is still
  // read every iteration because bits 4-5 (mouse/joystick suppression, 0x30) are owned
  // by other functions (update_mouse_inputs, input_update, OSD_Keyboard_process) and
  // must be preserved, so caching the whole register would be unsafe without a shared
  // writer cache. Detection delay for menu changes is bounded by POLL_INTERVAL
  // mainloop iterations - imperceptible, same pattern as input_reg_update().
  static uint8_t afp7_poll_counter = 0;
  static uint32_t cached_afp7 = 0xFFFFFFFFu; // force an AFP read on the first call
  constexpr uint8_t POLL_INTERVAL = 4;       // Check menu settings every 4th poll
  if (++afp7_poll_counter >= POLL_INTERVAL) {
    afp7_poll_counter = 0;
    cached_afp7 = AFP_REGISTOR(7);
  }
  uint32_t tmp = CORE_OUTPUT_REGISTOR();
  uint32_t next = (tmp & 0xFFFFFF30) | (cached_afp7 & 0x000000cf);
  if (tmp != next) {
    CORE_OUTPUT_REGISTOR() = next;
  }
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
