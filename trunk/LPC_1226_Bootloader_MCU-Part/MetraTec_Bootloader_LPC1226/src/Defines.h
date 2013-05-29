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

#ifndef DEFINES_H_
#define DEFINES_H_

#define START_BOOTLOADER_ON_IIII 1	/* if this is set the bootloader may also be started by sending an 'i' in the matching timeslot. disabled normally */
#define BOOTLOADER_FLASHSIZE 0x2000	/* Flash size of Bootloader*/
#define USED_UART 0					/* The Uart used by bootloader. May be changed depending on the used hardware*/

#include "stdint.h"					/*standard C lib*/

/*shortervariable type names*/
typedef uint8_t 	u8;
typedef int8_t 		s8;
typedef uint16_t 	u16;
typedef int16_t 	s16;
typedef uint32_t 	u32;
typedef int32_t 	s32;
typedef int64_t		s64;
typedef enum { FALSE = 0, TRUE  = 1} Bool;

extern u8 bInterruptDisableCounter;		/*counts how often __disable_irq is used so if its more than once (by interrupt) it's not overridden at the interrupt end*/
#define ENABLE_GLOBAL_IRQ()  if (bInterruptDisableCounter) {bInterruptDisableCounter--; 	if (!bInterruptDisableCounter) {__enable_irq() ;}}	/*(re-)enable interrupt, nested*/
#define DISABLE_GLOBAL_IRQ() do{__disable_irq();bInterruptDisableCounter++; } while(0)															/*disable interrupt, nested*/

#define NumberOfElementsInArray(x) (sizeof(x)/sizeof(x[0]))			/*gives the length of an array not in byte but elements*/

extern unsigned char mProgramBuffer[0x100];							/*this is the buffer for RAM residing ISR (used in the appilication). Also used as global buffer*/

typedef	struct
{
	const const u8 mBootloaderName[16];					/*contains the bootloader name*/
	const const u8 mBootloaderRevision[4];				/*and its revision*/
	const const u8 mHardwareName[16];					/*the same for hardware*/
	const const u8 mHardwareRevision[4];
	const const u8 mSerialNumber[16];					/*and a serial number of 16 bytes (ascii usage!)*/
}tBootloaderParamFlash;									/*the data of the bootloader*/

typedef struct
{
	const const u8 mFW_Name[16];						/*application (firmware) name*/
	const const u8 mFW_Rev[4];							/*its revision*/
	const const u8 mHW_Req_Name[16];					/*hardware requirement name*/
	const const u8 mHW_Req_Rev[4];						/*and revision*/
	const const u32 dwFirmwareSizeBytes;				/*the Firmware size in bytes, needed for the crc check*/
	const const u32 dwFirmwareCrcInMet;					/*the firmware crc is save in the met file (computed in generation process of met)*/
	const const u32 dwFirmwareCrcOnMcu;					/*the local saved Crc, saved after the first compiting of Crc. if everything works fine the two CRCs are identical*/
}tFirmwareParamFlash;									/*the data of the application*/

extern tBootloaderParamFlash udtBootloaderParamFlash;	/*a variable located on flash, reading is no problem, writing only by IAP*/
#define FLASHPARAM_FirmwareData ((tFirmwareParamFlash*)(pStartOfApplicationCode+0x100))	/*pointer to the start of firmware data*/

#endif /* DEFINES_H_ */
