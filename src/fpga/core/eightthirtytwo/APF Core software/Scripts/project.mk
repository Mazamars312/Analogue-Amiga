MAKEFLAGS+=--no-print-directory
PROJECTS =
BOARD = 
CMD =
DEMISTIFYPATH=
PROJECTPATH=fpga
PROJECTOROOT=../..

# Bring in site-specific configuration (paths to tools, etc.)
include $(DEMISTIFYPATH)/site.mk

# Bring in board-specific defines (which tool-specific makefile and tool version to use).
include $(DEMISTIFYPATH)/Board/$(BOARD)/board.mk

all:
	@for PROJECT in ${PROJECTS}; do \
		mkdir -p $(PROJECTPATH)/$$BOARD; \
		make -C $(PROJECTPATH)/$$BOARD -f $(PROJECTTOROOT)/$(DEMISTIFYPATH)/Scripts/$(TOOL_MAKEFILE) PROJECTOROOT=$(PROJECTTOROOT)  DEMISTIFYPATH=$(PROJECTTOROOT)/$(DEMISTIFYPATH) BOARD=$(BOARD) PROJECT=$$PROJECT TOOLPATH=$(TOOLPATH) $(CMD); \
	done

