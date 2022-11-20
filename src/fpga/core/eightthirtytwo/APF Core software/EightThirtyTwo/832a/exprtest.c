#include <stdio.h>

#include "expressions.h"
#include "832util.h"


int main(int argc,char **argv)
{
	char *line="(3+_label24)+255*(4+7)&15";
	struct expression *expr;
	struct equate *equ;
	if(argc>1)
		line=argv[1];
	equ=equate_new("_label24",1234);
	expr=expression_parse(line);
	expression_dumptree(expr,0);
	printf("Evaluates to: %d\n",expression_evaluate(expr,equ));
	expression_delete(expr);
	return(0);
}

