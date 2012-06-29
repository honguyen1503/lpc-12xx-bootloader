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

#include "GlobalIncludes.h"
const u8* const sEPY=(const u8* const)("");

void Parser(void)
{
	if (COMPARE_TO("RST"))		//Reset
	{
		if (!CommCmpStrC(sEPY))	//A parameter follows
			SendErrorByCode(ecUnknownParameter);
		else
		{
			SendErrorByCode(ecAcknowledge);
			delay_ms(5);
			NVIC_SystemReset();
			//SCB->AIRCR=0x05FA0004;
		}
	}
	else if (COMPARE_TO("BTL"))//Bootloader
	{
		if (!CommCmpStrC(sEPY))//A parameter follows
			SendErrorByCode(ecUnknownParameter);
		else
		{
			SendErrorByCode(ecAcknowledge);
			delay_ms(10);
			//hard restart mcu so it goes to boatloader
			__disable_irq();
			LPC_SYSCON->SYSMEMREMAP=0x02;
			*(u32*)(0x10000000 + 0x1FE4)=0xB00410AD;
			NVIC_SystemReset();
		}
	}
	else if (COMPARE_TO("REV"))//FirmwareRevision
	{
		if (CommCmpStrC(sEPY))
		{
			CommSendBuf(udtFirmwareParamFlash.mFW_Name,sizeof(udtFirmwareParamFlash.mFW_Name));
			CommSendBuf13(udtFirmwareParamFlash.mFW_Rev,sizeof(udtFirmwareParamFlash.mFW_Rev));						/*Send Firmware Version via UART*/
		}
		else
			SendErrorByCode(ecUnknownParameter);
	}
	else if (COMPARE_TO("RSN"))//Read Serial Number
	{
		if (CommCmpStrC(sEPY))
		{
			CommSendBuf13(FLASHPARAM_BootloaderData->mSerialNumber,sizeof(FLASHPARAM_BootloaderData->mSerialNumber));
		}
		else
			SendErrorByCode(ecUnknownParameter);

	}
	else if (COMPARE_TO("RHR"))//read Hardware Revision
	{
		if (CommCmpStrC(sEPY))
		{
			CommSendBuf13(FLASHPARAM_BootloaderData->mHardwareRevision,sizeof(FLASHPARAM_BootloaderData->mHardwareRevision));
		}
		else
			SendErrorByCode(ecUnknownParameter);
	}
	else//no suitable command at all
	{
		SendErrorByCode(ecUnknownCommand);	//"UCO" anser
	}
	DeleteUartPacket();//delete the packet which called the parser
}
