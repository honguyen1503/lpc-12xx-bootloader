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

void InitCRC (void)
{
	LPC_SYSCON->PRESETCTRL |= (0x1<<9);		/*disable crc reset*/
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);	/*enable crc clocking*/
}

void GetCRC_CCITT(const u32 dwCrcMode,const  u8* const mData, const u32 dwLength, u16 * pCRC)
{
	LPC_CRC->MODE&=~(0x3F);				/*remove 6 bits*/
	LPC_CRC->MODE|=(dwCrcMode<<2);		/*move and add to the correct positon*/

	LPC_CRC->SEED=*pCRC;				/*set init value*/
	for (int j=0;j<dwLength;j++)		/*for the whole length*/
		LPC_CRC->WR_DATA_8=mData[j];	/*add data to CRC*/
	*pCRC=LPC_CRC->SUM;					/*return the CRC value*/
}

/*
 * returns true if the CRC is ok so there is a suitable Firmware
 */
Bool checkFirmwareCrc(void)
{
	/*if the CRC value in met-file is set AND the CRC in the Firmware is identical to the saved CRC value (about getting this value see below) */
	if ((FLASHPARAM_FirmwareData->dwFirmwareCrcInMet!=0xFFFFFFFF)&&(FLASHPARAM_FirmwareData->dwFirmwareCrcInMet==FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu))
		return TRUE;

	/*
	 * 	CRC of Code was not computed by now so it's the default (important value for program code to set it to 0xFFFFFFFF.
	 * 	Just  using the same value is NOT sufficient because the flash must be writable!
	 */
	/*If the value is the CRC of the application is not computed*/
	if (FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu==0xFFFFFFFF)
	{
		u16 wCRC=0xFFFF;	/*use preset value*/
		u32 dwCRC;			/*variable to set Crc to 4 Byte sized memory area*/
		GetCRC_CCITT(		/*get the CRC first part (pre CRC saving area)*/
				CRCMODE_UARTCOMM_EXCL_END,																/*CRC mode allowing getting the next part after*/
				pStartOfApplicationCode,																/*Start of computation address is the start of application address*/
				(u32)((&(FLASHPARAM_FirmwareData->dwFirmwareCrcInMet)))-(u32)(pStartOfApplicationCode),	/*length is the difference between the start address and the address of the first CRC data*/
				&wCRC);																					/*the variable to get the start value from and to save the crc value to*/
		u8* pAfterCRC=((u8*)(FLASHPARAM_FirmwareData))+sizeof(tFirmwareParamFlash);						/*pointer to the area after CRC*/
		GetCRC_CCITT(																/*get the CRC second part (post CRC saving area)*/
				CRCMODE_UARTCOMM_INCL_END,											/*finalizing Mode*/
				pAfterCRC,															/*starts after CRC*/
				(FLASHPARAM_FirmwareData->dwFirmwareSizeBytes)-(((u32)pAfterCRC)-((u32)pStartOfApplicationCode)),	/*length is the length of the whole code reduced by the already computed part and the bootloader*/
				&wCRC);																/*the variable to get the start value from and to save the crc value to*/
		dwCRC=wCRC;																	/*copy to 4Byte variable. This is the smallest number of bytes to write to flash afaik*/

		/*write CRC value to the FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu flash area so it will be used in the future*/
		writeFlash((u8*)&dwCRC,sizeof(FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu),(u8*)(&(FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu)));

//		__disable_irq();
//		delay_us(10000);	//TODO: WHY IS THIS DELAY NEEDED?!?
//		sendU32Hex(NVIC->ISPR[0]);
//		sendU32Hex(SCB->ICSR);
//		__enable_irq();
//		__asm volatile("nop");
//		delay_us(10000);	//TODO: WHY IS THIS DELAY NEEDED?!?
//		__disable_irq();

		/*try again, same check as at the start*/
		if (FLASHPARAM_FirmwareData->dwFirmwareCrcInMet==FLASHPARAM_FirmwareData->dwFirmwareCrcOnMcu)
			return TRUE;
	}
	return FALSE;	/*return false if none of the return true cases (crc matches) occured*/
}
