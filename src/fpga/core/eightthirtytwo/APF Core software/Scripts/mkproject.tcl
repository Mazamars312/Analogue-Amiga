load_package flow

package require cmdline
variable ::argv0 $::quartus(args)
set options {
   { "project.arg" "" "Project name" }
   { "board.arg" "" "Target board" }
   { "rootpath.arg" "" "Root path from project" }
}
set usage "You need to specify options and values"
array set optshash [::cmdline::getoptions ::argv $options $usage]
set board $optshash(board)
set project $optshash(project)
set rootpath $optshash(rootpath)

if {[string length $board]==0} {return -code error "Must specify a board"}
if {[string length $project]==0} {return -code error "Must specify a project"}
if {[string length $rootpath]==0} {return -code error "Must specify a root path"}

set corename "${project}_${board}"

source ${rootpath}/project_defs.tcl
source ${boardpath}/${board}/${board}_defs.tcl

if { ${requires_sdram}==0 || ${have_sdram}==1 } {
	project_new $corename -revision $corename -overwrite
	set_global_assignment -name TOP_LEVEL_ENTITY ${board}_top
	set_global_assignment -name AUTO_MERGE_PLLS OFF


	source ${boardpath}/${board}/${board}_opts.tcl
	source ${boardpath}/${board}/${board}_pins.tcl
	source ${boardpath}/${board}/${board}_support.tcl
	
	if {[catch "source custom_defs.tcl"]} {
		puts "Board-specific project override not found, or contains errors."
	} else {
		puts "Included board-specific project override without error."
	}
	
	set_global_assignment -name VERILOG_MACRO "DEMISTIFY=1"

	set_global_assignment -name QIP_FILE top.qip
	source ${corename}_files.tcl
} else {
	puts "Board ${board} has no SDRAM, not building ${project}"
}

