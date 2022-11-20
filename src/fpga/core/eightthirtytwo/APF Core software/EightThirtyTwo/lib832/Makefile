832DIR=../
AS=$(832DIR)/832a/832a
LD=$(832DIR)/832a/832l
CC=$(832DIR)/vbcc/bin/vbcc832
COPT = -O=1311
CFLAGS = -+ -fpic -unsigned-char $(COPT) -I$(832DIR)/include/ -I$(LIBDIR)
TIME=2ms

OBJDIR=832dir

all: $(OBJDIR) crt0.a tinycrt0.a dualcrt0.a lib832.a libtiny832.a helloworld helloworld_tiny

COMMONOBJ=$(OBJDIR)/uart.o $(OBJDIR)/string.o $(OBJDIR)/string_c.o $(OBJDIR)/stringstubs.o $(OBJDIR)/abort.o $(OBJDIR)/division.o $(OBJDIR)/setjmp.o

clean :
	-rm *.asm
	-rm $(OBJDIR)/*.o
	-rm *.a

helloworld : crt0.a lib832.a $(OBJDIR)/helloworld.o
	$(LD) -o $@ -m $@.map $+

helloworld_tiny : crt0.a libtiny832.a $(OBJDIR)/helloworld.o
	$(LD) -o $@ -m $@.map $+

crt0.a : $(OBJDIR)/start.o $(OBJDIR)/premain.o $(OBJDIR)/absolutestack.o
	cat >$@ $+

tinycrt0.a : $(OBJDIR)/start.o $(OBJDIR)/premain_tiny.o $(OBJDIR)/absolutestack.o
	cat >$@ $+

dualcrt0.a : $(OBJDIR)/start.o $(OBJDIR)/premain_dualthread.o $(OBJDIR)/dualthread.o
	cat >$@ $+

lib832.a : $(OBJDIR)/small_printf.o $(COMMONOBJ)
	cat >$@ $+

libtiny832.a : $(OBJDIR)/tiny_printf.o $(COMMONOBJ)
	cat >$@ $+

$(OBJDIR)/%.o : %.asm Makefile
	$(AS) -o $@ $*.asm

$(OBJDIR)/%.o : %.S Makefile
	$(AS) -o $@ $*.S

%.asm : %.c Makefile
	$(CC) $(CFLAGS) $*.c

$(OBJDIR):
	mkdir $(OBJDIR)

#%.o : %.c Makefile
#	$(CC) $(CFLAGS) $*.c
#	$(AS) $*.asm

