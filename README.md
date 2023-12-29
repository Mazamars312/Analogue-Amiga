Analogue Pocket - Commodore Amiga - 0.1.0
=========================================

The Commodore Amiga was a personal computer that replaced the well-loved
Commodore 64 in the Commodore product line. It also was one of the many 68K-based
personal computers that showed that this CPU was a powerhouse when integrated
with additional great hardware.

This is based on the MiSTer Github of the Amiga MiST project (Also known as
Minimig) and the VexRISCV RISC-V chip for the Media Processing Unit (MPU)
between the core and APF framework for floppy, hard drive and CD-ROM access.

This core works best with the [AmigaVision](https://amiga.vision/) configurations.

What can it do?
---------------

-   Both 68000 and 68020 CPUs

-   Chip memory 512kB, 1MB, 1.5MB and 2MB

-   Slow memory none, 512kB, 1MB, 1.5MB

-   Fast memory of none/2/4/8MB for the 68000 CPU

-   Fast memory of 16/32MB for the 68020 CPU setup

-   AGA Chipset

-   Turbo boot

-   It can Read and write to ADF disk images (Floppy disks)

-   Can also access up to 4 Harddrive HDF files

How to setup
------------

-   Place your ADF and HDF images in the `\Assets\amiga\common` folder.

-   Place the Kickstart BIOS rom in the `\Assets\amiga\common` folder (Any
    original 256/512/1024kB firmware ROM will work - no encrypted BIOSes yet).
    Currently `MegaAGS-Kickstart.rom` is the default rom

-   Make sure that the `\Assets\amiga\Mazamars312.Amiga\` Folder has the
    `mpu.bin` file - this is supplied and should already be there.

-   When changing system configurations, you must select the "CPU reset" in the
    menu to apply the config.

-   All harddrive and floppy images are to be placed into the
    `\Assets\amiga\common`

-   For Harddrive images - In the \\Cores\\Mazamars312.Amiga\\ folder the
    data.json file would need to be edited with the files names for each HDF file placed in data slots
    310-313. Currently at the moment the Amiga-Vision images are referenced here for now. But change the names for other harddrives
    These cannot be changed on the fly due to how the HDF images work in the core.
	These are also limited to 4GBytes in size due to the APF restrictions

How can I get AmigaVision running on this core?
-----------------------------------------------

For this you are required to have the following 3 files to place in to the
`\Assets\amiga\common` folder

-   The Kickstart ROM - MegaAGS-Kickstart.rom
-   The Main Hard drive image - MegaAGS-Pocket.hdf
-   The Save Hard drive image - MegaAGS-Saves.hdf

The Recommended setting in the interact menu are as follows:

-   FDD/HDD LED = Off
-   Port 1/Port 2 = Mouse/CD32PAD
-   Mouse Speed = 10
-   System Config = AGA/Turbo/PAL
-   CPU Type 68020K CPU - No Cache
-   memory - Chip = 2Mb
-   memory - Slow = None
-   memory - Fast = 32Mbytes
-   Audio Filter = A500/PWM

These can also be defaulted by selecting "Reset all to Default" in the interaction menu

Is the best way to play this using the dock?
--------------------------------------------

-   Yes, as you can then fully use the keyboard and mouse to interface with the
    core. Just make sure you select Port1/Port2 as Mouse/Joystick1 or Mouse/CD32
    when using the real mouse (The new APF firmware has been found to have a bug
    with the mouse plugged in - am awaiting for an update on this).

-   Also the mouse is emulated on the left analogue thumb stick at the same time
    as the joypad 1 is being used

-   Be aware that you will need a controller to access the Menu if you want to
    make changes.

-   There is a bug being addressed by Analogue on the output at this moment and we
    hope they can report back on what is causing the video to move over on
    random lines.
	
-   Have fixed up the Mouse input when a joystick was connected to the dock after
	has been release on the pocket firmware.

Menu Listing
------------

-   Load Bios - This will allow you to reload new BIOSes on the fly - will cause
    a reset.

-   Load Floppy Drive 0-1 - This will allow you to load different floppies while
    playing. Yes I had to drop this down as there was not enough space in the
    menus

-   CPU Reset - This will reset the core, and any change in settings will be
    applied.

-   FDD/HDD LED - this places a FDD or HDD activity light on the screen. The HDD
    light stays on some times unknown why yet.

-   Floppy Drives - this will allow you to select how many drives are
    installed, and delimit the transfer rate of the Amiga core.

-   Port 1/Port 2 - This allows you to change and swap the controller ports with
    CD32/Normal Joysticks/Mouse - if you need to use the emulated mouse, make
    sure you use the mouse/joystick or mouse/CD32 options.

-   Mouse Speed - This will change the update rate of both the emulated and real
    mouse movements.

-   Screen Movement X/Y - Moves the screen around.

-   System Config - Allows you to select PAL/NTSC refresh, OG (original)
    hardware or the AGA (Advanced Graphics Architecture) hardware, Turbo Boot or
    standard boot modes.

-   CPU Type - Allows you which type of CPU you want installed 68000, 68010,
    68020 with no cache, 68020 with cache.

-   Memory: Chip - Size of the Chip memory.

-   Memory: Slow - Size of the slow memory.

-   Memory: Fast - on the 68000 and 68010 CPU this is none/2/4/8MB, on the 68020
    this is 16 or 32.

-   Audio Filter - there are 4 options - A500 or A1200 filters with or without
    PWM

Is there an emulated mouse while not connected to the dock?
-----------------------------------------------------------

-   Yes, you can toggle this with the start button, and the D-pad becomes the
    movement, and the buttons are emulated on the left and right triggers.

-   Please note that you need to have the Port1/Port2 setup as Mouse/Joystick1
    or Mouse/CD32 for the mouse to work.

Is there an emulated Keyboard?
------------------------------

-   Yes, now there is! Press the Select button and a OSD keyboard will come
    up. There are no double pressed buttons yet.

Can we write to floppy disks at this moment?
--------------------------------------------

-   Yes you now can. So make sure you want to write to your images when they are
    on here!

Wait, you have another CPU in the FPGA?
---------------------------------------

-   Yes, the MPU is a VexRISCV CPU configured with a small footprint to
    interface with the APF interface to the HPS bus currently in the MiSTer
    Amiga core.

Specs: 

* Running at 74.2MHz (Same as the APF bus). 
* Connected to the core via a 16bit HPS Bus. 
* Two timers - one for interrupts and the other for user timing. 
* Currently, 128KB of memory/RAM for the MPU's program and buffers

How did you speed up load times
-------------------------------

-   Through talking to the Analogue Techs, it is faster to buffer more data and
    have that ready for the core to use then smaller 512 chunks that the
    original code would do.

Why are some of the resolutions not correct?
--------------------------------------------

-   Current the standard Amiga resolutions for both the OCS and AGS work. ECS
    will not at the max resolution of the Analogue Pocket is 800x720. Where ECS
    is 1024x200 or 1024x400

-   Due to memory access, the RTG resolutions will not work on this right now.
    They are also not used in the AmigaVision list of games

I have game X that does not work!
---------------------------------

-   A few games only work from ADF floppy images with the exact correct version of 
    Kickstart.

-   The main goal of this project was getting an MPU Framework so external media
    can be accessed, and help giving developers some tools to help create more
    cores on the Pocket.

-   I do expect that there is more to learn about the Amiga to get many of these
    bugs resolved.

-   For multi-disk games, also try putting disks in other drives. Make sure you
    change the amount of installed drives to the amount needed. (Up to 2)

-   Some games, like "Another World", do not work on the AmigaVision + Analogue
    Pocket right now. Report any bugs you find!

Amiga Vision does not start up and only a black screen is there
---------------------------------------------------------------

The Amiga Vision requires a 68020K CPU, AGS Chipset and extra memory to run. Make
sure you do a "Reset all to Default" to atleast get the core to boot.

Change since 0.1.0 to 0.1.2
---------------------------

-   Just getting the mouse working on the dock 

- 	Thanks to Analogue fixing up the line bug that was found with this core on the dock.

Change since 0.0.6 to 0.1.0
---------------------------

-   Hard Drive support for the Pocket

-   Proper 640x400/200 video outputs work!

-   Write to floppy images

-   More memory for the fast memory locations and tested working correctly.

-   Larger memory footprint for the MPU to hold all this awesomeness!!! 128KBYTE
    OF POWER!

-   A OSD Keyboard on the pocket when you are playing on pocket mode.

-   Null modem support using a Gameboy serial cable between two pockets! This
    could also open up MIDI support.

-   Also a emulated mouse on a analogue joystick in docked mode (This is the
    left thumb stick and the left and right triggers for the mouse clicks)

-   Dropped the floppy drives from 4 to 2 as there was not enough room in the interation menu
    at this moment.

Change since 0.0.5 to 0.0.6
---------------------------

-   Interlace mode corrected for games like DarkSeed.

-   Folder names changed for Unix/Linux/Mac OS and to help with the uploader
    system.

-   Faster floppy access times using a larger buffer in the MPU core and
    caching.

-   But what about the things below? I wanted to sort out these key issues out
    first and I'm awaiting a bugfix from Analogue to be released. This is due to
    files being corrupted when writing back to disk.

What will be the next update?
-----------------------------

-   Clock input for the Amiga to keep its time.

-   I have to find out why swapping disks will not always work.

-   Fix the bugs that cause some games not to boot with the
    [AmigaVision](https://amiga.vision/) configurations, e.g. Another World and
	the demo “Coda by Abyss” I believe this could be a memory issue. 
	However these do work on FDD images for now, if you want to play them.

Credits
-------

-   This source code is based on Rok Krajnc project (minimig-de1).

-   Original Minimig sources from Dennis van Weeren with updates by Jakub
    Bednarski are published on Google Code and the Community.

-   The [AmigaVision](https://amiga.vision/) team - Limi, Optiroc and hitm4n
    for their help and insight into the Amiga, and helping debug the Pocket at
    the same time with me.

-   ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang
    (minimig_tc64) and A.M. Robinson (minimig_tc64).

-   MiSTer project by Sorgelig (MiSTer) and the Community.

-   TG68K.C core by Tobias Gubener.

-   VexRISCV Team - https://github.com/SpinalHDL/VexRiscv

-   Boogerman for some coding questions - You still owe me on the use of my JTAG
    tho LOL.

-   Electron Ash, AGG23 as a beta tester.

-   Terminator2k2 for his fantastic images for the bootup.

-   And many more, so please message me and I would happily add you to this 
    if I forgot you!
