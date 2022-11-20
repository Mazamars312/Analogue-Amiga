# qsf to qip

# Script to assist in DeMiSTifying MiST cores.
# Copyright 2021 by Alastair M. Robinson
# Distributed under the terms of the GNU General Public License V3
# or later, see the file "Copying" for more details.

# Execute from the root of your DeMiSTified project like so:
# > quartus_sh -t DeMiSTify/Scripts/qsftoqip.tcl -qsf mist_core.qsf -qip project.qip

# Then edit the QIP file to make any necessary adjustments (usually removing any PLLs
# which will have to be regenerated in board-specific form.)

# FIXME - deal with quoted paths in the qsf file.

proc outputentry {outfile adjpath type entry} {
	if {[string length $adjpath] > 0} {
		if {[string match "../*" $entry]} {
			set entry [string trimleft $entry "../"]
		} else {
			set entry $adjpath/$entry
		}
	}
	puts $outfile "set_global_assignment -name $type \[file join \$::quartus(qip_path) $entry\]"
}

package require cmdline

variable ::argv0 $::quartus(args)

set options {
   { "qsf.arg" "" ".qsf file" }
   { "qip.arg" "" ".qip file" }
}
set usage "You need to specify options and values"
array set optshash [::cmdline::getoptions ::argv $options $usage]
set qsffile $optshash(qsf)
set qipfile $optshash(qip)

if {[string length $qsffile]==0} {return -code error "Must specify a project (.qsf) file"}
if {[string length $qipfile]==0} {return -code error "Must specify an output (.qip) file"}

set slashidx [ string first "/" $qsffile]
if {$slashidx>-1} {set adjpath [string range $qsffile 0 [expr $slashidx - 1]]} else {set adjpath ""}
set slashidx [ string first "\\" $qsffile]
if {$slashidx>-1} {set adjpath [string range $qsffile 0 [expr $slashidx - 1]]}

set fp [open $qsffile r]
set file_data [read $fp]
close $fp

set outfile [open $qipfile w]

set data [split $file_data "\n"]

# Extract filenames of RTL files
set preflowfound 0
foreach line $data {
	set parts [split $line]
	if {[string equal [lindex $parts 2] VHDL_FILE] == 1 } { outputentry $outfile $adjpath [lindex $parts 2] [lindex $parts 3] }
	if {[string equal [lindex $parts 2] VERILOG_FILE] == 1 } { outputentry $outfile $adjpath [lindex $parts 2] [lindex $parts 3] }
	if {[string equal [lindex $parts 2] SYSTEMVERILOG_FILE] == 1 } { outputentry $outfile $adjpath [lindex $parts 2] [lindex $parts 3] }
	if {[string equal [lindex $parts 2] QIP_FILE] == 1 } { outputentry $outfile $adjpath [lindex $parts 2] [lindex $parts 3] }
	if {[string equal [lindex $parts 2] SDC_FILE] == 1 } { outputentry $outfile $adjpath [lindex $parts 2] [lindex $parts 3] }
	if {[string equal [lindex $parts 2] PRE_FLOW_SCRIPT_FILE] == 1 } {
		if { $preflowfound == 0 } { puts "Pre-flow scripts found:" }
		puts [lindex $parts 3]
	}
}

