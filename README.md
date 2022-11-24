# Commodore Amiga 500

The Commodore Amiga 500 was a personal computer that replaced the well loved Commodore C64 in Commodores product line. It also was one of the many 68K personal computers that showed that this CPU was a power house when intergrated with great hardware beside it.

This is powered from the Mister line of the Amiga Mist project (Also known as the minimig) and the picorv32 RISC-v chip for the media arbitor between the core and APF framework for floopy, harddrive and CDrom access.

## What can it do?
* Both 68K and 68020 CPUs added
* Chip memory 512k, 1m, 1.5m and 2m
* Slow memory none, 512k, 1m, 1.5m
* Fast memory will be added later on
* AGA Chipset
* Turbo boot
* Watch the pritty colors
* Right now it can only access ADF disk images (Floppy disks)

## How to setup
* Place your adf floppy images in the /asset/amiga/common folder
* Place the kickstart rom in the /asset/amiga/common folder (Im using the 1.2 for now but any 256K rom will work). name it to kickstart.rom to autoload it


## Credits

* This sourcecode is based on Rok Krajnc project (minimig-de1).
* Original minimig sources from Dennis van Weeren with updates by Jakub Bednarski are published on Google Code.
* ARM firmware updates and minimig-tc64 port changes by Christian Vogelsang (minimig_tc64) and A.M. Robinson (minimig_tc64).
* MiSTer project by Sorgelig (MiSTer).
* TG68K.C core by Tobias Gubener.
* YosysHQ for their picorv32 CPU
* Robinsonb5 for his time answering my questions on his 832 CPU - Tho I did not use it, I would of love too as is was a nice looking core
* Boogerman for some coding questions - You still owe me on the use of my jtag tho LOL
