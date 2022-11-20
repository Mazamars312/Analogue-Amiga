# xxxxx DeMiSTified - Atlas CYC1000 port

xx/xx/22 Atlas CYC1000 port DeMiSTified by Somhic from original MiST xxxxxxx https://github.com/mist-devel/xxxxxxxxxxx
xx/xx/22  Atlas CYC1000 port by Somhic from previous Demistifyied MiST core https://github.com/xxxx by @xxx   

[Read this guide if you want to know how I DeMiSTified this core](https://github.com/DECAfpga/DECA_board/tree/main/Tutorials/DeMiSTify).

**Features:**

* ~~HDMI video output~~
* VGA 222 video output is available through an HDMI to VGA adapter
* HDMI audio output
* Audio Sigma-Delta output
* ~~Audio output (Midi, I2S)~~
* Joystick (tested with a Megadrive gamepad)

**Additional hardware required**:

* PS/2 keyboard 
* ~~USB keyboard~~ 

##### Versions:

v0.x Changes in Mist core to adapt DeMiSTify to both Deca and new board Atlas_cyc

### STATUS

* VGA version only at the moment.
* ~~HDMI video outputs special resolution so will not work on all monitors.~~ 



### Instructions to compile the project for a specific board:

(Note that sof/rbf files are already included in /atlas_cyc/output_files/)

```sh
git clone https://github.com/DECAfpga/xxxxxx
cd xxxxx
#Do a first make (will finish in error) but it will download missing submodules 
make
cd DeMiSTify
#Create file site.mk in DeMiSTify folder 
cp site.template site.mk
#Edit site.mk and add your own PATHs to Quartus (Q18)
gedit site.mk
#Go back to root folder and do a make with board target (deca, neptuno, uareloaded, atlas_cyc, ...). If not specified it will compile for all targets.
cd ..
make BOARD=atlas_cyc
#when asked just accept default settings with Enter key
```

After that you can:

* Flash bitstream directly from [command line](https://github.com/DECAfpga/DECA_binaries#flash-bitstream-to-fgpa-with-quartus)
* Load project in Quartus from /atlas_cyc/xxxxxxxxx_atlas_cyc.qpf

**Others:**

* User Button is a reset button

### OSD Controls

* F12 show/hide OSD 
* Long F12 toggles VGA/RGB mode
* The reset button KEY0 resets the controller (so re-initialises the SD card if it's been changed, reloads any autoboot ROM.) The OSD Reset menu item resets the core itself.

