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



#include <cr_section_macros.h>								/*allows setting of CRP (code read protection) by macros*/
__BSS(RESERVED) unsigned char mProgramBuffer[0x100]={0};	/*reserve 0x100 (256) Bytes in RAM at location zero to use it as Interrupt service pointer section*/

#include "includes.h"										/*includes the global header file*/
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;				/*sets read protection so only deleting of the whole code is possible by ISP*/
u8 bInterruptDisableCounter=0;								/*counts how often __disable_irq is used so if its more than once (by interrupt) it's not overridden at the interrupt end*/



__attribute__ ((section(".btlparam"))) tBootloaderParamFlash udtBootloaderParamFlash=	/*the bootloader versioning and CRC is init here. for positioning see linkscript and section btlparam*/
{
	"LPC122x_8kB     ",																	/*Bootloader Name*/
	"0100",																				/*bootloader version*/
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},	/*Hardware Name to be set*/
	{0xFF,0xFF,0xFF,0xFF},																/*Wardware version to be set*/
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}	/*Serial number empty, may be set*/
};

#define WDT_RESET() do{DISABLE_GLOBAL_IRQ();LPC_WWDT->FEED=0xAA;LPC_WWDT->FEED=0x55;ENABLE_GLOBAL_IRQ();}while(0)	/*this resets the watchdog timer*/
void initWatchDog(u32 Time_ms)				/*init the watchdog with time given in ms*/
{
	LPC_WWDT->MOD = 0;
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<0);		/*Disable power down*/
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);	/*enable clock*/

	LPC_WWDT->CLKSEL=0;						/*select system rtc as timer source*/
	LPC_WWDT->TC=Time_ms*12000/4;			/*set timer counter. 12000 is the number of ticks per millisecond. 4 is the hardware prescaler*/
	LPC_WWDT->MOD = 3;						/*set so: Watchdog timeout will cause a chip reset*/

	WDT_RESET();							/*reset counter and start watchdog by doing so*/
}

/*something is wrong here? why is this 1 us? would be right without counting i*/
#define delay_us(x) do{\
			volatile int i=50000;\
			while(i--)\
			{\
				__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");\
				__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");\
				__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");\
				__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");\
				__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");__asm volatile("nop");\
			}\
		}while(0)

int main(void)
{
	__disable_irq();							/*no interrupts so no problems with the remapping or uart startup*/

	SystemInit();								/*initialize the system (given by NXP/Code red)*/
	LPC_SYSCON->SYSMEMREMAP=0x02;				/*set Interrupt Service Routines to Flash (address 0x00000000)*/
	LPC_SYSCON->SYSAHBCLKCTRL |= 0xE001005FUL;	/*set system control variable*/

	initWatchDog(10000);						/*start watchdog with 10 seconds. High times are needed because Flash delete will take a longer time*/

	if (LPC_SYSCON->BODCTRL!=0x13)				/*init Brown out detection*/
		LPC_SYSCON->BODCTRL=0x13;

	InitCRC();									/*initialises the CRC check function*/
	SysTick_Config(SystemCoreClock / 1000);		/*set systick to 1ms*/

	UARTInit(115200);							/*initializes Uart to 115200Baud*/
	bInterruptDisableCounter=0;					/*reset counter for nested DISABLE IRQ*/
	__enable_irq();								/*enable irqs*/;

	Bool xStartBootloader=FALSE;				/*start up bootlaoder code (or application code)?*/
	/*Bootloader start in any of the following cases:*/
	if (
			(*((u32*)pStartOfApplicationCode)==0xFFFFFFFFUL)	/*start of application code is empty (no application at all)*/
			||
			((*(u32*)(0x10000000 + 0x1FE4))==0xB00410AD)		/*the RAM area reserved for it contains the Bootload flag which means the bootloader was called from application*/
			||
			(FLASHPARAM_FirmwareData->dwFirmwareSizeBytes>getFlashSizeInByte()-((u32)(pStartOfApplicationCode)))	/*the code (partly) set in flash is bigger then the free falsh size*/
		)
		xStartBootloader=TRUE;
	else
	{
		if (FALSE==checkFirmwareCrc())			/*if not of them perhaps...only check CRC if the other conditions are false so no empty firmware or to long data is checked*/
			xStartBootloader=TRUE;
	}
	*(u32*)(0x10000000 + 0x1FE4)=0;				/*reset bootloader ram flag*/

	if (FALSE==xStartBootloader)				/*dont start bootloader so start application*/
	{
#if START_BOOTLOADER_ON_IIII					/*in this case the bootloader shall wait a moment and accept an 'i' in this time to start the bootloader*/

		u32 dwStartUpClock=dwClockValue+50;		/*get actual time +50 ms*/
		if (dwStartUpClock<50)
			dwStartUpClock++;					/*prevents value zero and makes it 50ms even in case of overrun*/
		while (dwClockValue<dwStartUpClock)		/*for 50 ms*/
		{
			if (dwNumberOfInputByte)			/*if there is something on the uart*/
			{
				if (mInputBytes[0]=='i')		/*if it's an 'i' */
				{
					UartSendByte('L');			/*answer with 'L' as acknowledge*/
					delay_ms(5);				/*wait for some time to gie the sender time to stop*/
					dwNumberOfInputByte=0;		/*delete all data*/
					xStartBootloader=TRUE;
					break;
				}
				else
					dwNumberOfInputByte=0;		/*if there is anything else on the line just delete everything to minimize the danger of buffer overflow*/
			}
		}
	}
	if (FALSE==xStartBootloader)						/*if the bootloader needs no start start application*/
	{
#endif
		__disable_irq();		/*no errors from interrupts*/
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
		asm volatile("mov pc, r0");					/*thats all, application code starts now*/
#if START_BOOTLOADER_ON_IIII
	}
#endif
	else		/*else the code goes on meaning start bootloader*/
	{
		if (LPC_SYSCON->SYSRESSTAT&(1U<<3))			/*brown out*/
			sendAnswer(0x00,ecBrownOut,NULL,0);
		else if (LPC_SYSCON->SYSRESSTAT&(1U<<2))	/*watchdog*/
			sendAnswer(0x00,ecWatchDog,NULL,0);
		LPC_SYSCON->SYSRESSTAT=0x1F;				/*reset flags*/

		ExpandKey (mAES_Key, mExpandedKey);			/*initialize the AES by expanding the (constant) key*/
		while(1)
		{
			WDT_RESET();							/*first reset so its done between start up and first command for sure*/
			Parser();								/*check if there are any commands to serve*/
		}
		return 0;									/*never reach this*/
	}
}
