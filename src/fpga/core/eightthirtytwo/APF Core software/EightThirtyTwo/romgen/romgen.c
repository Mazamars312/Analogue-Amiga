// romgen.c
//
// Program to turn a binary file into a VHDL lookup table.
//   by Adam Pierce
//   29-Feb-2008
//
// Modified by Alastair M. Robinson
//
// This software is free to use by anyone for any purpose.
//
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include <getopt.h>

typedef unsigned char BYTE;

struct RomGenOptions
{
	int offset;
	int limit;
	int byteswap;
	int	word;
};

int ParseOptions(int argc,char **argv,struct RomGenOptions *opts)
{
	static struct option long_options[] =
	{
		{"help",no_argument,NULL,'h'},
		{"offset",required_argument,NULL,'o'},
		{"limit",required_argument,NULL,'l'},
		{"word",no_argument,NULL,'w'},
		{"byteswap",no_argument,NULL,'b'},
		{0, 0, 0, 0}
	};

	while(1)
	{
		int c;
		c = getopt_long(argc,argv,"ho:l:wb",long_options,NULL);
		if(c==-1)
			break;
		switch (c)
		{
			case 'h':
				printf("Usage: %s [options] <filename>\n",argv[0]);
				printf("    -h --help\t  display this message\n");
				printf("    -w --word\t  output as word-oriented rather than byte-oriented.\n");
				printf("    -o --offset\t  skip a number of bytes before outputting ROM data.\n");
				printf("    -l --limit\t  stop after a specified number of bytes of ROM data.\n");
				printf("    -b --byteswap\t  reverse the byte order of the ROM data.\n");
				break;
			case 'o':
				opts->offset=atoi(optarg);
				break;
			case 'l':
				opts->limit=atoi(optarg);
				break;
			case 'b':
				opts->byteswap=1;
				break;
			case 'w':
				opts->word=1;
				break;
		}
	}
	return(optind);
}

main(int argc, char **argv)
{
	BYTE    opcode[4];
	int     fd;
	int     addr = 0;
	int i;
	struct RomGenOptions opts;
	ssize_t s;

	opts.limit=0x7fffffff;
	opts.offset=0;
	opts.byteswap=0;
	opts.word=0;

	i=ParseOptions(argc,argv,&opts);

	// Check the user has given us an input file.
	if(i>=argc)
	return 1;

	// Open the input file.
	fd = open(argv[i], 0);
	if(fd == -1)
	{
		perror("File Open");
		return 2;
	}

	while(addr<(opts.limit/4))
	{
		// Read 32 bits.
		if(opts.offset)
		{
			while(opts.offset>0)
			{
				s = read(fd, opcode, 4);
				opts.offset-=4;
			}
		}
		s = read(fd, opcode, 4);
		if(s == -1)
		{
			perror("File read");
			return 3;
		}

		if(s == 0)
			break; // End of file.

		// Output to STDOUT.

		if(opts.word)
		{
			if(opts.byteswap)
			{
				printf("%6d => x\"%02x%02x%02x%02x\",\n",
				addr++, opcode[3], opcode[2],
				opcode[1], opcode[0]);
			}
			else
			{
				printf("%6d => x\"%02x%02x%02x%02x\",\n",
				addr++, opcode[0], opcode[1],
				opcode[2], opcode[3]);
			}
		}
		else
		{
			if(opts.byteswap)
			{
				printf("%6d => (x\"%02x\",x\"%02x\",x\"%02x\",x\"%02x\"),\n",
				addr++, opcode[3], opcode[2],
				opcode[1], opcode[0]);
			}
			else
			{
				printf("%6d => (x\"%02x\",x\"%02x\",x\"%02x\",x\"%02x\"),\n",
				addr++, opcode[0], opcode[1],
				opcode[2], opcode[3]);
			}
		}
	}

	close(fd);
	return 0;
}

