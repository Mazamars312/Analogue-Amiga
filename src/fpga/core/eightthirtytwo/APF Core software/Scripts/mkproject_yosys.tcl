set board [lindex $argv 1]
set project [lindex $argv 0]

set origin_dir "."
set orig_proj_dir "."

if {[string length $board]==0} {return -code error "Must specify a board"}
if {[string length $project]==0} {return -code error "Must specify a project"}

set corename "${project}"

source ../project_defs.tcl
source ${boardpath}/${board}/${board}_defs.tcl

puts $corename

if { ${requires_sdram}==1 && ${have_sdram}==0 } {
	puts "Board ${board} doesn't have SDRAM, skipping project generation for ${project}"
	exit
}

# Add source files to the project
source ${corename}_${board}_files.tcl

# Bring in any standard files or definitions required by the board.
source ${boardpath}/${board}/${board}_support.tcl

# Bring in project specific files
source top_files.tcl

proc post_message {msg} {
	puts $msg
}

# Run any pre-flow scripts
if {[info exists tcl_scripts]} {
	foreach {f} $tcl_scripts {
		puts "Run Tcl script $f"
		source $f
	}
}

# Parse HDL files
if {[info exists vhdl_files]} {
	foreach {f} $vhdl_files {
		puts "analyze $f"
		exec ghdl -a --ieee=synopsys -fexplicit $f
	}
}
if { [info exists vhdl_hierarchies] == 1 } {
	foreach {h} $vhdl_hierarchies {
		puts "hierarchy $h"
		yosys ghdl --ieee=synopsys -fexplicit $h
	}
}
if {[info exists verilog_files]} {
	foreach {f} $verilog_files {
		puts "verilog file $f"
		yosys read_verilog -sv $f
	}
}
yosys ghdl ${topmodule}
# Create .json file
yosys synth_ecp5 -abc9 -top ${topmodule} -json ${corename}.json
#yosys hierarchy -top ${topmodule}
#yosys proc
#yosys bugpoint -script ${boardpath}/${board}/test.ys -clean -grep "combinatorial loop"
#yosys write_rtlil result.il

