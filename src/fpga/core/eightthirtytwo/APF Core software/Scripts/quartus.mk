BOARD=none
PROJECT=none
PROJECTPATH=
PROJECTOROOT=../..
DEMISTIFYPATH=
MANIFEST=$(PROJECTOROOT)/project_files.rtl
SCRIPTSDIR=$(DEMISTIFYPATH)/Scripts

SOF=output_files/$(PROJECT)_$(BOARD).sof
QSF=$(PROJECT)_$(BOARD).qsf
FILES=$(PROJECT)_$(BOARD)_files.tcl

all: init compile

.PHONY init:
init: $(QSF)

$(FILES): $(MANIFEST) $(PROJECTTOROOT)/project_defs.tcl
	@bash $(SCRIPTSDIR)/expandtemplate_quartus.sh $(PROJECTTOROOT) $+ >$@

%.qsf: $(FILES)
	@echo -n "Making project file for $(PROJECT) on $(BOARD)..."
	@$(TOOLPATH)/quartus_sh >init.log -t $(SCRIPTSDIR)/mkproject.tcl -project $(PROJECT) -board $(BOARD) -rootpath $(PROJECTTOROOT) && echo "\033[32mSuccess\033[0m" || grep Error init.log

.PHONY compile:
compile: $(QSF)
	@echo -n "Compiling $(PROJECT) for $(BOARD)... "
	@$(TOOLPATH)/quartus_sh >compile.log -t $(SCRIPTSDIR)/compile.tcl -project $(PROJECT) -board $(BOARD) && echo "\033[32mSuccess\033[0m" || grep Error compile.log
	-@grep implicit compile.log

clean:
	-rm $(QSF)
	-rm $(FILES)
	-rm -rf db incremental_db output_files

