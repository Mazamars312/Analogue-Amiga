# Script to mimic the Altera-virtual-JTAG-based interface used by the
# 832 Debug interface.

# JTAG facilities are provided by libtcljtag, a Tcl extension based upon
# xc3sprog.  The libtcljtag.so file must be in the tcljtag directory

load [file dirname [info script]]/../tcljtag/libtcljtag.so

namespace eval vjtag {
	variable instance
	variable devicecount
	variable usbblaster_name
	variable usbblaster_device
	variable device_description
	variable ir_reg
	variable dr_reg
}

proc vjtag::select_cable { cable } {
	set ::vjtag::usbblaster_name $cable
	set ::vjtag::usbblaster_device 0
	set ::vjtag::devicecount 0
}

proc vjtag::select_device { dev } {
	set ::vjtag::usbblaster_device $dev
	set ::vjtag::device_description [jtag::get_device_description $dev]
#	puts "Device type is $::vjtag::device_description"
	if { [string match "XC3*" $::vjtag::device_description] } {
		# Spartan 3 series - use USER1 (2) and USER2 (3) registers for the virtual IR and DR, respectively
		set ::vjtag::ir_reg 2
		set ::vjtag::dr_reg 3
	} elseif { [string match "XC6*" $::vjtag::device_description] } { 
		# 6 series - use USER3 (0x1A) and USER4 (0x1B) registers
		set ::vjtag::ir_reg 0x1a
		set ::vjtag::dr_reg 0x1b
	} elseif { [string match "XC7*" $::vjtag::device_description] } { 
		# 7 series - use USER3 (0x22) and USER4 (0x23) registers
		set ::vjtag::ir_reg 0x22
		set ::vjtag::dr_reg 0x23
	} elseif { [string match "XA7*" $::vjtag::device_description] } { 
		# 7 series - use USER3 (0x22) and USER4 (0x23) registers
		set ::vjtag::ir_reg 0x22
		set ::vjtag::dr_reg 0x23
	} else {
		puts "Device $::vjtag::device_description not yet supported"
		throw { NONE } { Device $::vjtag::device_description not yet supported }
	}
	jtag::select_device $dev
}

proc vjtag::usbblaster_open {} {
	set ::vjtag::devicecount [jtag::open_cable $::vjtag::usbblaster_name]
	vjtag::select_device $::vjtag::usbblaster_device
	return 1
}

proc vjtag::usbblaster_close {} {
	jtag::close_cable
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

proc vjtag::count_instances { id } {
	return [ expr [vjtag::ident $id] ]
}

proc vjtag::select_instance { id } {
	set ::vjtag::instance -1

	if { [vjtag::count_instances $id] >0 } {
		set ::vjtag::instance 0
	}
	return $::vjtag::instance
}

proc vjtag::ir {a} {
	# BSCANE2 on USER3 register
#	jtag::shift_ir 0x22
	jtag::shift_ir $::vjtag::ir_reg
	jtag::shift_dr $a 2
}

proc vjtag::dr {a} {
	# BSCANE2 on USER4 register
	jtag::shift_ir $::vjtag::dr_reg
#	puts [ format 0x%x $a]
	return [jtag::shift_dr $a 32]
}

proc vjtag::ident {a} {
	vjtag::ir 0x02
	set r [vjtag::dr 0]
	puts [format 0x%x $r]
	set r [expr $r>>16]
	if {[expr $r & 65535] == $a} {
		puts "Ident found"
		return 1
	}
	puts "Ident not found"
	return 0
}

proc vjtag::recv {} {
	vjtag::ir 0x02
	if {[expr [vjtag::dr 0] & 1]} {
		return -1
	}
	vjtag::ir 0x00
	return [vjtag::dr 0]
}

proc vjtag::recv_blocking {} {
	vjtag::ir 0x02
	while {[expr [vjtag::dr 0] & 1]} {
	}
	vjtag::ir 0x00
	return [vjtag::dr 0]
}

proc vjtag::send {a} {
	vjtag::ir 0x02
	while {[expr [vjtag::dr 0] & 8]} {
		puts "Fifo full"
		after 100
	}
	vjtag::ir 0x01
	vjtag::dr $a
}

proc vjtag::stat {} {
	vjtag::ir 0x02
	set r [vjtag::dr 0x00]
#	puts [format 0x%x $r]	
	if {[expr $r & 1]} { puts "Receive buffer empty" }
	if {[expr $r & 2]} { puts "Receive buffer full" }
	if {[expr $r & 4]} { puts "Transmit buffer empty" }
	if {[expr $r & 8]} { puts "Transmit buffer full" }
}

