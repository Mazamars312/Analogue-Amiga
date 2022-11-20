MEMORY
{
   mem : ORIGIN = 0x0, LENGTH = 0x0010000
}

SECTIONS
{
	. = 0x0;
	_start = .;

	.text.startup :
	{
		KEEP(*(.text.startup)) /* Seed section, allows use of --gc-sections */
	} >mem

	.code : {
	  *(.text)
	  *(.rodata)
	  *(.rodata.*)
	  *(.data)
	} >mem

	.bss : {
		__bss_start__ = . ;
	  *(.bss)
	  *(.bss.*)
	  	__bss_end__ = . ;
	} >mem

	/DISCARD/ : {
	  *(.eh_frame)
	  *(.gnu.build-id)
	} 
}

