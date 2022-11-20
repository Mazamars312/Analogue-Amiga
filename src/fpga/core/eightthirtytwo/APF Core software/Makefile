MAKEFLAGS+=--no-print-directory
PROJECTS=
OVERRIDES=
DEMISTIFYPATH=
PROJECTPATH=fpga
PROJECTTOROOT=../../
EIGHTTHIRTYTWOPATH=EightThirtyTwo
include $(DEMISTIFYPATH)/site.mk

# Sizes of the two ROM parts - 8k + 4k = 12k by default
ROMSIZE1=8192
ROMSIZE2=4096

ifndef NOTIFY_BOARDCOMPLETE
NOTIFY_BOARDCOMPLETE=echo -ne '\007'
endif

all: $(EIGHTTHIRTYTWOPATH)/Makefile $(DEMISTIFYPATH)/site.mk firmware init compile

empty:=
space:= $(empty) $(empty)

$(EIGHTTHIRTYTWOPATH)/Makefile:
	git submodule init
	git submodule update --recursive

.PHONY: firmware
firmware:
	make -f $(DEMISTIFYPATH)/firmware/Makefile DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) ROMSIZE1=$(ROMSIZE1) ROMSIZE2=$(ROMSIZE2)

.PHONY: firmware_clean
firmware_clean: $(SUBMODULES)
	@make -f ../$(DEMISTIFYPATH)/firmware/Makefile DEMISTIFYPATH=../$(DEMISTIFYPATH) ROMSIZE1=$(ROMSIZE1) ROMSIZE2=$(ROMSIZE2) clean

$(DEMISTIFYPATH)/site.mk:
	$(info ************************************************)
	$(info Copy the example site.template file to site.mk)
	$(info and edit the paths for the version(s) of Quartus)
	$(info you have installed.)
	$(info ************************************************)
	$(error site.mk not found.)

.PHONY: init
init: $(DEMISTIFYPATH)/EightThirtyTwo/lib832/lib832.a
ifdef BOARD
	@make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$(BOARD) PROJECTPATH=$(PROJECTPATH) CMD=init
else
	@for BOARD in ${BOARDS}; do \
		make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$$BOARD PROJECTPATH=$(PROJECTPATH) CMD=init; \
	done
endif

.PHONY: compile
compile:
ifdef BOARD
	@make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$(BOARD) PROJECTPATH=$(PROJECTPATH) CMD=compile
else
	@for BOARD in ${BOARDS}; do \
		make --quiet -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$$BOARD PROJECTPATH=$(PROJECTPATH) CMD=compile; \
		$(NOTIFY_BOARDCOMPLETE) "DeMiSTify: Finished building for $$BOARD"; \
	done
endif

.PHONY: config
config:
ifdef BOARD
	@make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$(BOARD) PROJECTPATH=$(PROJECTPATH) CMD=compile
else
	@for BOARD in ${BOARDS}; do \
		make --quiet -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$$BOARD PROJECTPATH=$(PROJECTPATH) CMD=config; \
	done
endif

.PHONY: clean
clean:
ifdef BOARD
	@make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$(BOARD) PROJECTPATH=$(PROJECTPATH) CMD=clean
else
	@for BOARD in ${BOARDS}; do \
		make -f $(DEMISTIFYPATH)/Scripts/project.mk PROJECTTOROOT=$(PROJECTTOROOT) DEMISTIFYPATH=$(DEMISTIFYPATH) PROJECTS=$(PROJECTS) BOARD=$$BOARD PROJECTPATH=$(PROJECTPATH) CMD=clean; \
	done
endif

$(DEMISTIFYPATH)/EightThirtyTwo/RTL/eightthirtytwo_cpu.vhd:
	cd $(DEMISTIFYPATH) \
	git submodule init \ 
	git submodule update

$(DEMISTIFYPATH)/EightThirtyTwo/vbcc/bin/vbcc832: $(DEMISTIFYPATH)/EightThirtyTwo/RTL/eightthirtytwo_cpu.vhd
	make -C $(DEMISTIFYPATH)/EightThirtyTwo

$(DEMISTIFYPATH)/EightThirtyTwo/lib832/lib832.a: $(DEMISTIFYPATH)/EightThirtyTwo/vbcc/bin/vbcc832
	make -C $(DEMISTIFYPATH)/EightThirtyTwo/lib832


