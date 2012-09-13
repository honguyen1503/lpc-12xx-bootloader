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
	"0100",
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};

#define WDT_RESET() do{DISABLE_GLOBAL_IRQ();LPC_WWDT->FEED=0xAA;LPC_WWDT->FEED=0x55;ENABLE_GLOBAL_IRQ();}while(0)
void initWatchDog(u32 Time_ms)
{
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<0);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);

	LPC_WWDT->CLKSEL=0;
	LPC_WWDT->TC=Time_ms*12000/4;
	LPC_WWDT->MOD |= 1;

	WDT_RESET();
}



int main(void)
{
	DISABLE_GLOBAL_IRQ();

	SystemInit();
	LPC_SYSCON->SYSMEMREMAP=0x02;				//set Interrupt Service Routines to Flash (address 0x00000000)
	LPC_SYSCON->SYSAHBCLKCTRL |= 0xE001005FUL;	//set system control variable

	initWatchDog(10000);

	if (LPC_SYSCON->BODCTRL!=0x13)
		LPC_SYSCON->BODCTRL=0x13;
	__enable_irq();
	bInterruptDisableCounter=0;


	InitCRC();
	SysTick_Config(SystemCoreClock / 1000);		//set systick to 1ms
	__disable_irq();
	UARTInit(115200);							//initializes Uart to 115200Baud
	bInterruptDisableCounter=0;					//reset counter for nested DISABLE IRQ
	__enable_irq();								//enable irqs

	Bool xStartBootloader=FALSE;
	if (
			(*((u32*)pStartOfApplicationCode)==0xFFFFFFFFUL)
			||
			((*(u32*)(0x10000000 + 0x1FE4))==0xB00410AD)
			||
			(FLASHPARAM_FirmwareData->dwFirmwareSizeBytes>TOTAL_FLASHSIZE-((u32)(pStartOfApplicationCode)))
		)
		xStartBootloader=TRUE;
	else if (FALSE==checkFirmwareCrc())	//only check CRC if the other conditions are false so no empty firmware or to long data is checked
		xStartBootloader=TRUE;
	*(u32*)(0x10000000 + 0x1FE4)=0;		//reset bootloader ram flag

	if (FALSE==xStartBootloader)
	{
#if START_BOOTLOADER_ON_IIII

		u32 dwStartUpClock=dwClockValue+50;
		if (dwStartUpClock<50)
			dwStartUpClock++;	//prevents value zero and makes it 50ms even in case of overrun
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
		//else the code goes on meaning start bootloader
#endif
	}
	else
	{
		if (LPC_SYSCON->SYSRESSTAT&(1U<<3))
			sendAnswer(0x00,ecBrownOut,NULL,0);
		else if (LPC_SYSCON->SYSRESSTAT&(1U<<2))
			sendAnswer(0x00,ecWatchDog,NULL,0);
		LPC_SYSCON->SYSRESSTAT=0x1F;


		ExpandKey (mAES_Key, mExpandedKey);				//initialize the AES by expanding the key
		while(1)
		{
			WDT_RESET();
			Parser();
		}
		return 0;
	}
}
