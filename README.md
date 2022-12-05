# Analogue Pocket - Commodore Amiga 500 - 0.0.5 Alpha

The Commodore Amiga 500 was a personal computer that replaced the well loved Commodore C64 in Commodores product line. It also was one of the many 68K personal computers that showed that this CPU was a power house when intergrated with great hardware beside it.

This is powered from the Mister line of the Amiga Mist project (Also known as the minimig) and the VexRISCV RISC-v chip for the Media Processing Unit (MPU) between the core and APF framework for floppy, harddrive and CDrom access.

## What can it do?
* Both 68K and 68020 CPUs
* Chip memory 512k, 1m, 1.5m and 2m
* Slow memory none, 512k, 1m, 1.5m
* Fast memory of none/2/4/8Mbyte for the 68000K CPU
* Fast memory of 16/32Mbyte for the 68020K CPU setup - Another 32Mbytes will be added soon
* AGA Chipset
* Turbo boot
* Right now it can only read ADF disk images (Floppy disks). Writes will come soon.

## How to setup
* 
* Place your adf floppy images in the \asset\amiga\common folder
* Place the kickstart rom in the \asset\amiga\common folder (Im using the 1.2 for now but any 256K rom will work). name it to kickstart.rom to autoload it
* Make sure that the \asset\amiga\Mazamars312.amiga\ Folder has the mpu.bin file - this is supplied and should already be in there.
* When changing system configuations, you will need to select the "CPU reset" to apply the config.

## Is the best way to play this using the dock?

* Yes as you can then fully use the Keyboard and mouse to interface many games and functions. Just make sure you select in the Port1/Port2 as Mouse/Joystick1 or Mouse/CD32 for mouse access

## Is there a emulated mouse while not connected to the dock?

* Yes, Press both the left and right triggers and the dpad becomes the movement and the buttons are emulated on buttons A,B,X. However they are the opersite way arround (So Right to left). For me, it just felt better for the A button to be the left mouse button while off the dock. 
* Please note that you have to have the Port1/Port2 setup as Mouse/Joystick1 or Mouse/CD32 for the mouse to work

## Is there a emulated Keyboard?

* Not at this moment, This is up for the next build as I have not desided on the best way to do this yet.

## Can we write to floppy disks at this moment?

* No, I have the code ready but I want to check it before releasing it.

## When is writable disks and Harddrives going to be working?

* Now that the framework for the MPU is sorted, this is a matter of getting the C++ code working with the MPU and APF interface now.

## Wait you have another CPU in the FPGA???

* Yes the MPU is a VexRISCV CPU that has been configured with a small footprint to interface with the APF interface to the HPS bus that is currently in the Amiga core.

Specs:
* Running at 74.2mhz (Same as the APF bus)
* Connected to the core via a  16bit HPS Bus 
* Two timers - one for interrupts and the other for user access.
* Currently 32K of memory/ram for the MPU's program (This will be dropped to 16K for a smaller footprint)
* A 2Kbyte swap buffer for the both the APF and MPU to do transfers 

## I have selected a disk and now its taking some time to load?

* So it could be due to the game being loaded still. thats what that nice green indercater at the top left hand side is for :-)
* Try only using one disk drive - I have found that some games dont like having more then one floppy drive.
* Or you can use the UART interface on the Dev Cart (Bard rate of 115200) to see what the MPU is doing with the debug logs. Analogue Dev Units have this card only at this moment.
* A double buffer will be added on the next update to help speed up this process.

## Why are some of the resolutions not correct?

* I need to see what are the comman resolutions to then make sure the scaler will work correctly - Interlacing is one thing I know I need to build up.
* The RTG Resolutions will not work on this right now due to memory access at this moment.

## I have game X that does not work!!!

* I would not be suprised as from the testing I have been doing there are multible configuations, bios and even ADF Images that just would not completely work. So 
* The main goal of this project was getting a MPU Framework so external media can be accessed and gives devolopers some tools to help create more cores on the Pocket.
* There will be more bug fixes and functions added to this core and MPU.

## Why not use the CHIP32?

* At this moment the CHIP32 that Analogue provides is not fast enough for doing both Media access and Keyboard and Mouse access. 
* Another big thing for myself was building C or C++ code to easily create and modify code for media transfers and time frames in dev time.
* However, an idea of using it for setting up larger systems like the Amiga 1200, 2000 and 4000 cores when I can look into thos

## Credits

* This sourcecode is based on Rok Krajnc project (minimig-de1).
* Original minimig sources from Dennis van Weeren with updates by Jakub Bednarski are published on Google Code and the Community.
* ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang (minimig_tc64) and A.M. Robinson (minimig_tc64).
* MiSTer project by Sorgelig (MiSTer) and the Community.
* TG68K.C core by Tobias Gubener.
* VexRISCV Team - https://github.com/SpinalHDL/VexRiscv
* Robinsonb5 for his time answering my questions on his 832 CPU - Tho I did not use it, I would of love too as is was a nice looking core
* Boogerman for some coding questions - You still owe me on the use of my jtag tho LOL
* Electron Ash, AGG23 as a beta testers.
* Terminator2k2 for his fantastic images for the boot up
* And many more!!
