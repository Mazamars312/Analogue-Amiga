/*  Example of a code-generator for an Intel 386 or higher.         */

#include "dt.h"

#define BEI386

/*  This struct can be used to implement machine-specific           */
/*  addressing-modes.                                               */
/*  Not used in this code-generrator.                               */
struct AddressingMode{
    int never_used;
};

/*  The number of registers of the target machine.                  */
#define MAXR 16

/*  Number of commandline-options the code-generator accepts.       */
#define MAXGF 10

/*  If this is set to zero vbcc will not generate ICs where the     */
/*  target operand is the same as the 2nd source operand.           */
/*  This can sometimes simplify the code-generator, but usually     */
/*  the code is better if the code-generator allows it.             */
#define USEQ2ASZ 0

/*  This specifies the smallest integer type that can be added to a */
/*  pointer.                                                        */
#define MINADDI2P INT

/*  If the bytes of an integer are ordered most significant byte    */
/*  byte first and then decreasing set BIGENDIAN to 1.              */
#ifdef BEI386
#define BIGENDIAN 1
#else
#define BIGENDIAN 0
#endif

/*  If the bytes of an integer are ordered lest significant byte    */
/*  byte first and then increasing set LITTLEENDIAN to 1.           */
#ifdef BEI386
#define LITTLEENDIAN 0
#else
#define LITTLEENDIAN 1
#endif

/*  Note that BIGENDIAN and LITTLEENDIAN are mutually exclusive.    */

/*  If switch-statements should be generated as a sequence of       */
/*  SUB,TST,BEQ ICs rather than COMPARE,BEQ ICs set this to 1.      */
/*  This can yield better code on some machines.                    */
#define SWITCHSUBS 0

/*  In optimizing compilation certain library memcpy/strcpy-calls   */
/*  with length known at compile-time will be inlined using an      */
/*  ASSIGN-IC if the size is less or equal to INLINEMEMCPY.         */
/*  The type used for the ASSIGN-IC will be UNSIGNED|CHAR.          */
#define INLINEMEMCPY 1024


