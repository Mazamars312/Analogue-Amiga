/* First steps towards LONG LONG and FLOAT support.
   832 is not well suited to handling these types. */

char *use_libcall(int code,int typf1, int typf2)
{
	char *result=0;
//	printf("Querying libcall for %d, %d, %d\n",code,typf1,typf2);
	switch(code)
	{
		case CONVERT:
			if((typf1&NU)==LLONG && (typf2&NU)==INT)
				return("__conv_ll_int\n");
			if((typf1&NU)==(UNSIGNED|LLONG) && (typf2&NU)==(UNSIGNED|INT))
				return("__conv_ull_uint\n");
			if((typf1&NU)==INT && (typf2&NU)==LLONG)
				return("__conv_int_ll\n");
			if((typf1&NU)==(UNSIGNED|INT) && (typf2&NU)==(UNSIGNED|LLONG))
				return("__conv_uint_ull\n");
		case ADD:
			if((typf1&NU)==LLONG)
				return("__add_ll_ll\n");
			if((typf1&NU)==(UNSIGNED|LLONG))
				return("__add_ull_ull\n");
		case MULT:
			if((typf1&NU)==LLONG && (typf2&NU)==INT)
				return("__mul_ll_int\n");
			if((typf1&NU)==(UNSIGNED|LLONG) && (typf2&NU)==(UNSIGNED|INT))
				return("__mul_ull_uint\n");
			if((typf1&NU)==LLONG && (typf2&NU)==LLONG)
				return("__mul_ll_ll\n");
			if((typf1&NU)==(UNSIGNED|LLONG) && (typf2&NU)==(UNSIGNED|LLONG))
				return("__mul_ull_ull\n");
		default:
			break;	
	}
	return(result);
}

void declare_builtins()
{
	declare_builtin("__conv_int_ll",INT,LLONG,0,LLONG,0,1,0);
	declare_builtin("__conv_ull_ull",UNSIGNED|INT,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
	declare_builtin("__add_ll_ll",LLONG,LLONG,0,LLONG,0,1,0);
	declare_builtin("__add_ull_ull",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
	declare_builtin("__mul_ll_ll",LLONG,LLONG,0,LLONG,0,1,0);
	declare_builtin("__mul_ull_ull",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
	declare_builtin("__mul_ll_int",LLONG,LLONG,0,INT,0,1,0);
	declare_builtin("__mul_ull_uint",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|INT,0,1,0);
}

