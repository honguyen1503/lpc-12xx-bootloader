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

#define START_BOOTLOADER_ON_IIII 1
#define TOTAL_FLASHSIZE 0x18000
#define USED_UART 0

#include "stdint.h"

typedef uint8_t 	u8;
typedef int8_t 		s8;
typedef uint16_t 	u16;
typedef int16_t 	s16;
typedef uint32_t 	u32;
typedef int32_t 	s32;
typedef int64_t		s64;
typedef enum { FALSE = 0, TRUE  = 1} Bool;

extern u8 bInterruptDisableCounter;
#define ENABLE_GLOBAL_IRQ()  if (bInterruptDisableCounter) {bInterruptDisableCounter--; 	if (!bInterruptDisableCounter) {__enable_irq() ;}}
#define DISABLE_GLOBAL_IRQ() do{__disable_irq();bInterruptDisableCounter++; } while(0)

#define NumberOfElementsInArray(x) (sizeof(x)/sizeof(x[0]))


extern unsigned char mProgramBuffer[0x100];

typedef	struct
{
	const const u8 mBootloaderName[16];
	const const u8 mBootloaderRevision[4];
	const const u8 mHardwareName[16];
	const const u8 mHardwareRevision[4];
	const const u8 mSerialNumber[16];
}tBootloaderParamFlash;

typedef struct
{
	const const u8 mFW_Name[16];
	const const u8 mFW_Rev[4];
	const const u8 mHW_Req_Name[16];
	const const u8 mHW_Req_Rev[4];
	const const u32 dwFirmwareSizeBytes;
	const const u32 dwFirmwareCrcInMet;
	const const u32 dwFirmwareCrcOnMcu;
}tFirmwareParamFlash;

extern tBootloaderParamFlash udtBootloaderParamFlash;
#define FLASHPARAM_FirmwareData ((tFirmwareParamFlash*)(pStartOfApplicationCode+0x100))

#endif /* DEFINES_H_ */
