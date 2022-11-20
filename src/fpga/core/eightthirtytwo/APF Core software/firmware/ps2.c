/*
-- Low-level PS/2 communication code
-- Copyright (c) 2014 by Alastair M. Robinson

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty
-- of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include "ps2.h"
#include "interrupts.h"
#include "keyboard.h"

void ps2_ringbuffer_init(struct ps2_ringbuffer *r)
{
	r->in_hw=0;
	r->in_cpu=0;
#ifdef PS2_WRITE
	r->out_hw=0;
	r->out_cpu=0;
#endif
}

#ifdef PS2_WRITE
void ps2_ringbuffer_write(struct ps2_ringbuffer *r,int in)
{
	while(r->out_hw==((r->out_cpu+1)&(PS2_RINGBUFFER_SIZE-1)))
		;
//	printf("w: %d, %d\n, %d\n",r->out_hw,r->out_cpu,in);
	DisableInterrupts();
	r->outbuf[r->out_cpu]=in;
	r->out_cpu=(r->out_cpu+1) & (PS2_RINGBUFFER_SIZE-1);
	PS2Handler();
	EnableInterrupts();
}
#endif

int ps2_ringbuffer_read(struct ps2_ringbuffer *r)
{
	unsigned char result;
//	printf("%d,%d\n",r->in_hw,r->in_cpu);
	if(r->in_hw==r->in_cpu)
		return(-1);	// No characters ready
	DisableInterrupts();
	result=r->inbuf[r->in_cpu];
	r->in_cpu=(r->in_cpu+1) & (PS2_RINGBUFFER_SIZE-1);
	EnableInterrupts();
	return(result);
}

void ps2_ringbuffer_receive(struct ps2_ringbuffer *r,int in)
{
	r->inbuf[r->in_hw]=in&0xff;
	r->in_hw=(r->in_hw+1) & (PS2_RINGBUFFER_SIZE-1);
}

int ps2_ringbuffer_count(struct ps2_ringbuffer *r)
{
	if(r->in_hw>=r->in_cpu)
		return(r->in_hw-r->in_cpu);
	return(r->in_hw+PS2_RINGBUFFER_SIZE-r->in_cpu);
}

struct ps2_ringbuffer kbbuffer;

#ifdef PS2_MOUSE
struct ps2_ringbuffer mousebuffer;
#endif

void PS2Handler()
{
	int in;

	DisableInterrupts();
	
	in=HW_PS2(REG_PS2_KEYBOARD);
	if(in & (1<<BIT_PS2_RECV))
		PS2KeyboardReceive(in);
#ifdef PS2_WRITE
	if(in & (1<<BIT_PS2_CTS))
	{
		if(kbbuffer.out_hw!=kbbuffer.out_cpu)
		{
			HW_PS2(REG_PS2_KEYBOARD)=kbbuffer.outbuf[kbbuffer.out_hw];
			kbbuffer.out_hw=(kbbuffer.out_hw+1) & (PS2_RINGBUFFER_SIZE-1);
		}
	}
#endif

#ifdef PS2_MOUSE
	in=HW_PS2(REG_PS2_MOUSE);
	if(in & (1<<BIT_PS2_RECV))
		PS2MouseReceive(in);
	if(in & (1<<BIT_PS2_CTS))
	{
		if(mousebuffer.out_hw!=mousebuffer.out_cpu)
		{
			HW_PS2(REG_PS2_MOUSE)=mousebuffer.outbuf[mousebuffer.out_hw];
			mousebuffer.out_hw=(mousebuffer.out_hw+1) & (PS2_RINGBUFFER_SIZE-1);
		}
	}
#endif
	GetInterrupts();	// Clear interrupt bit
	EnableInterrupts();
}

void PS2Init()
{
	ps2_ringbuffer_init(&kbbuffer);
#ifdef PS2_MOUSE
	ps2_ringbuffer_init(&mousebuffer);
#endif
	SetIntHandler(&PS2Handler);
	ClearKeyboard();
}

