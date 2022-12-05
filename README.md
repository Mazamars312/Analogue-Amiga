# Commodore Amiga 500

The Commodore Amiga 500 was a personal computer that replaced the well loved Commodore C64 in Commodores product line. It also was one of the many 68K personal computers that showed that this CPU was a power house when intergrated with great hardware beside it.

This is powered from the Mister line of the Amiga Mist project (Also known as the minimig) and the picorv32 RISC-v chip for the media arbitor between the core and APF framework for floppy, harddrive and CDrom access.

## What can it do?
* Both 68K and 68020 CPUs added
* Chip memory 512k, 1m, 1.5m and 2m
* Slow memory none, 512k, 1m, 1.5m
* Fast memory of none/2/4/8Mbyte for the 68000K CPU
* Fast memory of 16/32Mbyte for the 68020K CPU setup
* AGA Chipset
* Turbo boot
* Right now it can only read ADF disk images (Floppy disks). Writes will come soon.

## How to setup
* Place your adf floppy images in the \asset\amiga\common folder
* Place the kickstart rom in the \asset\amiga\common folder (Im using the 1.2 for now but any 256K rom will work). name it to kickstart.rom to autoload it
* Make sure that the \asset\amiga\Mazamars312.amiga\ Folder has the mpu.bin file

## Can I use a emulated mouse while not connected to the dock?

* Yes, Press both the left and right triggers and the dpad becomes the movement and the buttons are emulated on buttons A,B,X. However they are the opersite way (So Right to left) It just felt better for the B button to be the Left button. 
* Please note that you have to have the Port1/Port2 setup as Mouse/Joystick1 or Mouse/CD32 for the mouse to work

## Can we write to disks at this moment?

* No I have the code in there ready, but I want to check it before releasing it.

## When is writable disks and Harddrives going to be working?

* Now that the framework for the MPU is sorted, this is a matter of getting the C++ code working with the MPU and APF interface now. So this is easier to sort out and create faster access as well

## Wait you have another CPU in the FPGA???

* Yes the MPU is a VexRISCV CPU that has been configured with a small footprint to interface with the APF interface to the HPS bus that is currently in the Amiga core.
* This will help with 

Specs:
* Running at 74.2mhz (Same as the APF bus)
* Has the HPS Bus interface 
* Two timers - one for interrupts and the other for user access.
* Currently 32K of memory/ram for the MPU's program (This will be dropped to 16K for size)
* A 2Kbyte swap buffer for the both the APF and MPU to do transfers 

## I have selected a disk and now its taking some time to load?

* So It could be due to the game being loaded still or the game has crashed. Try another bios or the ADF image you are using is not usable at this moment.
* Try only using one disk drive - I have found that some games dont like having more then one drive controller.
* Or you can use the UART interface on the Cart (Bard rate of 115200) to see what the MPU is doing with the debug logs.

## Credits

* This sourcecode is based on Rok Krajnc project (minimig-de1).
* Original minimig sources from Dennis van Weeren with updates by Jakub Bednarski are published on Google Code and the Community.
* ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang (minimig_tc64) and A.M. Robinson (minimig_tc64).
* MiSTer project by Sorgelig (MiSTer) and the Community.
* TG68K.C core by Tobias Gubener.
* YosysHQ for their picorv32 CPU.
* Robinsonb5 for his time answering my questions on his 832 CPU - Tho I did not use it, I would of love too as is was a nice looking core
* Boogerman for some coding questions - You still owe me on the use of my jtag tho LOL
* Electron Ash as a beta tester.
