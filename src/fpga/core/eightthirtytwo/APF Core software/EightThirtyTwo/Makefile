# Makefile for toolchain and tests

all: 832a/832a 832emu/832e 832ocd/832ocd vbcc/bin/vbcc832 lib832/lib832.a test.log
	$(info )
	$(info A default dtgen setup is supplied in vbcc_quickstart to make building )
	$(info the compiler less tedious.  If you want to build on a less typical system )
	$(info delete vbcc/machines/832/dt.* then run the following command to recompile:)
	$(info > make -C vbcc TARGET=832)
	$(info )
	$(info NB: Take note of the terms of the VBCC license if you intend to distribute)
	$(info a product which incorporates code compiled for 832.)
	$(info )

clean:
	-make -C 832a clean
	-make -C 832emu clean
	-make -C 832ocd clean
	-make -C lib832 clean
	-rm vbcc/machines/832/*.o
	-rm vbcc/bin/vbcc832
	-rm test.log

832a/832a:
	-make -C 832a

832emu/832e:
	-make -C 832emu

832ocd/832ocd:
	-make -C 832ocd

lib832/lib832.a: 832a/832a 832a/832l vbcc/bin/vbcc832
	-make -C lib832 clean
	-make -C lib832

vbcc0_9g.tar.gz:
	wget http://phoenix.owl.de/tags/vbcc0_9g.tar.gz

vbcc/supp.h:
	tar -xzf vbcc0_9g.tar.gz
	cd vbcc; \
	mkdir bin; \
	patch -i ../vbcc_09g_volatilefix.patch

vbcc/bin/vbcc832: vbcc/machines/832/machine.c vbcc/supp.h
	make -C vbcc bin/dtgen
	cp vbcc_quickstart/dt.* vbcc/machines/832/
	make -C vbcc TARGET=832

vbcc/bin:
	mkdir vbcc/bin

.PHONY vbcc:
vbcc: vbcc/bin/vbcc832

test.log: lib832/lib832.a 832a/832a
	-@make -C vbcc/test emu 2>/dev/null >test.log
	-@grep --color=never Passed test.log
	-@grep Failed test.log || echo "All tests passed"

force:

