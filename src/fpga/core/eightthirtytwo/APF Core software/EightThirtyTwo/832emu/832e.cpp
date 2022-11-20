#include <iostream>
#include <string>
#include <sstream>

#include <getopt.h>

#include "binaryblob.h"
#include "debug.h"

#include "832opcodes.h"

/* Note: Emulator is not currently useful since I'm using the CPU in little-endian mode and the
   emulator only supports big-endian mode! */

#define STACKSIZE 1024
int STACKOFFSET=0;


enum e32endian {BIGENDIAN,LITTLEENDIAN};
enum e32size {WORD,HALFWORD,BYTE};


class EightThirtyTwoMemory
{
	public:
	EightThirtyTwoMemory(int ramsize) : uartbusyctr(0), ram(0), ramsize(ramsize), uartin(0)
	{
		ram=new unsigned char[ramsize];
	}
	virtual ~EightThirtyTwoMemory()
	{
		if(ram)
			delete[] ram;
	}
	virtual void SetUARTIn(const char *c)
	{
		uartin=c;
	}
	virtual int Read(unsigned int addr,e32endian endian,e32size opsize)
	{
		switch(addr)
		{
			case 0xffffffc4:
				Debug[COMMENT] << std::endl << "Reading from SPI_CS" << std::endl;
				break;
			case 0xffffffc8:
				Debug[COMMENT] << std::endl << "Reading from SPI" << std::endl;
				break;
			case 0xffffffcc:
				Debug[COMMENT] << std::endl << "Reading from SPI_PUMP" << std::endl;
				break;
			case 0xda8000:
			case 0xffffff84:
			case 0xffffffc0:
				Debug[COMMENT] << std::endl << "Reading from UART" << std::endl;
				if(uartbusyctr)
				{
					--uartbusyctr;
					return(0);
				}
				else
				{
					int result=0x100;
					uartbusyctr=1;	// Make the UART pretend to be busy for the next n cycles
					if(uartin)
						result=0x300 | (unsigned char)*uartin++;	// Received byte ready...
					else
					{
						if(std::cin.readsome(inbuf,1))
							result=0x300 | (inbuf[0]&0xff);
					}

					if(result==0x300)	// End of string?
						uartin=0;

					return(result);
				}
				break;
			default:
				Debug[TRACE] << std::endl << "Reading from RAM " << addr << std::endl;
				if(endian==BIGENDIAN)
				{
					Debug[TRACE] << "(Big endian)" <<std::endl;
					int r;
					switch(opsize)
					{
						case WORD:
							r=(*this)[addr]<<24;
							r|=(*this)[addr+1]<<16;
							r|=(*this)[addr+2]<<8;
							r|=(*this)[addr+3];
							break;
						case HALFWORD:
							r|=(*this)[addr]<<8;
							r|=(*this)[addr+1];
							break;
						case BYTE:
							r=(*this)[addr];
							break;
					}
					return(r);
				}
				else
				{
					Debug[TRACE] << "(Little endian)" <<std::endl;
					int r;
					switch(opsize)
					{
						case WORD:
							r=(*this)[addr+3]<<24;
							r|=(*this)[addr+2]<<16;
							r|=(*this)[addr+1]<<8;
							r|=(*this)[addr];
							return(r);
						case HALFWORD:
							r|=(*this)[addr+1]<<8;
							r|=(*this)[addr];
							break;
						case BYTE:
							r=(*this)[addr];
							break;
					}
					return(r);
				}
		}
		return(0);
	}
	virtual void Write(unsigned int addr,int v,e32endian endian,e32size opsize)
	{
		switch(addr)
		{
			case 0xda8000:
				v>>=16;
			case 0xffffff84:
			case 0xffffffc0:
				if(char(v))
				{
					Debug[COMMENT] << std::endl << "Writing " << char(v) << " to UART" << std::endl;
					std::cout << char(v);
				}
				else
				{
					Debug[COMMENT] << std::endl << "Writing (nul) to UART" << std::endl;
					std::cout << "(nul)";
				}
				break;

			case 0xffffff88:
				Debug[COMMENT] << std::endl << "Setting UART divisor to " << v << std::endl;
				break;

			case 0xffffff8C:
				Debug[COMMENT] << std::endl << "Writing to overlay register: " << v << std::endl;
				break;

			case 0xffffff90:
				Debug[COMMENT] << std::endl << "Writing " << v << " to HEX display" << std::endl;
				break;

			case 0xffffffc4:
				Debug[COMMENT] << std::endl << "Setting spi_cs line " << (v&1 ? "low" : "high" ) << std::endl;
				break;

			case 0xffffffc8:
				Debug[COMMENT] << std::endl << "Writing " << v << " to SPI" << std::endl;
				break;

			case 0xffffffcc:
				Debug[COMMENT] << std::endl << "Writing " << v << " to SPI_pump" << std::endl;
				break;

			case 0xfffffffc:
				Debug[COMMENT] << std::endl << "Breadcrumb " << std::endl;
				break;

			default:
				Debug[TRACE] << std::endl << "Writing " << v << " to RAM " << addr <<std::endl;
				if(endian==BIGENDIAN)
				{
					Debug[TRACE] << "(Big endian)" <<std::endl;
					switch(opsize)
					{
						case WORD:
							(*this)[addr]=(v>>24)&255;
							(*this)[addr+1]=(v>>16)&255;
							(*this)[addr+2]=(v>>8)&255;
							(*this)[addr+3]=v&255;
							break;
						case HALFWORD:
							(*this)[addr]=(v>>24)&255;
							(*this)[addr+1]=(v>>16)&255;
							break;
						case BYTE:
							(*this)[addr]=(v>>24)&255;
							break;
					}
				}
				else
				{
					Debug[TRACE] << "(Little endian)" <<std::endl;
					switch(opsize)
					{
						case WORD:
							(*this)[addr+3]=(v>>24)&255;
							(*this)[addr+2]=(v>>16)&255;
							(*this)[addr+1]=(v>>8)&255;
							(*this)[addr]=v&255;
							break;
						case HALFWORD:
							(*this)[addr+1]=(v>>8)&255;
							(*this)[addr]=v&255;
							break;
						case BYTE:
							(*this)[addr]=v&255;
							break;
					}
				}
		}
	}
	virtual unsigned char &operator[](const int idx)
	{
		return(ram[idx]);
	}
	virtual int GetRAMSize()
	{
		return(ramsize);
	}
	protected:
	int uartbusyctr;
	unsigned char *ram;
	int ramsize;
	const char *uartin;
	char inbuf[4];
};


// EightThirtyTwoProgram overlays the program loaded from disk onto a fixed-size memory block.
// Accesses within the program are sent directly to the binary blob.
// Accesses beyond the program are passed to the EightThirtyTwoMemory superclass.
// FIXME - need to support byte accesses to EightThirtyTwoMemory.

class EightThirtyTwoProgram : public BinaryBlob, public EightThirtyTwoMemory
{
	public:
	EightThirtyTwoProgram(const char *filename, int ramsize=8*1024*1024) : BinaryBlob(filename), EightThirtyTwoMemory(ramsize), base(0)
	{
	}
	~EightThirtyTwoProgram()
	{
	}
	virtual int Read(unsigned int addr,e32endian endian,e32size opsize)
	{
		if(((addr-base)>=0) && ((addr-base)<=(size&~3)))
		{
			if(endian==BIGENDIAN)
			{
				int r;
				Debug[TRACE] << "(Read internal memory Big endian)" << std::endl;
				switch(opsize)
				{
					case WORD:
						r=(*this)[addr]<<24;
						r|=(*this)[addr+1]<<16;
						r|=(*this)[addr+2]<<8;
						r|=(*this)[addr+3];
						break;
					case HALFWORD:
						r|=(*this)[addr]<<8;
						r|=(*this)[addr+1];
						break;
					case BYTE:
						r=(*this)[addr];
						break;
				}
				return(r);
			}
			else
			{
				int r;
				Debug[TRACE] << "(Read internal memory little endian)" << std::endl;
				switch(opsize)
				{
					case WORD:
						r=(*this)[addr+3]<<24;
						r|=(*this)[addr+2]<<16;
						r|=(*this)[addr+1]<<8;
						r|=(*this)[addr];
						return(r);
					case HALFWORD:
						r|=(*this)[addr+1]<<8;
						r|=(*this)[addr];
						break;
					case BYTE:
						r=(*this)[addr];
						break;
				}
				return(r);
			}
		}
		return(EightThirtyTwoMemory::Read(addr,endian,opsize));
	}
	virtual void Write(unsigned int addr,int v,e32endian endian,e32size opsize)
	{
		if(((addr-base)>=0) && ((addr-base)<(size-3)))
		{
			if(endian==BIGENDIAN)
			{
				Debug[TRACE] << "(Write internal memory Big endian)" << std::endl;
				switch(opsize)
				{
					case WORD:
						(*this)[addr]=(v>>24)&255;
						(*this)[addr+1]=(v>>16)&255;
						(*this)[addr+2]=(v>>8)&255;
						(*this)[addr+3]=v&255;
						break;
					case HALFWORD:
						(*this)[addr]=(v>>8)&255;
						(*this)[addr+1]=v&255;
						break;
					case BYTE:
						(*this)[addr]=v&255;
						break;
				}
			}
			else
			{
				Debug[TRACE] << "(Write internal memory little endian)" << std::endl;
				switch(opsize)
				{
					case WORD:
						(*this)[addr+3]=(v>>24)&255;
						(*this)[addr+2]=(v>>16)&255;
						(*this)[addr+1]=(v>>8)&255;
						(*this)[addr]=v&255;
						break;
					case HALFWORD:
						(*this)[addr+1]=(v>>8)&255;
						(*this)[addr]=v&255;
						break;
					case BYTE:
						(*this)[addr]=v&255;
						break;
				}
			}
		}
		else
			EightThirtyTwoMemory::Write(addr,v,endian,opsize);
	}
	void SetBase(int base)
	{
		this->base=base;
	}
	unsigned char &operator[](const int idx)
	{
		if(idx<size)
			return(BinaryBlob::operator[](idx));
		else
			return(EightThirtyTwoMemory::operator[](idx));
	}
	protected:
	int base;
};


class EightThirtyTwoEmu 
{
	public:
	EightThirtyTwoEmu() : initpc(0), steps(-1), endian(LITTLEENDIAN)
	{
		temp=0;
		regfile[0]=0;
		regfile[1]=0;
		regfile[2]=0;
		regfile[3]=0;
		regfile[4]=0;
		regfile[5]=0;
		regfile[6]=0;
	}

	~EightThirtyTwoEmu()
	{
	}

	int ParseOptions(int argc,char *argv[])
	{
		static struct option long_options[] =
		{
			{"help",no_argument,NULL,'h'},
			{"steps",required_argument,NULL,'s'},
			{"offset",required_argument,NULL,'o'},
			{"report",required_argument,NULL,'r'},
			{0, 0, 0, 0}
		};
		bool offset=false;
		int stackbit=30;
		bool stackboot=false;

		while(1)
		{
			int c;
			c = getopt_long(argc,argv,"he:s:r:o:bm",long_options,NULL);
			if(c==-1)
				break;
			switch (c)
			{
				case 'h':
					printf("Usage: %s [options] <UART input text>\n",argv[0]);
					printf("    -h --help\t  display this message\n");
					printf("    -e --endian\t  Set endian mode to \"l\" (default) or \"b\"\n");
					printf("    -s --steps\t  Emulate a specific number of steps (default: indefinite)\n");
					printf("    -r --report\t  set reporting level - 0 for silent, 4 for verbose\n");
					printf("    -o --offsetstack\t  specify base address for stack RAM. Zero by default,\n");
					printf("\t\t  specified as a bit number, so 30=0x40000000, etc.\n");
					break;
				case 'e':
					endian=optarg[0]=='l' ? LITTLEENDIAN : BIGENDIAN;
					break;
				case 'o':
					offset=true;
					stackbit=atoi(optarg);
					break;
				case 's':
					steps=atoi(optarg);
					break;
				case 'r':
					Debug.SetLevel(DebugLevel(atoi(optarg)));
					break;
			}
		}

		return(optind);
	}

	

	int GetOpcode(EightThirtyTwoMemory &prg, int pc)
	{
//		int t=prg.Read(pc&~3,endian);
//		int opcode=t>>((3-(pc&3))<<3);
//		return(opcode&0xff);
		return(prg[pc]&0xff);
	}


	void Run(EightThirtyTwoProgram &prg)
	{
		Debug[WARN] << "Starting emulation" << std::endl;
		Debug[ERROR] << std::hex << std::endl;
		for(int i=0;i<7;++i)
			regfile[i]=0;
		regfile[7]=initpc;
                regfile[6]=prg.GetRAMSize();
		zero=0; carry=0;
		cond=1;

		int tick=0;
		bool run=true;
		bool immediate_continuation=false;
		enum e32size sizemod=WORD;
		bool sign_mod=false;

		while(run)
		{
			int nextpc;
			int opcode;
			int operand;
			int operim;
			int t;
			long long t2;

			std::stringstream mnem;
			mnem << std::hex;

			opcode=GetOpcode(prg,regfile[7]);
			operand=opcode&0x7;
			operim=opcode&0x3f;
			opcode&=0xf8;

			Debug[TRACE] << std::dec << tick << ", r7: " << std::hex << regfile[7];

			nextpc=regfile[7]+1;
			regfile[7]=nextpc;			

			Debug[TRACE] << " op: " << opcode;

			if(cond) // is execution enabled?
			{
				if((opcode&0xc0)==0xc0)
				{
					if(immediate_continuation)
					{
						temp<<=6;
						temp|=operim;
						mnem<<("li (cont) ");
						mnem << operim;
					}
					else
					{
						temp=operim;
						if(operim&0x20)
							temp|=0xffffffc0;
						mnem<<("li ");
						mnem<< operim;
						immediate_continuation=true;
					}
				}
				else if((opcode|operand)==ovl_hlf)	// Evaluate overloaded (zero-operand) opcodes here
				{
					sizemod=HALFWORD;
					mnem << ("hlf ");
					// Set the halfword modifier, cleared by the next instruction that could use it.
				}
				else if((opcode|operand)==ovl_byt)
				{
					sizemod=BYTE;
					mnem << ("byt ");
					// Set the byte modifier, cleared by the next instruction that could use it.
				}
				else if((opcode|operand)==ovl_sgn)
				{
					sign_mod=true;
					mnem << ("sgn ");
					// Set the sign modifier, cleared by the next instruction that could use it.
				}
				else if((opcode|operand)==ovl_ldt)
				{
					temp=prg.Read(temp,endian,sizemod);
					sizemod=WORD;
					mnem << ("ldt ");
				}
				else	// Now having dealt with special cases, consider full opcodes
				{
					immediate_continuation=false;
					switch(opcode)
					{

						// Control flow:

						case opc_cond: // cond
							if(!operand)
								steps=1;
							t=((zero&carry)<<3)|((!zero&carry)<<2)|((zero&!carry)<<1)|(!zero&!carry);
							operand|=(operand&2)<<2;
							cond=(operand&t)>0;
							mnem << ("cond ") << operand << (", ") << t;
							break;


						// Register


						case opc_mt: // mt
							temp=regfile[operand];
							mnem << ("mt ") << operand;
							break;

						case opc_mr: // mr
							regfile[operand]=temp;
							if(operand==7)
								cond=1; // cancel cond on write to r7
							mnem << ("mr ") << operand;
							break;

						case opc_exg: // exg
							t=regfile[operand];
							regfile[operand]=temp;
							temp=t;
							if(operand==7)
								cond=1; // cancel cond on write to r7
							mnem << ("exg ") << operand;
							break;


						// Memory


						case opc_ld: // ld
							temp=prg.Read(regfile[operand],endian,sizemod); // &0xfffffffc);
							zero=(temp==0);
							carry=(temp&0x80000000)!=0;
							sizemod=WORD;
							mnem << ("ld ") << operand;
							break;

						case opc_ldinc: // ldinc
							temp=prg.Read(regfile[operand],endian,sizemod); // &0xfffffffc);
							regfile[operand]+=4;
							zero=(temp==0);
							carry=(temp&0x80000000)!=0;
							sizemod=WORD;
							mnem << ("ldinc ") << operand;
							break;

						case opc_ldbinc: // ldbinc
							Debug[TRACE] << " operand " << operand << ": " << regfile[operand]; 
							temp=prg[regfile[operand]];
							regfile[operand]++;
							zero=(temp==0);
							carry=0;
							sizemod=WORD;
							mnem << ("ldbinc ") << operand;
							break;

						case opc_ldidx: // ldidx
							temp=prg.Read(temp+regfile[operand],endian,sizemod);//&0xfffffffc);
							sizemod=WORD;
							zero=(temp==0);
							carry=(temp&0x80000000)!=0;
							mnem << ("ldidx ") << operand;
							break;


						case opc_st: // st
							prg.Write(regfile[operand],temp,endian,sizemod); // &0xfffffffc,temp);
							sizemod=WORD;
							mnem << ("st ") << operand;
							break;

						case opc_stdec: // stdec
							regfile[operand]-=4;
							prg.Write(regfile[operand],temp,endian,sizemod); //&0xfffffffc,temp);
							sizemod=WORD;
							mnem << ("stdec ") << operand;
							break;

						case opc_stmpdec: // stmpdec
							temp-=4;
							prg.Write(temp,regfile[operand],endian,sizemod);
							sizemod=WORD;
							mnem << ("stmpdec ") << operand;
							break;

						case opc_stbinc: // stbinc
							prg[regfile[operand]]=temp&0xff;
							regfile[operand]++;
							sizemod=WORD;
							mnem << ("stbinc ") << operand;
							break;

						case opc_stinc: // stinc
							prg.Write(regfile[operand],temp,endian,sizemod); //&0xfffffffc,temp);
							regfile[operand]+=4;
							sizemod=WORD;
							mnem << ("stinc ") << operand;
							break;

						// Arithmetic


						case opc_add: // add
							t2=regfile[operand];
							t2+=temp;
							carry=(t2>>32)&1;
							zero=(t2&0xffffffff)==0;
							if(operand==7)
							{
								cond=1; // cancel cond on write to r7
								temp=regfile[operand];	// For r7, previous value goes to temp
							}
							regfile[operand]=t2;
							mnem << ("add ") << operand;
							break;

						case opc_addt: // addt;
							t=regfile[operand];
							t2=regfile[operand];
							t2+=temp;
							carry=(t2>>32)&1;
							zero=(t2&0xffffffff)==0;
							if(operand==7)
								cond=1; // cancel cond on write to r7
							temp=t2; // result goes to temp.

							mnem << ("addt ") << operand;
							break;

						case opc_cmp: // cmp - FIXME - heed then clear sign modifier.
							sign_mod=(sign_mod) and (((regfile[operand]>>31)&1) xor ((temp>>31)&1));
							t2=regfile[operand];
							t2-=temp;
							carry=(t2>>32)&1;
							carry^=sign_mod;
							sign_mod=0;
							zero=(t2&0xffffffff)==0;
							mnem << ("cmp ") << operand;
							break;

						case opc_sub: // sub - FIXME - heed then clear sign modifier.
							sign_mod=(not sign_mod) and (((regfile[operand]>>31)&1) xor ((temp>>31)&1));
							t2=regfile[operand];
							t2-=temp;
							carry=(t2>>32)&1;
							carry^=sign_mod;
							sign_mod=0;
							regfile[operand]=t2;
							zero=(t2&0xffffffff)==0;
							if(operand==7)
								cond=1; // cancel cond on write to r7
							mnem << ("sub ") << operand;
							break;


						// Logical


						case opc_and: // and
							regfile[operand]&=temp;
							carry=0;
							zero=regfile[operand]==0;
							mnem << ("and ") << operand;
							break;

						case opc_or: // or
							regfile[operand]|=temp;
							carry=0;
							zero=regfile[operand]==0;
							mnem << ("or ") << operand;
							break;

						case opc_xor: // xor
							regfile[operand]^=temp;
							carry=0;
							zero=regfile[operand]==0;
							mnem << ("xor ") << operand;
							break;

						case opc_shl: // shl
							t2=regfile[operand]<<(temp-1);
							carry=t2>>32;
							regfile[operand]<<=temp;
							zero=regfile[operand]==0;
							mnem << ("shl ") << operand;
							break;

						case opc_shr: // asr FIXME heed sign bit
							carry=regfile[operand]>>(temp-1);
							carry&=1;
							if(sign_mod)
							{
								t=regfile[operand];
								t>>=temp;
								regfile[operand]=t;
								mnem << ("shr(a) ") << operand;
							}
							else
							{
								regfile[operand]>>=temp;
								mnem << ("shr(l) ") << operand;
							}
							sign_mod=false;
							zero=regfile[operand]==0;
							break;

						case opc_ror: // ror
							carry=regfile[operand]>>(temp-1);
							carry&=1;
							t=regfile[operand]<<(32-temp);
							regfile[operand]=(regfile[operand]>>temp)|t;
                                                        mnem << ("ror ") << operand;
							break;

					}
				}
			}
			else // execution disabled by cond
			{
				mnem << ("(");
				if(opcode==opc_cond)
				{
					t=((zero&carry)<<3)|((!zero&carry)<<2)|((zero&!carry)<<1)|(!zero&!carry);
					operand|=(operand&2)<<2;
					cond=(operand&t)>0;
					mnem << ("cond ") << operand << (", ") << t;
				}
				else if(operand==7)
				{
					switch(opcode)
					{
						case opc_mr: // mr
						case opc_exg: // exg
						case opc_add: // add
						case opc_sub: // sub
						case opc_addt: // addt;
							cond=1;
					}
				}
				mnem << (")");
			}
			++tick;
			if(steps>=0 && tick>=steps)
				run=0;

			Debug[TRACE] << "\tOp: " << (opcode|operand) << ", " << mnem.str() << "\n\t\t";
			DumpRegs();
			Debug[TRACE] << std::endl;

			if(!run)
				Debug[TRACE] << "Emulation ended\n" << std::endl;
		}
	}
	void DumpRegs()
	{
		Debug[TRACE] << "Temp: " << temp << ", ";
		for(int i=0;i<7;++i)
		{
			Debug[TRACE] << "r" << i << ": " << regfile[i] << ", ";
		}
		Debug[TRACE] << "Z: " << zero << ", C: " << carry << ", Cond: " << cond;
	}
	protected:
	unsigned int regfile[8];
	int cond;
	unsigned int temp;
	int zero;
	int carry;
	int initpc;
	int steps;
	enum e32endian endian;
};



int main(int argc, char **argv)
{
	try
	{
		Debug.SetLevel(WARN);
		if(argc>1)
		{
			int i;
			char *uartin=0;
			EightThirtyTwoEmu sim;
			i=sim.ParseOptions(argc,argv);
			if(i<argc)
			{
				EightThirtyTwoProgram prg(argv[i++]);
				if(i<argc)
				{
					Debug[TRACE] << "Setting uartin to " << argv[i] << std::endl;
					prg.SetUARTIn(argv[i++]);
				}
				else
				{
					Debug[TRACE] << "All arguments used: " << i << ", " << argc << std::endl;
				}
				sim.Run(prg);
				std::cout << std::endl;
			}
		}
	}
	catch(const char *err)
	{
		std::cerr << "Error: " << err << std::endl;
	}
	return(0);
}

