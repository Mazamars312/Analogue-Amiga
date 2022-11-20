/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *                                                                            
 *  Version:    C, Version 2.1
 *                                                                            
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#define __extension__ 
#include "dhry.h"
//#include <sys/null.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
//#include "tiny_printf.h"
#include <stdio.h>
#ifndef __GNUC__
#include "timer.h"
#endif


/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

Enumeration     Func_1 ();
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

/* variables for time measurement: */

#ifdef TIMES
// struct tms      time_info;
                /* see library function "times" */
#define Too_Small_Time 120
                /* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
extern long     time();
                /* see library function "time"  */
#define Too_Small_Time 2
                /* Measurements should last at least 2 seconds */
#endif

long           Begin_Time,
                End_Time,
                User_Time;
long            Microseconds,
                Dhrystones_Per_Second,
                Vax_Mips;
                
/* end of variables for time measurement */

int             Number_Of_Runs = 1;


volatile long _readMilliseconds()
{
#ifdef __GNUC__
	static int t=0;
	return t+=100;
#else
	return(HW_TIMER(REG_MILLISECONDS));
#endif
}

#if 0
#define strcpy _strcpy

_strcpy(char *dst,const char *src)
{
	while(*dst++=*src++);
}
#endif

Rec_Type rec1;
Rec_Type rec2;


// Keep anything remotely large off the stack...
Str_30          Str_1_Loc;
Str_30          Str_2_Loc;

int thread2main ()
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
        One_Fifty       Int_1_Loc;
  REG   One_Fifty       Int_2_Loc;
        One_Fifty       Int_3_Loc;
  REG   char            Ch_Index;
        Enumeration     Enum_Loc;
  REG   int             Run_Index;

  /* Initializations */

//  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
//  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

  Next_Ptr_Glob = &rec1;
  Ptr_Glob = &rec2;

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;


  strcpy (Ptr_Glob->variant.var_1.Str_Comp, 
          "DHRYSTONE PROGRAM, SOME STRING");
  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");
//	printf("%s\n",Ptr_Glob->variant.var_1.Str_Comp);
  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */
#if 0
  printf ("\n");
  printf ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  printf ("\n");
  if (Reg)
  {
    printf ("Program compiled with 'register' attribute\n");
    printf ("\n");
  }
  else
  {
    printf ("Program compiled without 'register' attribute\n");
    printf ("\n");
  }
  Number_Of_Runs;

  printf ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);
#endif
  /***************/
  /* Start timer */
  /***************/

#if 0
#ifdef TIMES
  times (&time_info);
  Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  Begin_Time = time ( (long *) 0);
#endif
#else
  Begin_Time = _readMilliseconds();
#endif
  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {
    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */
  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/
  
#if 0
#ifdef TIMES
  times (&time_info);
  End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  End_Time = time ( (long *) 0);
#endif
#else
//  End_Time = _readMilliseconds();
#endif

#define checkparam(n,v,d) if(v!=d) printf("Error %s is %d but should be %d\n",n,v,d);
#define checksparam(n,v,d) if(strcmp(v,d)) printf("Error %s is %s but should be %s\n",n,v,d);


//printf("Checking results...\n");

// checkparam("Int_Glob",Int_Glob,5);

checkparam("Bool_Glob",Bool_Glob,1);
#if 0
checkparam("Ch_1_Glob",Ch_1_Glob,'A');
checkparam("Ch_2_Glob",Ch_2_Glob,'B');
checkparam("Arr_1_Glob[8]",Arr_1_Glob[8],7);
checkparam("Arr_2_Glob[8][7]",Arr_2_Glob[8][7],Number_Of_Runs+10);
checkparam("Next_Ptr_Glob->Ptr_Comp",Next_Ptr_Glob->Ptr_Comp,Ptr_Glob->Ptr_Comp);
checkparam("Discr",Ptr_Glob->Discr,0);
checkparam("Enum_Comp",Ptr_Glob->variant.var_1.Enum_Comp,2);
checkparam("Int_Comp",Ptr_Glob->variant.var_1.Int_Comp,17);
checksparam("Str_Comp",Ptr_Glob->variant.var_1.Str_Comp,"DHRYSTONE PROGRAM, SOME STRING");
checkparam("next->Discr",Next_Ptr_Glob->Discr,0);
checkparam("next->Enum_Comp",Next_Ptr_Glob->variant.var_1.Enum_Comp,1);
checkparam("next->Int_Comp",Next_Ptr_Glob->variant.var_1.Int_Comp,18);
checksparam("next->Str_Comp",Ptr_Glob->variant.var_1.Str_Comp,"DHRYSTONE PROGRAM, SOME STRING");
checkparam("Int_1_Loc",Int_1_Loc,5);
checkparam("Int_2_Loc",Int_2_Loc,13);
checkparam("Int_3_Loc",Int_3_Loc,7);
checkparam("Enum_Loc",Enum_Loc,1);
checksparam("Str_1_Loc",Str_1_Loc,"DHRYSTONE PROGRAM, 1'ST STRING");
checksparam("Str_2_Loc",Str_2_Loc,"DHRYSTONE PROGRAM, 2'ND STRING");


  User_Time = End_Time - Begin_Time;
  printf ("User time: %d\n", (int)User_Time);
  
  if (User_Time < Too_Small_Time)
  {
    printf ("Measured time too small to obtain meaningful results\n");
    printf ("Please increase number of runs\n");
    printf ("\n");
  }
/*   else */
  {
#if 0
#ifdef TIME
    Microseconds = (User_Time * Mic_secs_Per_Second )
                        /  Number_Of_Runs;
    Dhrystones_Per_Second =  Number_Of_Runs / User_Time;
    Vax_Mips = (Number_Of_Runs*1000) / (1757*User_Time);
#else
    Microseconds = (float) User_Time * Mic_secs_Per_Second 
                        / ((float) HZ * ((float) Number_Of_Runs));
    Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
                        / (float) User_Time;
    Vax_Mips = Dhrystones_Per_Second / 1757.0;
#endif
//#else
    Microseconds = (1000*User_Time) / (Number_Of_Runs*50000);
    Dhrystones_Per_Second =  (Number_Of_Runs*100000000) / User_Time;
    Vax_Mips = (Number_Of_Runs*56900000) / User_Time;
//#endif 
    printf ("Microseconds for one run through Dhrystone: ");
    printf ("%d \n", (int)Microseconds);
    printf ("Dhrystones per Second:                      ");
    printf ("%d \n", (int)Dhrystones_Per_Second);
    printf ("VAX MIPS rating * 1000 = %d \n",(int)Vax_Mips);
    printf ("\n");
#endif
  }
 #endif
  return 0;
}


Proc_1 (Ptr_Val_Par)
/******************/

REG Rec_Pointer Ptr_Val_Par;
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */
  
  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


Proc_2 (Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */

One_Fifty   *Int_Par_Ref;
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


Proc_3 (Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

Rec_Pointer *Ptr_Ref_Par;

{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif


