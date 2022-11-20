/*
	832ocd_connection.c

	Copyright (c) 2020 by Alastair M. Robinson

	This file is part of the EightThirtyTwo CPU project.

	EightThirtyTwo is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	EightThirtyTwo is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with EightThirtyTwo.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>
#include "832ocd.h"
#include "832ocd_connection.h"


struct ocd_connection *ocd_connection_new()
{
	struct ocd_connection *result=0;
	result=(struct ocd_connection *)malloc(sizeof(struct ocd_connection));
	if(result)
	{
		result->sock=-1;
		result->bridgeconnected=0;
		result->sock=socket(AF_INET, SOCK_STREAM, 0);
		if(result->sock < 0)
		{
			free(result);
			return(0);
		}
	}
	return(result);
}


void ocd_connection_delete(struct ocd_connection *con)
{
	if(con)
	{
		if(con->bridgeconnected)
			close(con->sock);
		free(con);
	}
}


const char *ocd_connect(struct ocd_connection *con,const char *ip,int port)
{
	if(con)
	{
		if(con->sock>=0)
		{
			close(con->sock);
			con->sock=socket(AF_INET, SOCK_STREAM, 0);
			if(con->sock<0)
				return("Can't create new socket");
		}
		con->bridgeconnected=0;

	    memset(&con->serv_addr, '0', sizeof(con->serv_addr));
		con->serv_addr.sin_family = AF_INET;
		con->serv_addr.sin_port = htons(port);
      
		if(inet_pton(AF_INET, ip, &con->serv_addr.sin_addr)<=0) 
			return("Invalid address / Address not supported");

		if (connect(con->sock, (struct sockaddr *)&con->serv_addr, sizeof(con->serv_addr)) < 0)
			return("Can't connect to JTAG bridge");
		con->bridgeconnected=1;
		return(0);
	}
	return("Internal error");
}


int ocd_uploadfile(struct ocd_connection *con,const char *filename, int addr, enum eightthirtytwo_endian endian)
{
	int len;
	unsigned char buf[4];
	int result=0;
	FILE *file;
	OCD_STOP(con);

	file=fopen(filename,"rb");
	if(file)
	{
		int len=4;
		while(len==4)
		{
			len=fread(buf,1,4,file);		
			if(len)
			{
				int v;
				if(endian==EIGHTTHIRTYTWO_LITTLEENDIAN)
					v=(buf[3]<<24) | (buf[2]<<16) | (buf[1]<<8) | buf[0];
				else
					v=(buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
				OCD_WRITE(con,addr,v);
			}
			addr+=len;
		}
		result=1;
		fclose(file);
		len=OCD_READ(con,0); /* Finish with a read to wait until upload is complete */
		ocd_release(con);
	}
	return(result);
}


int ocd_command(struct ocd_connection *con,enum dbg832_op op,int paramcount,int responsecount,int p1,int p2,int p3)
{
	int result=0;
	int l=4;

	if(!con)
		return(0);

	con->cmdbuffer[0]=op;
	con->cmdbuffer[1]=paramcount;
	con->cmdbuffer[2]=responsecount;
	con->cmdbuffer[3]=p1;
	switch(paramcount)
	{
		case 8:	/* Two 32-bit parameters */
			con->cmdbuffer[8]=(p3>>24);
			con->cmdbuffer[9]=(p3>>16)&255;
			con->cmdbuffer[10]=(p3>>8)&255;
			con->cmdbuffer[11]=(p3)&255;
			l+=4;
			/* Fall through */
		case 4: /* First 32-bit parameter */
			con->cmdbuffer[4]=(p2>>24);
			con->cmdbuffer[5]=(p2>>16)&255;
			con->cmdbuffer[6]=(p2>>8)&255;
			con->cmdbuffer[7]=(p2)&255;
			l+=4;
			break;
		default:
			break;
	}
    send(con->sock,con->cmdbuffer,l,MSG_NOSIGNAL);
	if(errno==EPIPE)
		con->bridgeconnected=0;
	while(responsecount--)
	{
		recv(con->sock,con->cmdbuffer,1,0);
		result=(result<<8)|(con->cmdbuffer[0]&255);
	}
	return(result);
}

void ocd_release(struct ocd_connection *con)
{
	if(con)
	{
		con->cpuconnected=OCD_RELEASE(con);
	}
}

#ifdef DBG
int main(int argc, char const *argv[])
{
	int result;
    int valread;
	int i;

	struct ocd_connection *con;

	con=ocd_connection_new();
	if(!con)
		return(0);

	if(ocd_connect(con,OCD_ADDR,OCD_PORT))
	{
		printf("Connected...\n");

	}

//	result=ocd_command(con,DBG832_READ,4,4,7,0x0000004,0);
	result=OCD_READ(con,4);
	result=OCD_READ(con,8);
	result=OCD_READ(con,12);
	printf("%08x\n",result);
//	result=ocd_command(con,DBG832_SINGLESTEP,0,0,7,0,0);
	result=OCD_SINGLESTEP(con);
	printf("%08x\n",result);
	OCD_RELEASE(con);
	result=OCD_READREG(con,7);
	printf("%08x\n",result);
	for(i=0;i<1024;i+=4)
	{
		result=OCD_READ(con,i);
		printf("%08x\n",result);
	}
//	result=command(sock,DBG832_RELEASE,0,0,0,0,0);
//	printf("%08x\n",result);
//	result=command(sock,DBG832_SINGLESTEP,0,0,7,0x12345678,0);
//	printf("%08x\n",result);
    return 0;
}
#endif

