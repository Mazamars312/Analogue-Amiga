# Analogue Pocket - Commodore Amiga 500 - 0.0.6 Alpha

The Commodore Amiga 500 was a personal computer that replaced the well-loved Commodore C64 in the Commodore product line. It also was one of the many 68K personal computers that showed that this CPU was a powerhouse when integrated with great hardware beside it.

This is based on the Mister Github of the Amiga Mist project (Also known as the minimig) and the VexRISCV RISC-V chip for the Media Processing Unit (MPU) between the core and APF framework for floppy, hard drive and CD-ROM access.

## What can it do?
* Both 68000 and 68020 CPUs
* Chip memory 512kB, 1MB, 1.5MB and 2MB
* Slow memory none, 512kB, 1MB, 1.5MB
* Fast memory of none/2/4/8MB for the 68000 CPU
* Fast memory of 16/32MB for the 68020 CPU setup - This is experamental at this moment
* AGA Chipset
* Turbo boot
* It can only read ADF disk images (Floppy disks). Writes will come soon with Hard Drive access.

## How to setup
* PLEASE NOTE - Folder names have been changed to help with the updater - my mistake here.
* Place your ADF floppy images in the `\Asset\amiga\common` folder.
* Place the Kickstart BIOS rom in the `\Asset\amiga\common` folder (I'm using Kickstart 1.2 for now, but any original 256/512/1024kB firmware ROM will work - no encrypted BIOSes yet). Name it `kickstart.rom` to autoload it.
* Make sure that the `\Asset\amiga\Mazamars312.Amiga\` Folder has the `mpu.bin` file - this is supplied and should already be there.
* When changing system configurations, you must select the "CPU reset" in the menu to apply the config.

## Is the best way to play this using the dock?

* Yes, as you can then fully use the keyboard and mouse to interface with the core. Just make sure you select Port1/Port2 as Mouse/Joystick1 or Mouse/CD32 when using the real mouse.
* Be aware that you will need a controller to access the Menu if you want to make changes.

## Menu Listing
* Load Bios - This will allow you to reload new BIOSes on the fly - will cause a reset.
* Load Floppy Drive 0-3 - This will allow you to load different floppies while playing.
* CPU Reset - This will reset the core, and any change in settings will be applied.
* Installed Floppy Drives - this will allow you to select how many drives are installed, and delimit the transfer rate of the Amiga core.
* Port 1/Port 2 - This allows you to change and swap the controller ports with CD32/Normal Joysticks/Mouse - if you need to use the emulated mouse, make sure you use the mouse/joystick or mouse/CD32 options.
* Mouse Speed - This will change the update rate of both the emulated and real mouse movements.
* Screen Movement X/Y - Moves the screen around.
* System Config - Allows you to select PAL/NTSC refresh, OG (original) hardware or the AGA (Advanced Graphics Architecture) hardware, Turbo Boot or standard boot modes.
* CPU Type - Allows you which type of CPU you want installed 68000, 68010, 68020 with no cache, 68020 with cache.
* Memory Chip - Size of the Chip memory.
* Memory Slow - Size of the slow memory.
* Memory Fast - on the 68000 CPU this is none/2/4/8MB, on the 680X0 this is none or 32 at this moment.

## Is there an emulated mouse while not connected to the dock?

* Yes, Press both the left and right triggers, and the D-pad becomes the movement, and the buttons are emulated on buttons A,B,X. However, they are as follows – B for left click, A for right click and X for middle click. For me, it just felt better when in this mode, but I will look at the interaction menu to change this.
* Please note that you need to have the Port1/Port2 setup as Mouse/Joystick1 or Mouse/CD32 for the mouse to work.

## Is there an emulated Keyboard?

* Not at this moment, this is upcoming for the next build, as I have not decided on the best way to do this yet.

## Can we write to floppy disks at this moment?

* No, I have the code ready, but due to a bug in the framework, I'm holding off on releasing it, as it will damage files on the SD card.

## When are writable disks and hard drives going to be working?

* Now that the framework for the MPU has been built, this is a matter of getting the C++ code working with the MPU and APF interface. There is also the matter of SRAM being connected and tested, to see if we can use this as a buffer as well.

## Wait, you have another CPU in the FPGA?

* Yes, the MPU is a VexRISCV CPU configured with a small footprint to interface with the APF interface to the HPS bus currently in the MiSTer Amiga core.

Specs:
* Running at 74.2MHz (Same as the APF bus).
* Connected to the core via a 16bit HPS Bus.
* Two timers - one for interrupts and the other for user timing.
* Currently, 32KB of memory/RAM for the MPU's program - will be set to 16K for a smaller footprint.
* A 8KB swap buffer for both the APF and MPU to do transfers.


## In the future:
* A separate GitHub for the MPU and the APF framework for other developers to use soon with documentation and how to build with it.
* This has been released for others to have a go with it now.

## How did you speed up load times

* Through talking to the Analogue Techs, it is faster to buffer more data and have that ready for the core to use then smaller 512 chunks that the orginal code would do.

## Why are some of the resolutions not correct?

* I need to see the standard resolutions to ensure the scaler will work correctly.
* Due to memory access, the RTG resolutions will not work on this right now.

## I have game X that does not work!

* From the testing I have been doing, there are multiple configurations, BIOS and even ADF Images that you need to try. Hopefully, once I get both writing to floppy and the hard drive access working, this will be easier. 
* The main goal of this project was getting an MPU Framework so external media can be accessed, and help giving developers some tools to help create more cores on the Pocket. 
* I do expect that there is more to learn about the Amiga to get many of these bugs resolved.
* For multi-disk games, also try putting disks in other drives. Make sure you change the amount of installed drives to the amount needed. (Up to 4)

## Change since 0.0.5 to 0.0.6
* Interlace mode corrected for games like DarkSeed.
* Folder names changed for Unix/Linux/Mac OS and to help with the uploader system.
* Faster floppy access times using a larger buffer in the MPU core and caching.
* But what about the things below? I wanted to sort out these key issues out first and I'm awaiting a bugfix from Analogue to be released. This is due to files being corrupted when writing back to disk.

## What will be the next update?

* First, I'll be updating the Neo Geo core before doing the next lot of updates on this core.
* Then, I want a Virtual Keyboard to help non-dock users to be able to play this while on the go.
* Writable Floppy access.
* HDF Hard drives - will we be able to get all 4 working? It would be a interesting achievement. However, they will only be 4GB max in size due to the APF framework.
* Change or separate the mouse rate for both the emulated mouse and real mouse. Right now this value is used on both, but decreasing the value will make the emulated mouse move slower, but the real mouse move faster.
* An extra 32MB of Fast RAM for the 68020 CPU using the CRAM. 

## Credits

* This source code is based on Rok Krajnc project (minimig-de1).
* Original minimig sources from Dennis van Weeren with updates by Jakub Bednarski are published on Google Code and the Community.
* ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang (minimig_tc64) and A.M. Robinson (minimig_tc64).
* MiSTer project by Sorgelig (MiSTer) and the Community.
* TG68K.C core by Tobias Gubener.
* VexRISCV Team - https://github.com/SpinalHDL/VexRiscv
* Robinsonb5 for his time answering my questions on his 832 CPU - though I did not use this excellent-looking CPU, I was not able to get the C Compiler working 100% on my builds.
* Boogerman for some coding questions - You still owe me on the use of my JTAG tho LOL.
* Electron Ash, AGG23 as a beta tester.
* Terminator2k2 for his fantastic images for the bootup.
* And many more, so please message me and I would happily add them to this!
