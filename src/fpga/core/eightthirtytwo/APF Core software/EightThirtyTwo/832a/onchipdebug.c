#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>


enum dbg832_op {DBG832_NOP,DBG832_STATUS,DBG832_RUN,DBG832_SINGLESTEP,DBG832_STEPOVER,
		DBG832_READREG,DBG832_READFLAGS,DBG832_READ,DBG832_WRITE,
		DBG832_BREAKPOINT,DBG832_RELEASE=255};

char cmdbuffer[16];

struct regfile
{
	int regs[8];
	int tmp;
	char c,z,cond;
};


int command(int sock,enum dbg832_op op,int paramcount,int responsecount,int p1,int p2,int p3)
{
	int result=0;
	int l=4;
	cmdbuffer[0]=op;
	cmdbuffer[1]=paramcount;
	cmdbuffer[2]=responsecount;
	cmdbuffer[3]=p1;
	switch(paramcount)
	{
		case 8:	/* Two 32-bit parameters */
			cmdbuffer[8]=(p3>>24);
			cmdbuffer[9]=(p3>>16)&255;
			cmdbuffer[10]=(p3>>8)&255;
			cmdbuffer[11]=(p3)&255;
			l+=4;
			/* Fall through */
		case 4: /* First 32-bit parameter */
			cmdbuffer[4]=(p2>>24);
			cmdbuffer[5]=(p2>>16)&255;
			cmdbuffer[6]=(p2>>8)&255;
			cmdbuffer[7]=(p2)&255;
			l+=4;
			break;
		default:
			break;
	}
    send(sock,cmdbuffer,l,0);
	while(responsecount--)
	{
		read(sock,cmdbuffer,1);
		result=(result<<8)|(cmdbuffer[0]&255);
	}
	return(result);
}


int main(int argc, char const *argv[])
{
	int result;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(33581);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
//    send(sock , hello , strlen(hello) , 0 );
//    printf("Hello message sent\n");
//	result=command(sock,DBG832_READREG,0,0,7,0x12345678,0);
//	printf("%08x\n",result);
	result=command(sock,DBG832_READ,4,4,7,0x0000004,0);
	printf("%08x\n",result);
	result=command(sock,DBG832_SINGLESTEP,0,0,7,0,0);
	printf("%08x\n",result);
	result=command(sock,DBG832_READREG,0,4,7,0,0);
	printf("%08x\n",result);
//	result=command(sock,DBG832_RELEASE,0,0,0,0,0);
//	printf("%08x\n",result);
//	result=command(sock,DBG832_SINGLESTEP,0,0,7,0x12345678,0);
//	printf("%08x\n",result);
    return 0;
}

