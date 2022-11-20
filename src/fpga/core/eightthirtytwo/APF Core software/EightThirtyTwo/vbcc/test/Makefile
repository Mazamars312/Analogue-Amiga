832DIR=../../
LIBDIR = $(832DIR)/lib832
ROMGENDIR = ../../romgen
ROMGEN = $(ROMGENDIR)/romgen
GHDL = ghdl
AS=../../832a/832a
LD=../../832a/832l
CC=../bin/vbcc832
EM=../../832emu/832e
CFLAGS = -+ -c99 -I$(832DIR)/include/ -I$(LIBDIR)
COPT = -O=32767 -speed -unsigned-char
TIME=200us

# Endian setting must match the littleendian generic in the testbench

# LITTLE ENDIAN
#CFLAGS += -el
#ASFLAGS=-el
#LDFLAGS=-el
#EMFLAGS=-el
#ROMGENFLAGS=-b -w

# BIG ENDIAN
CFLAGS += -eb
ASFLAGS=-eb
LDFLAGS=-eb 
EMFLAGS=-eb
ROMGENFLAGS=-w


all: compiler

compiler:
	make -C .. TARGET=832

sim: helloworld.ghw strcpytest.ghw fptrtest.ghw copytest.ghw comparisons.ghw vatest.ghw divtest.ghw

emu: helloworld.emu strcpytest.emu fptrtest.emu copytest.emu comparisons.emu divtest.emu

clean :
	-rm *_ROM.vhd
	-rm *.ghw
	-rm *.bin
	-rm *.S
	-rm *.asm
	-rm *.o
	-rm *.elf

.PRECIOUS : %.ghw

%.emu : %.bin
	$(EM) $(EMFLAGS) $*.bin

%.gtkw : %.ghw
	sed 's/cpu_tb/$*_tb/g' >$*.gtkw <tb.gtkw

%.ghw : %_ROM.vhd tb.vhd force
	sed 's/_rom/$*_rom/;s/cpu_tb/$*_tb/' >$*_tb.vhd <tb.vhd
	$(GHDL) -a $*_ROM.vhd
	$(GHDL) -a $*_tb.vhd
	$(GHDL) -e $*_tb
	$(GHDL) -r $*_tb --ieee-asserts=disable-at-0 `grep stop-time\= $*.c || echo "--stop-time=$(TIME)"` --read-wave-opt=waveopt --wave=$*.ghw


%_ROM.vhd: %.bin $(ROMGEN) force
	sed 's/eightthirtytwo_rom/$*_rom/' >$*_ROM.vhd <$(ROMGENDIR)/rom_prologue_word.vhd
	$(ROMGEN) $(ROMGENFLAGS) $*.bin >>$*_ROM.vhd
	cat >>$*_ROM.vhd $(ROMGENDIR)/rom_epilogue_word.vhd

%.o : %.s Makefile
	@$(AS) $*.s

%.bin : $(LIBDIR)/crt0.a $(LIBDIR)/lib832.a %.o
	@$(LD) -o $@ $+ $(LDFLAGS) -m map

%.o : %.S Makefile
	@$(AS) $(ASFLAGS) -o $*.o $*.S

%.o : %.asm Makefile 
	@$(AS) $(ASFLAGS) -o $*.o $*.asm

%.o : %.c Makefile
	@../bin/vbcc832 $(CFLAGS) $(COPT) $*.c
	@$(AS) $(ASFLAGS) -o $*.o $*.asm

%.o : $(LIBDIR)/%.S Makefile
	@$(AS) $(ASFLAGS) -o $*.o $(LIBDIR)/$*.S

%.asm : %.c Makefile ../bin/vbcc832 $(CC)
	../bin/vbcc832 $(CFLAGS) $(COPT) -I$(832DIR) $*.c

%.asm : $(LIBDIR)/%.c Makefile ../bin/vbcc832 $(CC)
	../bin/vbcc832 $(CFLAGS) $(COPT) -I$(832DIR) $(LIBDIR)/$*.c -o=$@

$(ROMGEN): $(ROMGENDIR)/romgen.c
	make -C $(ROMGENDIR)

init:
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_pkg.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_aligner.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_aligner_le.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_fetchloadstore.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_decode.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_shifter.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_alu.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_hazard.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_debug.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../RTL/eightthirtytwo_cpu.vhd

force:

