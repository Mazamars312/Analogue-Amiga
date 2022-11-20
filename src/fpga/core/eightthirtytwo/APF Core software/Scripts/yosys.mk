BOARD=none
PROJECT=none
PROJECTOROOT=../..
DEMISTIFYPATH=
MANIFEST=$(PROJECTOROOT)/project_files.rtl
SCRIPTSDIR=$(DEMISTIFYPATH)/Scripts
BOARDDIR=$(DEMISTIFYPATH)/Board/$(BOARD)
include $(BOARDDIR)/board.mk

TARGET=$(PROJECT).json
SVFFILE=$(PROJECT).svf
BITFILE=$(PROJECT).bit
CFGFILE=$(PROJECT).config

ALL: init compile

init: $(PROJECT)_$(BOARD)_files.tcl

compile: $(BITFILE)

config: $(SVFFILE)
	openocd -f $(BOARDDIR)/target.cfg -c \
	"   init; \
	    scan_chain; \
	    svf -tap target.tap -quiet -progress ${SVFFILE}; \
	    exit;"

clean:
	-rm $(TARGET)
	-rm $(PROJECT)_$(BOARD)_files.tcl
	-rm $(BITFILE)

$(PROJECT)_$(BOARD)_files.tcl: $(MANIFEST)
	$(SCRIPTSDIR)/expandtemplate_yosys.sh $+ $(PROJECTTOROOT) >$@
	$(SCRIPTSDIR)/expandtemplate_yosys.sh $(BOARDDIR)/board.files $(BOARDDIR) >>$@

$(TARGET): $(MANIFEST) $(PROJECT)_$(BOARD)_files.tcl
	-rm $@
	$(TOOLPATH)yosys -mghdl -p 'tcl $(SCRIPTSDIR)/mkproject_yosys.tcl $(PROJECT) $(BOARD)' || echo "Compilation failed."

$(CFGFILE): $(TARGET) $(PROJECT)_$(BOARD)_files.tcl $(BOARDDIR)/$(BOARD).lpf
	-rm $@
	$(TOOLPATH)nextpnr-ecp5 $(DEVICE) --package $(DEVICE_PACKAGE) --speed $(DEVICE_SPEED) --json $< --textcfg $@ --lpf $(BOARDDIR)/$(BOARD).lpf --timing-allow-fail

$(BITFILE): $(CFGFILE)
	$(TOOLPATH)ecppack --svf ${SVFFILE} $< $@

$(SVFFILE): $(BITFILE)

