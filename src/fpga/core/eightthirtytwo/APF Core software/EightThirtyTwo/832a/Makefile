CC = gcc

all: 832a 832l 832d hello

clean:
	-rm 832a
	-rm 832l
	-rm 832d
	-rm hello
	-rm *.o

832a: 832a.o 832defs.o objectfile.o section.o symbol.o codebuffer.o 832util.o equates.o expressions.o peephole.o posix.o
	$(CC) -o $@ $+

832l: 832l.o 832defs.o executable.o objectfile.o section.o symbol.o codebuffer.o sectionmap.o 832util.o equates.o mapfile.o posix.o
	$(CC) -o $@ $+

832d: 832d.o 832defs.o 832util.o section.o symbol.o codebuffer.o mapfile.o posix.o
	$(CC) -o $@ $+

%.o: %.c
	$(CC) -c $+

hello: start.S premain.S hello.S uart.S
	./832a $+
	./832l -o $@ $(patsubst %.S,%.o,$+)

