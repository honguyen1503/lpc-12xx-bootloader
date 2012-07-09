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
__BSS(RESERVED) unsigned char mProgramBuffer[0x100]={0};

#include "includes.h"
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;
u8 bInterruptDisableCounter=0;



__attribute__ ((section(".btlparam"))) tBootloaderParamFlash udtBootloaderParamFlash=
{
	"LPC1226_8kB     ",
	"0005",
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};

int main(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= 0xE001005FUL;	//set system control variable
	LPC_SYSCON->SYSMEMREMAP=0x02;				//set Interrupt Service Routines to Flash (address 0x00000000)


	u32 dwBootloader=*(u32*)(0x10000000 + 0x1FE4);
	*(u32*)(0x10000000 + 0x1FE4)=0;

	if (
			(*((u32*)pStartOfApplicationCode)!=0xFFFFFFFFUL)
			&&
			(dwBootloader!=0xB00410AD)
			&&
			checkFirmwareCrc()
		)

	{
#if START_BOOTLOADER_ON_IIII
		SysTick_Config(SystemCoreClock / 1000);		//set systick to 1ms
		bInterruptDisableCounter=0;					//reset counter for nested DISABLE IRQ
		__enable_irq();								//enable irqs
		u32 dwStartUpClock=dwClockValue+50;
		while (dwClockValue<dwStartUpClock)
		{
			if (dwNumberOfInputByte)
			{
				if (mInputBytes[0]=='i')
				{
					UartSendByte('L');
					delay_ms(10);
					dwNumberOfInputByte=0;
					dwStartUpClock=0;
					break;
				}
				else
					dwNumberOfInputByte=0;
			}
		}
		if (dwStartUpClock)
		{
#endif
			__disable_irq();
			/* Valid application located in the next sector(s) of flash so execute */

			/* Load main stack pointer with application stack pointer initial value,
			   stored at first location of application area */
			asm volatile("ldr r0, =0x2000");
			asm volatile("ldr r0, [r0]");
			asm volatile("mov sp, r0");

			/* Load program counter with application reset vector address, located at
			   second word of application area. */
			asm volatile("ldr r0, =0x2004");
			asm volatile("ldr r0, [r0]");
			asm volatile("mov pc, r0");
#if START_BOOTLOADER_ON_IIII
		}
#endif
	}


	UARTInit(115200);							//initializes Uart to 115200Baud

	InitCRC();
	InitAES();
#if !START_BOOTLOADER_ON_IIII
	SysTick_Config(SystemCoreClock / 1000);		//set systick to 1ms
	bInterruptDisableCounter=0;					//reset counter for nested DISABLE IRQ
	__enable_irq();								//enable irqs
#endif
	while(1)
		Parser();
	return 0;
}
