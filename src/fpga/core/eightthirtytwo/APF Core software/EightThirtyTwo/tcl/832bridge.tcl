#!/opt/intelFPGA_lite/18.1/quartus/bin/quartus_stp -t

#   832bridge.tcl - Virtual JTAG proxy for Altera devices
#
#   Description: Create a TCP connection to communicate between
#   the 832 Debugger and the Debug module of a running 832 core.
#   This TCL script requires Quartus STP to be installed.
#  
#   Based on Binary Logic's vjtag example code.
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

###################### Parameters ###########################

set service_port 33581
set listen_address 127.0.0.1

###################### Code #################################

if {[info exists ::quartus]} {
	source [file dirname [info script]]/vjtagutil.tcl
} else {
	source [file dirname [info script]]/vjtagutil_xilinx.tcl
	set cable "xpc"
	if {$argc > 0 } {
		set cable [lindex $argv 0]
	}
	vjtag::select_cable $cable
	if { [catch {vjtag::usbblaster_open}] } {
		puts "Unable to open cable $cable"
		exit 1
	}
}

########## Process a connection on the port ###########################

proc conn {channel_name client_address client_port} {
	global service_port
	global listen_address
	global wait_connection

	# Configure the channel for binary
	fconfigure $channel_name -translation binary -buffering none -blocking true

	puts "Connection from $client_address"
	flush $channel_name

	set portopen 0

	while {1} {
		# Try to read a character from the buffer
		set cmd [read $channel_name 1]
		set parambytes [read $channel_name 1]
		set responsebytes [read $channel_name 1]
		set byteparam [read $channel_name 1]
		if {[eof $channel_name]} {
			break
		}
		set numchars [string length $byteparam]


		# Did we receive something?
		if { $numchars > 0 } {

			scan $cmd %c ascii

			if { $ascii == 255} {
				puts -nonewline $channel_name [format %c $portopen]
				# Release the USB blaster again
				if { $portopen == 1 } { vjtag::usbblaster_close  }
				set portopen 0
			}
			
			if { $ascii != 255} {
#				puts [vjtag_dec2bin $ascii]
				if { $portopen == 0 } {
					if [ vjtag::usbblaster_open ] { set portopen 1 }
				}

				set cmd [expr $ascii << 24]
				scan $parambytes %c parambytes
				set cmd [expr $cmd | [expr $parambytes << 16]]
				scan $responsebytes %c responsebytes
				set cmd [expr $cmd | [expr $responsebytes << 8]]
				scan $byteparam %c byteparam
				set cmd [expr $cmd | $byteparam]
				vjtag::send $cmd

				# Sending parameters, if any

				set cmd 0
				while { $parambytes > 0 } {
					set byteparam [read $channel_name 1]
					scan $byteparam %c ascii
					set cmd [expr [expr $cmd << 8] | $ascii]
					set parambytes [expr $parambytes - 1]
					if {$parambytes==0 || $parambytes==4} {
#						puts "Sending param..."
						vjtag::send $cmd
						set cmd 0
					}
				}

				while { $responsebytes > 0 } {
#					puts "Waiting for response"
					if {[eof $channel_name]} break
					set rx [vjtag::recv_blocking]
					puts -nonewline $channel_name [format %c [expr [expr $rx >> 24] & 255]]
					puts -nonewline $channel_name [format %c [expr [expr $rx >> 16] & 255]]
					puts -nonewline $channel_name [format %c [expr [expr $rx >> 8] & 255]]
					puts -nonewline $channel_name [format %c [expr $rx & 255]]
					set responsebytes [expr $responsebytes -4]
				}
	#				puts "done"
			}
		}
	}
	if { $portopen == 1 } {
		vjtag::usbblaster_close
		set portopen 0
		puts "port closed"
	}
	close $channel_name
	puts "\nClosed connection"


	set wait_connection 1
}


####################### Main code ###################################

global wait_connection

# Find the USB Blaster
if {[vjtag::select_instance 0x832d] > -1} {
	# Start the server socket
	socket -server conn -myaddr $listen_address $service_port

	# Loop forever
	while {1} {

		# Set the exit variable to 0
		set wait_connection 0
	
		# Display welcome message
		puts "832bridge listening for 832ocd connections on $listen_address:$service_port"

		# Wait for the connection to exit
		vwait wait_connection 
	}
} else {
	puts "Can't find virtual jtag instance"
}

##################### End Code ########################################

