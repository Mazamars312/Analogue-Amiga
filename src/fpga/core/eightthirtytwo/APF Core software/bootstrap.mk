832MF=EightThirtyTwo/Makefile
VBCC=EightThirtyTwo/vbcc/bin/vbcc832
LIB832=EightThirtyTwo/lib832/lib832.a

all: $(LIB832)

$(832MF):
	git submodule init
	git submodule update --recursive

$(VBCC): $(832MF)
	make -C EightThirtyTwo vbcc

$(LIB832): $(VBCC)
	make -C EightThirtyTwo

