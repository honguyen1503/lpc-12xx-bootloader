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

#include "includes.h"

u32 getFlashSizeInByte (void)
{
	switch (LPC_SYSCON->DEVICE_ID)
	{
		case 0x3670002B:	//1227
			return 128*1024;
		case 0x3660002B:	//1226
			return 96*1024;
		case 0x3652002B:	//1225 /321
			return 80*1024;
		case 0x3650002B:	//1225 /301
			return 64*1024;
		case 0x3642C02B:	//1224 /121
			return 48*1024;
		case 0x3640C02B:	//1224 /101
			return 32*1024;
		default:
			return 32*1024;
	}
}

/*This function tries to delete the flash using IAP commands*/
tErrorCode eraseAllFlash (void)
{
	typedef void (*IAP)(u32* command, u32* result);					/*define a function type for IAP*/
	IAP iapFunction=(IAP)IAP_LOCATION;								/*declare a function pointer to IAP location*/
	u32 commandPrepare[5]={50,BOOTLOADER_FLASHSIZE>>12,((getFlashSizeInByte())>>12)-1,0,0};							/*define the command sequence for 'prepare' IAP command*/
	u32 resultPrepare[4]={0xFF};									/*define the answer buffer for prepare command*/
	u32 commandErase[5]={52,BOOTLOADER_FLASHSIZE>>12,((getFlashSizeInByte())>>12)-1,(SystemCoreClock+500)/1000,0};		/*define the command sequence for 'erase' IAP command*/
	u32 resultErase[4]={0xFF};										/*define the answer buffer for erase command*/

	DISABLE_GLOBAL_IRQ();											/*disable interrupt so nothing can disturb the IAP*/
	iapFunction(commandPrepare,resultPrepare);						/*start prepare command*/
	ENABLE_GLOBAL_IRQ();											/*when done allow interrupts again*/

	if (resultPrepare[0]!=0)										/*check if successful*/
		return ecFailure;											/*if not: sende error*/

	DISABLE_GLOBAL_IRQ();											/*disable interrupt so nothing can disturb the IAP*/
	iapFunction(commandErase,resultErase);							/*start erase command*/
	ENABLE_GLOBAL_IRQ();											/*when done allow interrupts again*/

	if (resultErase[0]!=0)											/*check if successful*/
		return ecFailure;											/*if not: sende error*/
	return ecSuccess;										/*else everything worked fine*/
}

/* write data to flash using IAP. The area should be free (see erase flash)*/
tErrorCode writeFlash(u8* mBuffer, u32 dwLength, u8* pTargetArea)
{
	if (dwLength==0)
		return ecSuccess;

	typedef void (*IAP)(u32* command, u32* result);					/*define a function type for IAP*/
	IAP iapFunction=(IAP)IAP_LOCATION;								/*declare a function pointer to IAP location*/

	u32 commandPrepare[5]={50,((u32)(pTargetArea))>>12,((u32)(pTargetArea))>>12};	/*define the command sequence for 'prepare' IAP command*/
	u32 resultPrepare[4]={0xFF};									/*define the answer buffer for prepare command*/

	u32 commandCopy[5]={51,(u32)pTargetArea,(u32)mBuffer,dwLength,(SystemCoreClock+500)/1000};	/*define the command sequence for 'copy' IAP command*/
	u32 resultCopy[4]={0};											/*define the answer buffer for copy command*/

	DISABLE_GLOBAL_IRQ();											/*disable interrupt so nothing can disturb the IAP*/
	iapFunction(commandPrepare,resultPrepare);						/*start prepare command*/
	ENABLE_GLOBAL_IRQ();											/*when done allow interrupts again*/

	if (resultPrepare[0]!=0)										/*check if successful*/
		return ecFailure;											/*if not: send error*/

	DISABLE_GLOBAL_IRQ();											/*disable interrupt so nothing can disturb the IAP*/
	iapFunction(commandCopy,resultCopy);							/*start copy command*/
	ENABLE_GLOBAL_IRQ();											/*when done allow interrupts again*/

	if (resultCopy[0]!=0)											/*check if successful*/
		return ecFailure;											/*if not: send error*/
	return ecSuccess;										/*else everything worked fine*/
}
