# Commodore Amiga core for Analogue Pocket

The Commodore Amiga was a personal computer that replaced the well-loved Commodore 64 in the Commodore product line. It also was one of the many 68K-based personal computers that showed that this CPU was a powerhouse when integrated with additional great hardware.

This is based on the MiSTer GitHub extension of the Amiga MiST project (Also known as Minimig) and the VexRISCV RISC-V chip for the Media Processing Unit (MPU) between the core and APF framework for floppy, hard drive and CD-ROM access.

This core works best with the [AmigaVision](https://amiga.vision/) configurations.

Will only work on Analogue OS Firmware 2.1 and now works with the default AmigaVision harddrive image, no Pocket-specific version required anymore!

Support for hard drive images larger than 4GB was added with support for LBA48 in the MPU and Analogue Firmware.

If you are upgrading from an earlier version of the Amiga core on Pocket with AmigaVision installed:

* `MegaAGS-Pocket.hdf` and `MegaAGS.hdf` are no longer referenced in the `data.json` file, so we recommend downloading the latest release that has the file names `AmigaVision.hdf` and `AmigaVision-Saves.hdf`. 
* If you want to use your existing setup instead of downloading the latest version, just rename the files, but we always recommend using the latest version, since the project is very active in fixing any issues.

## What can it do?

* 68000, 68010 and 68020 CPUs
* Chip Memory: 512kB, 1MB, 1.5MB and 2MB
* Slow Memory: None, 512kB, 1MB, 1.5MB
* Fast Memory: None/2/4/8MB for the 68000 CPU
* Fast Memory: None/2/4/8/16/32MB for the 68020 CPU setup
* ECS and AGA Chipsets
* Turbo Boot
* Floppy Read/Write support for ADF files
* LBA48 support enabled — it can read and write to HDF hard disk images up to 2Tbytes now!
* Can also access up to 4 hard drive HDF files (requires drivers on the Amiga side)
* Can use a GameBoy link cable for Amiga games that support nullmodem for multiplayer action!

## How to set up

* Place your ADF and HDF images in the `/Assets/amiga/common` folder.
* Place the Kickstart ROM (what you call BIOS on the PC) in the `/Assets/amiga/common` folder (Any original 256/512/1024kB firmware ROM will work — no encrypted ROMs yet). Currently `AmigaVision.rom` is the default ROM.
* Make sure that the `/Assets/amiga/Mazamars312.Amiga/` folder has the `amiga_mpu_bios.bin` file — this is supplied and should already be there.
* When changing system configurations, you **must** select the "Reset Amiga" in the menu to apply the config.
* All hard drive and floppy images should be placed in the `/Assets/amiga/common` folder.
* For custom hard drive images: In the `/Cores/Mazamars312.Amiga/` folder, the `data.json` file would need to be edited with the files names for each HDF file placed in data slots 310-313. By default, the AmigaVision images are referenced, but you can change the names to use other hard drive files. These cannot be changed on the fly due to how the HDF images work in the core. They are limited to 2TB drives. Anything larger will need different sector sizes to run.
* If HDD3 or HDD4 are needed make sure your Kickstart has been patched to support this, or install AtapiMagic/IDEFix97. This is not a Pocket Core/MPU issue, but a limitation of AmigaOS from back in the day.

## How can I get AmigaVision running on this core?

For this you are required to have the following 3 files to place in to the `/Assets/amiga/common` folder

* The Kickstart ROM — `AmigaVision.rom`
* The Main hard drive image — `AmigaVision.hdf`
* The Save hard drive image — `AmigaVision-Saves.hdf`

The recommended settings in the menu are as follows:

* **FDD/HDD LED**: `Off`
* **Port 1/Port 2**: `Mouse/CD32`
* **Mouse Speed**: `10`
* **System Config**: `AGA/PAL/Turbo`
* **CPU Type:** `68020 CPU — No Cache`
* **Memory: Chip**: `2MB`
* **Memory: Slow**: `None`
* **Memory: Fast**: `32MB`
* **Audio Filter**: `A500/PWM`
* **Audio Mix**: `50%`

These can also be set to these values by selecting "Reset all to Default" in the interaction menu. You should always Reset to Defaults when you do an upgrade to ensure you have the latest settings.

## Do you recommend using the Dock?

* Yes, as you can then fully use the keyboard and mouse to interface with the core. Just make sure you select Port1/Port2 as Mouse/Joystick or Mouse/CD32 when using the real mouse. (The new APF firmware has been found to have a bug with the mouse plugged in — am awaiting for an update on this)
* The mouse is also emulated on the left analogue thumb stick at the same time as joypad 1 is being used.
* Be aware that you will need a controller to access the Menu if you want to make changes.
* The mouse needs some more work on the clamping and timing for it to work with every mouse. The current mouse I have is a lower sensitivity mouse and I find that it works fine, but high res based mice will cause the mouse to fly off in random locations. More work will be done once the MPU has some resources changed in it.

## Menu Overview

* **Load BIOS:** This will allow you to reload new BIOSes on the fly — will cause a reset.
* **Load Floppy Drive 0-1:** This will allow you to load different floppies while playing.
* **Reset Amiga:** This will reset the core, and any change in settings will be applied.
* **FDD/HDD LED:** this places a FDD or HDD activity light on the screen. The HDD light sometimes permanently stays on, unknown why yet.
* **Floppy Drives:** this will allow you to select how many drives are installed, and delimit the transfer rate of the Amiga core.
* **Port 1/Port 2:** This allows you to change and swap the controller ports with CD32 Gamepad (4 buttons + L1/R1), Normal Joystick (1 button) and/or Mouse — if you need to use the emulated mouse, make sure you use the Mouse/Joystick or Mouse/CD32 options.
* **Mouse Speed:** This will change the update rate of both the emulated and real mouse movements.
* **Screen Movement X/Y:** Moves the screen around.
* **System Config:** Allows you to select PAL/NTSC refresh, ECS (Amiga 500/600/2000) hardware or newer AGA (Advanced Graphics Architecture) hardware, Turbo Boot or standard boot modes.
* **CPU Type:** Allows you which type of CPU you want installed 68000, 68010, 68020 with no cache, 68020 with cache.
* **Memory: Chip:** Size of the Chip memory.
* **Memory: Slow:** Size of the Slow memory. Typically never used, Fast Memory is… faster.
* **Memory: Fast:** On the 68000 and 68010 CPU this can be None/2/4/8MB, on the 68020 this can also be 16 or 32.
* **Audio Filter:** There are 4 options — A500 or A1200 filters with or without PWM.
* **Audio Mixer:** This will mix the left and right audio streams together. This defaults to 50%.

## Is there a way to emulate a mouse with the D-pad while not connected to the dock?

* You can toggle this with the start button — the D-pad becomes the movement, and the buttons are emulated on the left and right triggers.
* Please note that you need to have the Port1/Port2 setup as Mouse/Joystick1 or Mouse/CD32 for the mouse to work.

## Is there a way to emulate a keyboard while not connected to the dock?

* Press the Select button and an on-screen keyboard will come up. There is no way to press two keys at once yet.

## Can I write to floppy disks?

* Yes, the core supports this.

## Wait, you have another CPU in the FPGA?

* Yes, the MPU is a VexRISCV CPU configured with a small footprint to interface with the APF interface to the HPS bus currently in the MiSTer Amiga core.

### Specs:

* Running at 74.2MHz (Same as the APF bus).
* Connected to the core via a 16bit HPS Bus.
* Two timers — one for interrupts and the other for user timing.
* Currently, 128KB of memory/RAM for the MPU's program and buffers* Work on interrupts and using a PSRAM chip is needed on the next steps

## How did you speed up load times?

* Through talking to the Analogue techs, it is faster to buffer more data and have that ready for the core to use then smaller 512 chunks that the original code would do. This is affected by the block size of the SD Card formats and also if you use FAT32 or NTFS.

## What resolutions will work?

* Currently, the standard Amiga resolutions for both the OCS and AGA work. Special ECS resolutions like 1024x200 or 1024x400 will not, since the max resolution of the Analogue Pocket is 800x720.
* Due to memory access, the RTG resolutions will not work right now. They are also not used in the AmigaVision list of games, so it's not something that is particularly important to support.

## Game X does not work!

* A few games only work from ADF floppy images with the exact correct version of Kickstart.
* The main goal of this project was getting an MPU Framework so external media can be accessed, and help giving developers some tools to help create more cores on the Pocket.
* I do expect that there is more to learn about the Amiga to get many of these bugs resolved.
* For multi-disk games, also try putting disks in other drives. Make sure you change the amount of installed drives to the amount needed. (Up to 2)

## AmigaVision does not start up, I only get a black screen

The AmigaVision setup requires a 68020 CPU, AGA Chipset and Fast Memory to run. Make sure you do a "Reset all to Default" to get the core to boot.

Do note that it takes a while to boot up, especially if you have a slow SD card. It's a 9GB+ hard drive file that needs to be mounted and booted from.

## Changes from 0.2.1 to 0.2.2

* Hybris scoreboard graphics issue fixed.
* Menu label cleanup.
* Updated to latest AmigaVision file naming.

## Changes from 0.1.2 to 0.2.1

* ICACHE Fixed for WHDLoad to work correctly
* LBA48 added to support larger HDD images to 2TB in size
* The joysticks are completely disabled when using the emulated mouse or keyboard
* Audio Mixing — Still some work to be done here to get rid of the snap or pops from the lower frequencies to 48kHz
* Some changes to the caching for the MPU to speed up some processes

## Changes from 0.1.0 to 0.1.2

* Just getting the mouse working on the dock
* Thanks to Analogue fixing up the line bug that was found with this core on the dock.

## Changes from 0.0.6 to 0.1.0

* Hard Drive support for the Pocket
* Proper 640x400/200 video outputs work!
* Write to floppy images
* More memory for the fast memory locations and tested working correctly.
* Larger memory footprint for the MPU to hold all this awesomeness!!! 128KBYTE OF POWER!
* A OSD Keyboard on the pocket when you are playing on pocket mode.
* Null modem support using a Gameboy serial cable between two Pockets! This could also open up MIDI support.
* Also a emulated mouse on a analogue joystick in docked mode (This is the left thumb stick and the left and right triggers for the mouse clicks)
* Dropped the floppy drives from 4 to 2 as there was not enough room in the interation menu at this moment.

## Changes from 0.0.5 to 0.0.6

* Interlace mode corrected for games like DarkSeed.
* Folder names changed for Unix/Linux/Mac OS and to help with the uploader system.
* Faster floppy access times using a larger buffer in the MPU core and caching.

## What will be next for updates?
* Clock input for the Amiga to keep its time.
* Figure out why swapping disks will not always work.

## Credits

* This core is based on Rok Krajnc project (minimig-de1).
* Original Minimig sources from Dennis van Weeren with updates by Jakub Bednarski are published on Google Code and the Community.
* The [AmigaVision](https://amiga.vision/) team — Limi, Optiroc and hitm4n for their help and insight into the Amiga, and helping debug the Pocket at the same time with me.
* ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang (minimig_tc64) and A.M. Robinson (minimig_tc64).
* MiSTer project by Sorgelig (MiSTer) and the Community.
* TG68K.C core by Tobias Gubener.
* VexRISCV Team — https://github.com/SpinalHDL/VexRiscv
* Boogerman for some coding questions — You still owe me on the use of my JTAG tho LOL.
* Electron Ash, AGG23 as a beta tester.
* Terminator2k2 for his fantastic images for the bootup.
* And many more — please message me and I would happily add you to this list if I forgot you!
