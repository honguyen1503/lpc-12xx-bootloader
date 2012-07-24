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

#include <cr_section_macros.h>
__BSS(RESERVED) char ISR_Buffer[0x100] ; // reserve 0x100 bytes for ISR pointer in RAM
#include "GlobalIncludes.h"


__attribute__ ((section(".fwparam"))) const tFirmwareParamFlash udtFirmwareParamFlash=
{
		"DWARF14_N       ",
		"0002",
		"DWARF14_N       ",
		"0100",
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
};

u8 bInterruptDisableCounter=0;
tBootloaderParamFlash udtBootloaderParamFlash;

int main(void)
{
	__disable_irq();
	LPC_SYSCON->SYSAHBCLKCTRL |= 0xE001005FUL;	//Systems activated
	LPC_SYSCON->SYSMEMREMAP=0x02;				//Activate ISR in Flash, remapping is inactive
	memcpy(&udtBootloaderParamFlash,(void*)0x100,sizeof(tBootloaderParamFlash));	//copy from flash now possible because of deactivated remapping
	LPC_SYSCON->SYSMEMREMAP=0x01;				//Re-Activate ISR in RAM
	*((u32*)(0x10000000 + 0x1FE4))=0;			//Reset Bootloader Variable
	bInterruptDisableCounter=0;
	UARTInit(115200);
	__enable_irq();								//Enable ISR

	InitFreeGlobalTimer();
	while(1)
	{
		if (udtUartReceiveBuffer.xPacketAvailable)
			Parser();
	}
	return 0;
}
