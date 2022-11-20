#   Utility functions for communicating with an Altera/Intel
#   Virtual JTAG instance over USB-Blaster
#  
#   Loosely based on Binary Logic's vjtag example code.
#   
#   This file is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#   
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

package require Tk

init_tk

namespace eval vjtag {
	variable instance
	variable usbblaster_name
	variable usbblaster_device
}

proc vjtag::match_blaster { blaster } {
	if { [string match "USB-Blaster*" $blaster] ||
	 [string match "*SoC*" $blaster] ||
	 [string match "*MAX*" $blaster] } {
		return 1
	}
	return 0
}

proc vjtag::match_devicename { devname } {
	if { [string match "*: EP*C*" $devname] } {
		return 1
	}
	if { [string match "*: 10CL*" $devname] } {
		return 1
	}
	if { [string match "*: 10M*" $devname] } {
		return 1
	}
	if { [string match "*: 5CSE*" $devname] } {
		return 1
	}
	if { [string match "*: 5CE*" $devname] } {
		return 1
	}
	return 0
}


# Find VJTAG instance on the current USB Blaster, matching the supplied ID
# Returns -1 if none found.

proc vjtag::findinstance {targetid {skip 0}} {
	set res 1
	if [ catch { open_device -hardware_name $::vjtag::usbblaster_name -device_name $::vjtag::usbblaster_device } ] { set res 0 }
	catch { device_lock -timeout 10000 }

	set instance -1
	while {$res == 1} {
		# Set instance to status mode, read status, return instance to bypass mode
		set instance [expr $instance + 1]
		if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 2 -no_captured_ir_value } ] {
			set res 0
			set instance -1
		}
		if { $res } {
			set id [device_virtual_dr_shift -dr_value 00000000000000000000000000000000 -instance_index $instance -length 32]
			device_virtual_ir_shift -instance_index $instance -ir_value 3 -no_captured_ir_value

			set id [vjtag::bin2dec $id]
			set id [expr $id >> 16]
			set idhex [format %x $id]
			if {$id==$targetid} {
				if {$skip} {
					set skip [expr skip -1]
				} else {
					set res 0
				}
			}
		}
	}
	catch {device_unlock}
	catch {close_device}
	return $instance
}


# Loop through the connected USB Blasters connected to the system, looking for a VJTAG instance 
# matching the supplied ID.
# Returns -1 if none found.

proc vjtag::usbblaster_findinstance { instanceid {skip 0} } {
	set blastercount 0
	foreach hardware_name [get_hardware_names] {
		if { [match_blaster $hardware_name] } {
			set ::vjtag::usbblaster_name $hardware_name
			set devicecount 0
			foreach device_name [get_device_names -hardware_name $::vjtag::usbblaster_name] {
				if { [match_devicename $device_name] } {
					set ::vjtag::usbblaster_device $device_name
					set ::vjtag::instance [vjtag::findinstance $instanceid]
					if {$::vjtag::instance > -1} {
						if {$skip} {
							set skip [expr $skip - 1]
						} else {
							return $::vjtag::instance
						}
					}
				}
				set devicecount [expr $devicecount + 1]
			}
			set blastercount [expr $blastercount + 1]
		}
	}
	return -1
}


# Setup connection - automatic if there's no ambiguity, prompts the user if
# more than one JTAG interface and/or more than one recognised chip is found.

proc vjtag::usbblaster_setup {} {
	# List all USB-Blasters connected to the system.  If there's only one
	# use that, otherwise prompt the user to choose one.
	set ::vjtag::instance -1

	set count 1
	foreach hardware_name [get_hardware_names] {
		if { [match_blaster $hardware_name] } {
			puts "Device $count: $hardware_name"
			set ::vjtag::usbblaster_name $hardware_name
			set count [expr $count + 1]
		}
	}

	if {$count==1} {
		puts "No JTAG interfaces found"
		exit
	}
	if {$count!=2} {
		puts "More than one USB-Blaster found - please select a device."
		gets stdin id
		scan $id "%d" idno
		set count 1
		foreach hardware_name [get_hardware_names] {
			if { [match_blaster $hardware_name] } {
				if { $count == $idno } {
					puts "Selected $hardware_name"
					set ::vjtag::usbblaster_name $hardware_name
				}
				set count [expr $count + 1]
			}
		}
	}

	# List all devices on the chain.  If there's only one, select that,
	# otherwise prompt the user to select one.

	puts "Devices on JTAG chain:";
	set count 0
	foreach device_name [get_device_names -hardware_name $::vjtag::usbblaster_name] {
		puts "Candidate: $device_name"
		if { [match_devicename $device_name] } {
			set ::vjtag::usbblaster_device $device_name
			set count [expr $count + 1]
		}
	}

	if {$count!=1} {
		puts "Please select a device.";
		gets stdin id
		foreach device_name [get_device_names -hardware_name $::vjtag::usbblaster_name] {
			if { [string match "@$id*" $device_name] } {
				set ::vjtag::usbblaster_device $device_name
			}
		}
	}
	puts "Selected $::vjtag::usbblaster_device.";
}


# Open device 
# If you want to talk to more than one instanc
proc vjtag::usbblaster_open {{instance ""}} {
	set res 1
	if {$instance==""} { set instance $::vjtag::instance }
	if [ catch { open_device -hardware_name $::vjtag::usbblaster_name -device_name $::vjtag::usbblaster_device } ] { set res 0 }

	# Set IR to 3, which is bypass mode - which has the side-effect of verifying that there's a suitable JTAG instance.
	catch { device_lock -timeout 10000 }
	if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 3 -no_captured_ir_value } ] {
		set res 0
		catch {device_unlock}
		catch {close_device}
	}
	return $res
}


# Close device.
proc vjtag::usbblaster_close {{instance ""}} {
	# Set IR back to 0, which is bypass mode
	if {$instance==""} { set instance $::vjtag::instance }
	catch { device_virtual_ir_shift -instance_index $instance -ir_value 3 -no_captured_ir_value }

	catch {device_unlock}
	catch {close_device}
}


# Convert decimal number to the required binary code
proc vjtag::dec2bin {i {width {}}} {

    set res {}
    if {$i<0} {
        set sign -
        set i [expr {abs($i)}]
    } else {
        set sign {}
    }
    while {$i>0} {
        set res [expr {$i%2}]$res
        set i [expr {$i/2}]
    }
    if {$res == {}} {set res 0}

    if {$width != {}} {
        append d [string repeat 0 $width] $res
        set res [string range $d [string length $res] end]
    }
    return $sign$res
}

# Convert a binary string to a decimal/ascii number
proc vjtag::bin2dec {bin} {
    if {$bin == 0} {
        return 0
    } elseif {[string match -* $bin]} {
        set sign -
        set bin [string range $bin[set bin {}] 1 end]
    } else {
        set sign {}
    }
    return $sign[expr 0b$bin]
}

# Send data to the Altera input FIFO buffer
proc vjtag::send {chr {instance ""}} {
	if {$instance==""} { set instance $::vjtag::instance }
	if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 1 -no_captured_ir_value } ] { return -1 }
	device_virtual_dr_shift -dr_value [vjtag::dec2bin $chr 32] -instance_index $instance  -length 32 -no_captured_dr_value
	return 1
}

# Read data in from the Altera output FIFO buffer
proc vjtag::recv {{instance ""}} {
	if {$instance==""} { set instance $::vjtag::instance }
	# Check if there is anything to read
	if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 2 -no_captured_ir_value } ] { return -1 }
	set tdi [device_virtual_dr_shift -dr_value 0000 -instance_index $instance -length 4]
	if {![expr $tdi & 1]} {
		device_virtual_ir_shift -instance_index $instance -ir_value 0 -no_captured_ir_value
		set tdi [device_virtual_dr_shift -dr_value 00000000000000000000000000000000 -instance_index $instance -length 32]
		return [vjtag::bin2dec $tdi]
	} else {
		return -1
	}
}

# Read data in from the Altera output FIFO buffer
proc vjtag::recv_blocking {{instance ""}} {
	if {$instance==""} { set instance $::vjtag::instance }
	while {1} {
		if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 2 -no_captured_ir_value } ] { return [ vjtag_bin2dec 0 ] }
		set tdi [device_virtual_dr_shift -dr_value 0000 -instance_index $instance -length 4]
		if {![expr $tdi & 1]} {
			device_virtual_ir_shift -instance_index $instance -ir_value 0 -no_captured_ir_value
			set tdi [device_virtual_dr_shift -dr_value 00000000000000000000000000000000 -instance_index $instance -length 32]
			return [vjtag::bin2dec $tdi]
		}
	}
}


# List VJTAG instances found within a particular chip

proc vjtag::listinstances { } {
	if [ catch { open_device -hardware_name $::vjtag::usbblaster_name -device_name $::vjtag::usbblaster_device } ] { set res 0 }
	catch { device_lock -timeout 10000 }

	set instance 0
	set res 1
	while {$res == 1} {
		# Set instance to status mode, read status, return instance to bypass mode
		if [ catch { device_virtual_ir_shift -instance_index $instance -ir_value 2 -no_captured_ir_value } ] {
			set res 0
		} else {
			set id [device_virtual_dr_shift -dr_value 00000000000000000000000000000000 -instance_index $instance -length 32]
			device_virtual_ir_shift -instance_index $instance -ir_value 3 -no_captured_ir_value

			set id [vjtag::bin2dec $id]
			set id [expr $id >> 16]
			set idhex [format %x $id]
			puts "Instance $instance - ID : 0x$idhex" 
			set instance [expr $instance + 1]
		}
	}
	catch {device_unlock}
	catch {close_device}
}


proc vjtag::count_instances { id } {
	set instancecount 0
	set instance $::vjtag::instance
	while { [vjtag::usbblaster_findinstance $id $instancecount] >-1} {
		incr instancecount
	}
	set $::vjtag::instance $instance
	return $instancecount
}


# Select a VJTAG instance, based on ID.  All connected JTAG cables and chips will be searched.
# If only one option is found it will be used.  If multiple options are found, the user
# will be presented with a dialog, to choose the appropriate instance.

proc vjtag::select_instance { id } {
	global usbblaster_name
	global usbblaster_device
	global vjtag_select_instance_idx
	set connected 0
	set ::vjtag::instance -1

	if { [vjtag::count_instances $id] <2 } {
		return [vjtag::usbblaster_findinstance $id 0]
	}

	toplevel .dlg
	wm state .dlg normal
	wm title .dlg "Connect to..."

	listbox .dlg.lb -selectmode single -width 78
	grid .dlg.lb -in .dlg -row 1 -column 1 -sticky ew -padx 5 -pady 2

	set idx 0
	set instance [vjtag::usbblaster_findinstance $id $idx]
	while {$instance>-1} {
		.dlg.lb insert $idx "$::vjtag::usbblaster_name / $vjtag::usbblaster_device / $instance"
		incr idx
		set instance [vjtag::usbblaster_findinstance $id $idx]
	}
	set ::vjtag::instance 0
	bind .dlg.lb {<<ListboxSelect>>} {set vjtag_select_instance_idx [.dlg.lb curselection]}
	button .dlg.bt -text "Connect" -command [list destroy .dlg]
	grid .dlg.bt -in .dlg -row 2 -column 1 -sticky ew -padx 5 -pady 2

	tkwait window .dlg
	vjtag::usbblaster_findinstance $id $vjtag_select_instance_idx
	return $::vjtag::instance
}

