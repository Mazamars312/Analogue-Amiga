/*
	expressions.c

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
#include <string.h>
#include <stdlib.h>

#include "expressions.h"
#include "832util.h"

struct operatordef {
	char *key;
	enum operator op;
	enum optype type;
} operators[]=
{
	{"(",OP_PARENTHESES,OPTYPE_PAREN},
	{"*",OP_MULTIPLY,OPTYPE_BINARY},
	{"/",OP_DIVIDE,OPTYPE_BINARY},
	{"%",OP_MODULO,OPTYPE_BINARY},
	{"<<",OP_SHLEFT,OPTYPE_BINARY},
	{">>",OP_SHRIGHT,OPTYPE_BINARY},
	{"+",OP_ADD,OPTYPE_BINARY},
	{"-",OP_SUBTRACT,OPTYPE_BINARY},
	{"&",OP_AND,OPTYPE_BINARY},
	{"|",OP_OR,OPTYPE_BINARY},
	{"^",OP_XOR,OPTYPE_BINARY},
	{"-",OP_NEGATE,OPTYPE_UNARY},
	{"~",OP_INVERT,OPTYPE_UNARY},
	{"!",OP_INVERT,OPTYPE_UNARY},
	{0,OP_VALUE,OPTYPE_NULL},
	{0,OP_END,OPTYPE_NULL},
	{0,OP_NONE,OPTYPE_NULL}
};



/*

Expression parsing
Need to support the following binary operators:
+, -, *, /, &, |, ^, <<, >>
Also unary operators: -, ~

Build a tree structure for expression parsing.

Scan the expression from left to right searching for operators.
Create a leaf from everything up to the first operator.
If none is found we have a value, set op and value fields accordingly.
If an operator is found, create a 2nd value leaf from anything up to the next operator.
If another operator was found, compare priorities:
  If priority is higher than the first operator, recursively create a new expression with current expression as its left branch.
If priorty is higher than the first operator, set this as a leaf and return to caller.
If no further operators are found, set this as a leaf and return to caller.
If brackets are found, anything within the brackets should be evaluated as a separate expression.

FIXME - need to deal with negation and inversion.

*/

static struct expr_linebuffer *linebuffer_new(char *buf)
{
	struct expr_linebuffer *result;
	if(result=(struct expr_linebuffer *)malloc(sizeof(struct expr_linebuffer)))
	{
		result->cursor=0;
		result->buf=buf;
		result->currentop=OP_NONE;
	}
	return(result);
}

static void linebuffer_delete(struct expr_linebuffer *lb)
{
	if(lb)
		free(lb);
}


/* Function to extract a subexpression into a new linebuffer. 
   Modifies the supplied linebuffer in-place, replacing ‘)’ with a \nul,
   And advances the cursor accordingly.
   Returns either a new linebuffer or null on failure. */

static struct expr_linebuffer *linebuffer_extractsubexpr(struct expr_linebuffer *lb)
{
	struct expr_linebuffer *result=0;
	int parencount=0;
	int i=0;
	int c;
	if(!lb)
		return(0);

	i=lb->cursor;
	if(lb->buf[i]!='(')    /* Not a subexpression? */
		return(0);

	while(c=lb->buf[i])
	{
		if(c=='(')
			++parencount;
		else if (c==')')
		{
			--parencount;
			if(parencount==0)
			{
				debug(1,"Terminating first expression\n");
				lb->buf[i]=0; /* Terminate the subexpression */        
				debug(1,"Creating new lb\n");
				result=linebuffer_new(&lb->buf[lb->cursor+1]);
				lb->cursor=i+1;
				return(result);
			}
		}
		debug(1,"i: %d, parencount: %d\n",i,parencount);
		++i;
	}
	return(result);
}


static struct operatordef *matchoperator(char *str)
{
	int i;
	for(i=0;i<(sizeof(operators)/sizeof(struct operatordef));++i)
	{
		if(operators[i].key)
		{
			if(strncmp(operators[i].key,str,strlen(operators[i].key))==0)
				return(&operators[i]);
		}
		else
			return(0);
	}
}

/* advance the cursor past the next operator, return the operator, and null it out. */
static enum operator expression_findoperator(struct expr_linebuffer *lb)
{
	if(lb && lb->buf)
	{
		struct operatordef *result;
		while(lb->buf[lb->cursor])
		{
			while(lb->buf[lb->cursor]==' ' || lb->buf[lb->cursor]=='\t')
				++lb->cursor;

			if(result=matchoperator(&lb->buf[lb->cursor]))
			{
				int i;
				/*	If we found the operator, set currentop in the linebuffer structure
					and zero out its text representation in the string. */
				debug(1,"Found operation %d\n",result->op);
				lb->currentop=result->op;
				for(i=0;i<strlen(result->key);++i)
					lb->buf[lb->cursor++]=0;
				return(result->op);
			}
			++lb->cursor;
		}
		lb->currentop=OP_END;
		return(0);
	}
}


struct expression *expression_new()
{
	struct expression *result;
	if(result=(struct expression *)malloc(sizeof(struct expression)))
	{
		result->left=result->right=0;
		result->value=0;
		result->op=OP_NONE;
		result->storage=0;
	}
	return(result);
}


void expression_delete(struct expression *expr)
{
	if(expr)
	{
		if(expr->storage)
			free(expr->storage);
		if(expr->left)
			expression_delete(expr->left);
		if(expr->right)
			expression_delete(expr->right);
		free(expr);
	}
}


/*	Create a leaf node from everything up to the next operator, setting the value field
	Fill in the operator field with OP_value.
	Fill in buf with operator, if any.
	Zero-out the operator character(s).
	Advance cursor
*/

static struct expression *expression_buildtree(struct expr_linebuffer *lb);

static struct expression *expression_makeleaf(struct expr_linebuffer *lb)
{
	struct expression *result;
	/* If the expression begins with brackets, extract the subexpression and build a tree from it. */

	while(lb->buf[lb->cursor]==' '||lb->buf[lb->cursor]=='\t')
		lb->cursor++;

	if(lb->buf[lb->cursor]=='(')
	{
		struct expr_linebuffer *subexpr=linebuffer_extractsubexpr(lb);
		result=expression_buildtree(subexpr);
		linebuffer_delete(subexpr);
		expression_findoperator(lb);
	}
	else if(result=expression_new())
	{
		result->value=&lb->buf[lb->cursor];
		result->op=OP_VALUE;
		expression_findoperator(lb);
		if(strlen(result->value)==0) /* Unary operator? */
		{
			result->op=lb->currentop;
			result->left=expression_new();
			result->left->op=OP_VALUE;
			result->left->value="0";
			result->right=expression_new();
			result->right->op=OP_VALUE;
			result->right->value=&lb->buf[lb->cursor];
			expression_findoperator(lb);
			debug(1,"Created unary operator\n");
		}
		else
			debug(1,"created leaf: %s\n",result->value);
	}
	return(result);
}


static struct expression *expression_makerightleaf(struct expression *leftleaf,struct expr_linebuffer *lb)
{
	struct expression *expr,*expr2;
	expr=expression_new();
	expr->left=leftleaf;
	expr->op=lb->currentop;

	while(lb->buf[lb->cursor]==' '||lb->buf[lb->cursor]=='\t')
		lb->cursor++;

	debug(1,"assigning %s to right hand\n",&lb->buf[lb->cursor]);
	expr2=expression_makeleaf(lb);
	/* 	If another operator was found, compare priorities:
		If priority is higher than the first operator, recursively create a new expression
		with current expression as its left branch. */

	while(lb->currentop<expr->op)
	{
		debug(1,"Creating new righthand leaf\n");
		expr2=expression_makerightleaf(expr2,lb);
	}
	expr->right=expr2;
	return(expr);
}


static struct expression *expression_buildtree(struct expr_linebuffer *lb)
{
	struct expression *expr=0;
	struct expression *expr2=0;
	enum operator op;

	debug(1,"Buildtree: %s\n",&lb->buf[lb->cursor]);

	/* 	Scan the expression from left to right searching for operators.
		Create a leaf from everything up to the first operator. */
		
	expr=expression_makeleaf(lb);

	while(lb->currentop!=OP_END)
	{
		/* If an operator is found, create a 2nd value leaf from anything up to the next operator. */
		expr=expression_makerightleaf(expr,lb);
	}
	return(expr);
}


void expression_dumptree(struct expression *expr,int indent)
{
	int i;
	if(expr)
	{
		if(expr->value)
		{
			for(i=0;i<indent;++i) printf("  ");
			printf("Value: %s\n",expr->value);
		}
		if(expr->left)
		{
			for(i=0;i<indent;++i) printf("  ");
			printf("left: -> \n");
			expression_dumptree(expr->left,indent+1);
			for(i=0;i<indent;++i) printf("  ");
			printf("%s\n",operators[expr->op].key ? operators[expr->op].key : "(none)");
		}
		if(expr->right)
		{
			for(i=0;i<indent;++i) printf("  ");
			printf("right: -> \n");
			expression_dumptree(expr->right,indent+1);
		}
	}
}


struct expression *expression_parse(const char *str)
{
	struct expression *expr=0;
	if(str)
	{
		char *buf=strdup(str);
		struct expr_linebuffer *lb=linebuffer_new(buf);
		expr=expression_buildtree(lb);
		if(expr)
			expr->storage=buf;
		linebuffer_delete(lb);
	}
	return(expr);
}


int expression_evaluate(const struct expression *expr,const struct equate *equates)
{
	int result=0;
	char *t;
	if(expr)
	{
		int left=expression_evaluate(expr->left,equates);
		int right=expression_evaluate(expr->right,equates);
		switch(expr->op)
		{
			case OP_VALUE:
				if(strlen(expr->value)==0)
				{
					result=0;
					break;
				}
				result=strtoul(expr->value,&t,0);
				if(t==expr->value && result==0)
				{
					const struct equate *equ=equates;
					/* Not a literal value - search for an equate */
					debug(1,"Hunting for %s\n",expr->value);
					while(equ)
					{
						if(strcmp(expr->value,equ->identifier)==0)
						{
							result=equ->value;
							break;
						}
						equ=equ->next;
					}
					if(!equ)
						asmerror("Undefined value");
				}
				break;

			case OP_MULTIPLY:
				result=left*right;
				break;

			case OP_DIVIDE:
				result=left/right;
				break;

			case OP_MODULO:
				result=left%right;
				break;

			case OP_SHLEFT:
				result=left<<right;
				break;

			case OP_SHRIGHT:
				result=left>>right;
				break;

			case OP_ADD:
				result=left+right;
				break;

			case OP_SUBTRACT:
				result=left-right;
				break;

			case OP_AND:
				result=left & right;
				break;

			case OP_OR:
				result=left | right;
				break;

			case OP_XOR:
				result=left ^ right;
				break;

			case OP_NEGATE:
				result=-right;
				break;

			case OP_INVERT:
				result=~right;
				break;

			default:
				fprintf(stderr,"Expression - unknown op %x\n",expr->op);
				break;
		}

	}
	return(result);
}


int expression_evaluatestring(const char *str,const struct equate *equates)
{
	int result;
	struct expression *expr=expression_parse(str);
	if(getdebuglevel())
		expression_dumptree(expr,0);
	result=expression_evaluate(expr,equates);
	debug(1,"Evaluates to: %ld\n",result);
	expression_delete(expr);
	return(result);
}


