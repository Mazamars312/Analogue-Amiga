832DIR=../../
LIBDIR = $(832DIR)/lib832
ROMGENDIR = ../../romgen
ROMGEN = $(ROMGENDIR)/romgen
GHDL = ghdl
AS=../../832a/832a
LD=../../832a/832l
CC=../bin/vbcc832
COPT = -O=-1 -speed
CFLAGS = -+ -unsigned-char $(COPT) -I$(832DIR)/include/ -I$(LIBDIR)

# LITTLE ENDIAN
ASFLAGS=-el
LDFLAGS=-el -s_STACKSIZE=4096
ROMGENFLAGS=-b -w

#BIG ENDIAN
#ASFLAGS=-eb
#LDFLAGS=-eb -s_STACKSIZE=4096 -b0,0x1500
#ROMGENFLAGS=


TIME=3ms
all: compiler dhrystone_ROM.vhd

compiler:
	make -C .. TARGET=832

run: dhrystone.ghw

clean :
	-rm *ROM.vhd
	-rm *.ghw
	-rm *.asm
	-rm *.o
	-rm *.bin

a.out : dhry_1.c dhry_2.c
	gcc $+

%.ghw : %_ROM.vhd  tb.vhd force
	sed 's/_rom/$*_rom/;s/cpu_tb/$*_tb/' >$*_tb.vhd <tb.vhd
	$(GHDL) -a $*_ROM.vhd
	$(GHDL) -a $*_tb.vhd
	$(GHDL) -e $*_tb
	$(GHDL) -r $*_tb --ieee-asserts=disable-at-0 --stop-time=$(TIME) --read-wave-opt=waveopt --wave=$*.ghw


%_ROM.vhd: %.bin $(ROMGEN)
	sed 's/eightthirtytwo_rom/$*_rom/' >$*_ROM.vhd <$(ROMGENDIR)/rom_prologue_word.vhd
	$(ROMGEN) $(ROMGENFLAGS) $*.bin >>$*_ROM.vhd
	cat >>$*_ROM.vhd $(ROMGENDIR)/rom_epilogue_word.vhd

%.o : %.asm Makefile
	$(AS) $(ASFLAGS) $*.asm

dhrystone.bin : $(LIBDIR)/crt0.a $(LIBDIR)/lib832.a dhry_1.o dhry_2.o
	$(LD) $(LDFLAGS) -M $@.map -o $@ $+

%.asm : %.c Makefile ldscript.ld $(LIBDIR)/start.S $(CC)
	../bin/vbcc832 $(CFLAGS) $*.c

%.o : %.c Makefile ldscript.ld $(LIBDIR)/start.S $(CC)
	../bin/vbcc832 $(CFLAGS) $*.c
	$(AS) $(ASFLAGS) $*.asm

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

