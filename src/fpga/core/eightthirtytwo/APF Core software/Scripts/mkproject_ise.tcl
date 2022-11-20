package require cmdline
variable ::argv0
set options {
   { "project.arg" "" "Project name" }
   { "board.arg" "" "Target board" }
}
set usage "You need to specify options and values"
array set optshash [::cmdline::getoptions ::argv $options $usage]
set board $optshash(board)
set project $optshash(project)

if {[string length $board]==0} {return -code error "Must specify a board"}
if {[string length $project]==0} {return -code error "Must specify a project"}

set corename "${project}_${board}"

source ../../project_defs.tcl
source ${boardpath}/${board}/${board}_defs.tcl

if { ${requires_sdram}==0 || ${have_sdram}==1 } {
	project open ${corename}.xise
	source ${boardpath}/${board}/${board}_support.tcl
	source ${corename}_files.tcl
	xfile add ${boardpath}../PLL/${fpga}_${base_clock}_${target_frequency}/pll.${pll_extension}
	if ${requires_sdram}==1 {
		xfile add ${boardpath}/${board}/sdram_iobs.ucf
	} else {
		puts "Project doesn't require SDRAM, omitting sdram_iobs.ucf"
	}
	project save
	project close
} else {
	puts "Board ${board} has no SDRAM, not building ${project}"
}

