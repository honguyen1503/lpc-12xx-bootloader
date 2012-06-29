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

void InitCRC (void)
{
	LPC_SYSCON->PRESETCTRL |= (0x1<<9);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
}

void GetCRC_CCITT(u32 dwCrcMode, u8* mData, u32 dwLength, u16 * pCRC)
{
	LPC_CRC->MODE&=~(0x3F);
	LPC_CRC->MODE|=(dwCrcMode<<2);

	LPC_CRC->SEED=*pCRC;
	for (int j=0;j<dwLength;j++)
		LPC_CRC->WR_DATA_8=mData[j];
	*pCRC=LPC_CRC->SUM;

}
