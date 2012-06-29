/*
Copyright (c) 2012 Martin Koehler, metraTec GmbH (metraTec.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", 
WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* INSERT HEADER */
INCLUDE "MetraTec_Bootloader_LPC1226_Release_lib.ld"

MEMORY
{
  /* Define each memory region */
  MFlash96 (rx) : ORIGIN = 0x0, LENGTH = 0x2000
  RamLoc8 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x1FE0 /* 8k */

}
  /* Define a symbol for the top of each memory region */
  __top_MFlash96 = 0x0 + 0x2000;
  __top_RamLoc8 = 0x10000000 + 0x1FE0;


ENTRY(${start_symbol})

SECTIONS
{
	/* INSERT TEXT_SECTION */

	/* MAIN TEXT SECTION */	
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))
		. = 0xC0;
		_btlparam = .;
		KEEP(*(.btlparam)) 
		
		/* INSERT SECTION_TABLE */

		*(.after_vectors*)
		
		/* INSERT CRP */
		*(.text*)
		/* INSERT RODATA */
		
		/* INSERT CPP_INFO */
	} > ${CODE}

	/* INSERT EXDATA */
	
	_etext = .;
		
	/* INSERT DATA_SECTION */
	
	/* MAIN DATA SECTION */

	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
	} > ${DATA}

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} > ${DATA} AT>${CODE}

	/* INSERT BSS_SECTION */

	/* MAIN BSS SECTION */
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		PROVIDE(end = .);
	} > ${DATA}
	
	    /* Add no init section! */
    /* non initialized variables */
    .noinit (NOLOAD) :
    {
        *(.noinit)
    } > RamLoc8
	
	PROVIDE(${heap_symbol} = .);
	PROVIDE(_vStackTop = __top_${DATA} - ${STACK_OFFSET});
}

