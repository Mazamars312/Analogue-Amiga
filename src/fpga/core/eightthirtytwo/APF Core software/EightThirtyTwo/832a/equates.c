/*
	equates.c

	Copyright (c) 2019,2020 by Alastair M. Robinson

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
#include <stdlib.h>
#include <string.h>

#include "832util.h"
#include "equates.h"

struct equate *equate_new(const char *identifier, int value)
{
	struct equate *result=0;
	if(result=(struct equate *)malloc(sizeof(struct equate)))
	{
		result->identifier=strdup(identifier);
		result->value=value;
		result->next=0;
	}
	return(result);	
}


void equate_delete(struct equate *equ)
{
	if(equ)
	{
		if(equ->identifier)
			free(equ->identifier);
		free(equ);
	}
}


int equate_getvalue(struct equate *equ,struct equate *equatelist)
{
	if(equ)
	{
		return(equ->value);
	}
	exit(1);
}
