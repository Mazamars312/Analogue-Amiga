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


// ============================================================================
// Machine Profile definitions (Plan A - Named Model Profiles)
// ============================================================================
//
// Profile identifiers (0 = Custom, pass-through existing settings).
// Stable identifiers: do not repurpose released values.
//
// Register field encodings (as consumed by userio.v / minimig.v):
//
//   chipset_config (reg 2, 0xF0000008, 5 bits):
//     bit 0 = Turbo (0 for profiles)
//     bit 1 = NTSC (merged from existing video-standard choice)
//     bit 2 = A1000 mode (0 for profiles)
//     bit 3 = ECS
//     bit 4 = AGA
//     Hardware decodes: ecs = |chipset_config[4:3], aga = chipset_config[4]
//
//   cpu_config (reg 3, 0xF000000C, 5 bits):
//     bits[1:0] = CPU type (00=68k/fx68k, 01=68k10/fx68k, 11=68k20/TG68K)
//     bit 2     = Cache enable
//     bit 3     = Fast kickstart
//
//   memory_config (reg 4, 0xF0000010, 8 bits):
//     bits[1:0] = Chip RAM (0=512K, 1=1M, 2=1.5M, 3=2M)
//     bits[3:2] = Slow RAM (0=None, 1=512K, 2=1M, 3=1.5M)
//     bits[5:4] = Fast RAM (0=None, 1=2M, 2=4M, 3=8M, 4=16M, 5=32M)
//

#include "profile_def.h"

// Resolve the effective machine configuration from a profile identifier and
// the current menu register snapshot using the shared profile definitions.
static void resolve_profile(uint8_t profile_id, uint8_t reg_chip, uint8_t reg_cpu,
                            uint8_t reg_mem, uint8_t *eff_chip, uint8_t *eff_cpu,
                            uint8_t *eff_mem, bool *eff_ide) {
  const struct ProfileDef *prof = nullptr;
  bool ok = resolve_profile_lookup(profile_id, reg_chip, reg_cpu, reg_mem,
                                   eff_chip, eff_cpu, eff_mem, eff_ide, &prof);

  if (!ok || prof == nullptr) {
    mainprintf("Profile warning: unknown profile ID %u, falling back to Custom\r\n", profile_id);
    return;
  }

  if (prof->id == PROFILE_CUSTOM) {
    mainprintf("Profile: Custom (pass-through existing settings)\r\n");
    return;
  }

  mainprintf("Profile: %s (ID %u) -> chip=0x%02X cpu=0x%02X mem=0x%02X ide=%d\r\n",
             prof->name, profile_id, *eff_chip, *eff_cpu, *eff_mem, *eff_ide ? 1 : 0);
}

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

  // Snapshot current menu settings from bridge registers
  uint8_t reg_chip = (uint8_t)AFP_REGISTOR(2);   // System Configuration (0xF0000008)
  uint8_t reg_cpu  = (uint8_t)AFP_REGISTOR(3);   // CPU Type (0xF000000C)
  uint8_t reg_mem  = (uint8_t)AFP_REGISTOR(4);   // Memory (0xF0000010)
  uint8_t reg_flop = (uint8_t)AFP_REGISTOR(1);   // Floppy (0xF0000004)
  uint8_t reg_joy  = (uint8_t)AFP_REGISTOR(5);   // Joystick (0xF0000014)
  uint8_t reg_prof = (uint8_t)AFP_REGISTOR(6);   // Machine Profile (0xF0000018)

  // Resolve effective configuration through the profile resolver
  uint8_t eff_chip, eff_cpu, eff_mem;
  bool eff_ide;
  resolve_profile(reg_prof, reg_chip, reg_cpu, reg_mem, &eff_chip, &eff_cpu, &eff_mem, &eff_ide);

  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , reg_flop);
  // Get the chip setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, eff_chip);
  // Get the Cpu setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_CPU , eff_cpu);
  // Get the MEM setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_MEM , eff_mem);
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , reg_joy);
  // Update the Floppy controller
  minimig_fdd_update();
  minimig_hdd_update();
  mainprintf("DONE Core Setup\r\n");
};

void minimig_restart_running_core() {
  // Snapshot current menu settings from bridge registers
  uint8_t reg_chip = (uint8_t)AFP_REGISTOR(2);   // System Configuration (0xF0000008)
  uint8_t reg_cpu  = (uint8_t)AFP_REGISTOR(3);   // CPU Type (0xF000000C)
  uint8_t reg_mem  = (uint8_t)AFP_REGISTOR(4);   // Memory (0xF0000010)
  uint8_t reg_flop = (uint8_t)AFP_REGISTOR(1);   // Floppy (0xF0000004)
  uint8_t reg_joy  = (uint8_t)AFP_REGISTOR(5);   // Joystick (0xF0000014)
  uint8_t reg_prof = (uint8_t)AFP_REGISTOR(6);   // Machine Profile (0xF0000018)

  // Resolve effective configuration through the profile resolver
  uint8_t eff_chip, eff_cpu, eff_mem;
  bool eff_ide;
  resolve_profile(reg_prof, reg_chip, reg_cpu, reg_mem, &eff_chip, &eff_cpu, &eff_mem, &eff_ide);

  // Get the Floppy Controllers sorted
  HPS_spi_uio_cmd8(UIO_MM2_FLP , reg_flop);
  // Get the chip setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_CHIP, eff_chip);
  // Get the Cpu setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_CPU , eff_cpu);
  // Get the MEM setup sorted (profile-resolved)
  HPS_spi_uio_cmd8(UIO_MM2_MEM , eff_mem);
  // Get the JOY setup sorted
  HPS_spi_uio_cmd8(UIO_MM2_JOY , reg_joy);
  mainprintf("DONE Core Setup\r\n");
};

// Heuristic bootblock profile detection for Amiga games
static bool buffer_has_substr(const uint8_t *buf, int len, const char *sub) {
  int sub_len = strlen(sub);
  if (sub_len > len) return false;
  for (int i = 0; i <= len - sub_len; i++) {
    bool match = true;
    for (int j = 0; j < sub_len; j++) {
      char c1 = (char)buf[i + j];
      char c2 = sub[j];
      // Case-insensitive ASCII comparison
      if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
      if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
      if (c1 != c2) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

void minimig_detect_game_profile() {
  if (!(df[0].status & DSK_INSERTED)) return;

  // Read Track 0 (first 5632 bytes, containing bootblock sectors 0 and 1)
  uint32_t foo1 = (uint32_t)&sector_bufferfdd1;
  while (!dataslot_ready()) {}
  if (dataslot_read(df[0].dataslot, foo1 | APF_ADDRESS_OFFSET, 0, 5632)) {
    mainprintf("Profile detection: failed to read Track 0\r\n");
    return;
  }

  const uint8_t *bb = sector_bufferfdd1;
  int scan_len = 2048; // Scan first 2KB for signatures

  // 1. Platformer profile detection (games benefiting from B Jump + X Special)
  if (buffer_has_substr(bb, scan_len, "TURRICAN") ||
      buffer_has_substr(bb, scan_len, "FACTOR 5") ||
      buffer_has_substr(bb, scan_len, "GIANA") ||
      buffer_has_substr(bb, scan_len, "RUFF") ||
      buffer_has_substr(bb, scan_len, "LIONHEART") ||
      buffer_has_substr(bb, scan_len, "SUPERFROG") ||
      buffer_has_substr(bb, scan_len, "SONIC") ||
      buffer_has_substr(bb, scan_len, "MARIO") ||
      buffer_has_substr(bb, scan_len, "MAGIC POCKETS") ||
      buffer_has_substr(bb, scan_len, "GODS") ||
      buffer_has_substr(bb, scan_len, "ZOOL") ||
      buffer_has_substr(bb, scan_len, "JAMES POND") ||
      buffer_has_substr(bb, scan_len, "CHUCK ROCK")) {
    mainprintf("Auto-detected profile: PLATFORMER (Jump=B, Special=X)\r\n");
    // Control mode: 1 = Platformer
    // We notify via mainprintf and user can see in logs
    return;
  }

  // 2. Driving profile detection (games with gas on Up, brake on Down)
  if (buffer_has_substr(bb, scan_len, "LOTUS") ||
      buffer_has_substr(bb, scan_len, "MAGNETIC FIELDS") ||
      buffer_has_substr(bb, scan_len, "SUPER CARS") ||
      buffer_has_substr(bb, scan_len, "JAGUAR") ||
      buffer_has_substr(bb, scan_len, "CRAZY CARS") ||
      buffer_has_substr(bb, scan_len, "NITRO") ||
      buffer_has_substr(bb, scan_len, "OVERDRIVE") ||
      buffer_has_substr(bb, scan_len, "ROADKILL")) {
    mainprintf("Auto-detected profile: DRIVING (Gas=R, Brake=L)\r\n");
    return;
  }

  // 3. Mouse profile detection (point & click, Workbench, managers)
  if (buffer_has_substr(bb, scan_len, "LEMMINGS") ||
      buffer_has_substr(bb, scan_len, "SETTLERS") ||
      buffer_has_substr(bb, scan_len, "MONKEY ISLAND") ||
      buffer_has_substr(bb, scan_len, "CANNON FODDER") ||
      buffer_has_substr(bb, scan_len, "SENSIBLE") ||
      buffer_has_substr(bb, scan_len, "POPULOUS") ||
      buffer_has_substr(bb, scan_len, "SYNDICATE") ||
      buffer_has_substr(bb, scan_len, "WORKBENCH")) {
    mainprintf("Auto-detected profile: MOUSE (D-pad Cursor, A/B Clicks)\r\n");
    return;
  }

  mainprintf("Auto-detected profile: STANDARD JOYSTICK\r\n");
}

void minimig_fdd_update(){
  bool df0_was_empty = (df[0].status == 0);
  int i = 0;
  while (i != 4)
  {
    uint32_t size = dataslot_size(i + DATASLOT_FDD_BASE);
    if (size != 0){
      UnsertFloppy(&df[i]);
      riscusleep(400000);
      InsertFloppy(&df[i], size, i + DATASLOT_FDD_BASE);
    }
    i++;
  }
  UpdateDriveStatus_fdd();

  // If DF0 was empty and is now loaded, detect profile and do a clean reset of the 68000 CPU!
  // This guarantees trackloader games (like Sensible Soccer) get a completely clean Chip RAM & Agnus state,
  // preventing playfield corruption and bypassing Workbench.
  if (df0_was_empty && (df[0].status & DSK_INSERTED)) {
    mainprintf("DF0 loaded into empty drive - inspecting bootblock\r\n");
    minimig_detect_game_profile();
    mainprintf("Resetting Amiga 68000 to boot directly into floppy game\r\n");
    riscusleep(200000);
    core_reset(1); // 68000 CPU reset
    riscusleep(2000);
    core_reset(0);
    riscusleep(500);
    UpdateDriveStatus_fdd();
    mainprintf("Disk ready in DF0: - booting floppy\r\n");
  }
}

// Rotate floppy disks among drives: DF0 <- DF1 <- DF2 <- DF3 <- DF0
void minimig_fdd_swap_disks(){
  // Check how many drives actually have disks
  int loaded_count = 0;
  for (int d = 0; d < 4; d++) {
    if (df[d].status & DSK_INSERTED) loaded_count++;
  }
  if (loaded_count <= 1) {
    mainprintf("No additional disks loaded to swap\r\n");
    return;
  }

  mainprintf("Swapping disks among drives (rotating DF0<-DF1<-DF2<-DF3)\r\n");
  // Temporarily signal disk ejection on all drives so Amiga trackdisk detects the disk change
  RemoveDriveStatus();
  riscusleep(400000);

  adfTYPE temp = df[0];
  df[0] = df[1];
  df[1] = df[2];
  df[2] = df[3];
  df[3] = temp;

  for (int d = 0; d < 4; d++) {
    df[d].track_prev = -1;
    df[d].sector_offset = 0;
  }

  UpdateDriveStatus_fdd();
  mainprintf("Disk rotation complete! New DF0 dataslot: %d\r\n", df[0].dataslot);
}

void minimig_hdd_update(){
  // If user has a floppy disk loaded in DF0, do NOT mount Hard Drives / Workbench!
  // This keeps the Amiga purely as an unadulterated floppy-based system without Workbench taking over.
  if (df[0].status & DSK_INSERTED) {
    mainprintf("Floppy in DF0 detected - skipping HDD setup to prevent Workbench auto-boot\r\n");
    HPS_spi_uio_cmd8(UIO_MM2_HDD, 0);
    return;
  }

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
