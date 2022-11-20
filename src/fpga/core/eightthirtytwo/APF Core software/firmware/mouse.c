#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "keyboard.h"
#include "uart.h"
#include "ps2.h"
#include "user_io.h"
#include "spi.h"
#include "timer.h"

#ifdef PS2_MOUSE
static unsigned char initmouse[]=
{	
	0x1,0xff, // Send 1 byte reset sequence
	0x82,	// Wait for two bytes in return (in addition to the normal acknowledge byte)
#ifndef PS2_MOUSE_WHEEL
	1,0xf4,0,
#else
	8,0xf3,200,0xf3,100,0xf3,80,0xf2,1, // Send PS/2 wheel mode knock sequence...
	0x81,	// Receive device ID (should be 3 for wheel mice)
	1,0xf4,0	// Enable reporting.
#endif
};

static unsigned char rxmouse[4];

void HandlePS2Mouse(int reset)
{
	int byte;
	static int delay=0;
	static int timeout;
	static int rxidx=0;
	static int idx=0;
	static int txcount=0;
	static int rxcount=0;
#ifdef PS2_MOUSE_WHEEL
	static int protocol=4;
#else
	static int protocol=3;
#endif
	if(reset)
		idx=0;

	if(!idx)
	{
		while(PS2MouseRead()>-1)
			; // Drain the buffer;
		txcount=initmouse[idx++];
		rxcount=0;
	}
	else
	{
		if(rxcount)
		{
			int q=PS2MouseRead();
			if(q>-1)
			{
//				printf("Received %x\n",q);
				--rxcount;
			}
			else if(CheckTimer(timeout))
			{
				/* Clear the mouse buffer on timeout, to avoid blocking if no mouse if connected */
				ps2_ringbuffer_init(&mousebuffer);
				idx=0;
			}
	
			if(!txcount && !rxcount)
			{
				int next=initmouse[idx++];
				if(next&0x80)
				{
					rxcount=next&0x7f;
//					printf("Receiving %x bytes",rxcount);
				}
				else
				{
					txcount=next;
//					if(!txcount)
//						printf("Mouse initialised in %d byte mode\n",protocol);
//					printf("Sending %x bytes",txcount);
				}
			}
		}
		else if(txcount)
		{
//			printf("Sending %x bytes",txcount);
			PS2MouseWrite(initmouse[idx++]);
			--txcount;
			rxcount=1;
			timeout=GetTimer(3500);	//3.5 seconds
		}
#ifdef PS2_MOUSE_USERIO
		else
		{
			int c;
			while((c=PS2MouseRead())>-1)
			{
				if(rxidx && CheckTimer(timeout))	// Did we receive a part packet?  Probably in the wrong mode...
				{
//					printf("Timeout on %d, Protocol %d - toggling\n",rxidx,protocol);
					protocol=7-protocol; // Toggle between 3 and 4 byte packets.
					rxidx=0;
				}
				rxmouse[rxidx++]=c;
				if(rxidx>=protocol)
				{
					// Send the packet to the core.
					spi_uio_cmd8_cont(UIO_MOUSE, rxmouse[0]);
					SPI(rxmouse[1]);
					SPI(rxmouse[2]);
					DisableIO();
					rxidx=0;				
				}
				timeout=GetTimer(500);	//0.5 seconds
			}
		}
#endif
	}
}
#endif

