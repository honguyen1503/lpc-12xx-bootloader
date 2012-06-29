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


tErrorCode eraseAllFlash (void)
{
	typedef void (*IAP)(u32* command, u32* result);
	IAP iapFunction=(IAP)IAP_LOCATION;
	u32 commandPrepare[5]={50,2,23,0,0};
	u32 resultPrepare[4]={0};
	u32 commandErase[5]={52,2,23,(SystemCoreClock+500)/1000,0};
	u32 resultErase[4]={0};

	DISABLE_GLOBAL_IRQ();
	resultPrepare[0]=0xFF;
	iapFunction(commandPrepare,resultPrepare);

	ENABLE_GLOBAL_IRQ();
	if (resultPrepare[0]!=0)
		return ecFailure;
	DISABLE_GLOBAL_IRQ();
	iapFunction(commandErase,resultErase);
	ENABLE_GLOBAL_IRQ();

	if (resultErase[0]!=0)
		return ecFailure;
	return ecSuccess;
}

tErrorCode writeFlash(u8* mBuffer, u32 dwLength, u8* pTargetArea)
{
	if (dwLength==0)
		return ecSuccess;

	typedef void (*IAP)(u32* command, u32* result);
	IAP iapFunction=(IAP)IAP_LOCATION;

	u32 commandPrepare[5]={50,((u32)(pTargetArea))>>12,((u32)(pTargetArea))>>12};
	u32 resultPrepare[4]={0};

	u32 commandCopy[5]={51,(u32)pTargetArea,(u32)mBuffer,dwLength,(SystemCoreClock+500)/1000};
	u32 resultCopy[4]={0};

	DISABLE_GLOBAL_IRQ();
	resultPrepare[0]=0xFF;
	iapFunction(commandPrepare,resultPrepare);
	ENABLE_GLOBAL_IRQ();

	if (resultPrepare[0]!=0)
		return ecFailure;

	DISABLE_GLOBAL_IRQ();
	resultCopy[0]=0xFF;
	iapFunction(commandCopy,resultCopy);

	ENABLE_GLOBAL_IRQ();

	if (resultCopy[0]!=0)
		return ecFailure;

	return ecSuccess;
}
