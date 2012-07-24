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

tTimerValues udtGlobalFreeTimerValue;			/*The structure containing all global values for the timers*/

void UartReceiveTimeoutFunction(void)
{
	udtUartReceiveBuffer.bWrite=udtUartReceiveBuffer.bLastPacketStart;//set the end to the last completly received packet
	udtUartReceiveBuffer.xStopReceivingUntilCR=FALSE;
	SendErrorByCode(ecCommandReceiveTimeout);	// command receive timeout
}

void delay_ms(u32 dwDelayTime)
{
	u32 dwDelayBaseTime=udtGlobalFreeTimerValue.clock;	//get actual time
	if (dwDelayBaseTime>dwDelayBaseTime+dwDelayTime)	//end time if goes over zero
		dwDelayBaseTime=dwDelayBaseTime+dwDelayTime+1;
	else												//else
		dwDelayBaseTime+=dwDelayTime;
	while (udtGlobalFreeTimerValue.clock!=dwDelayBaseTime);	//wait until time's up
}

void InitFreeGlobalTimer(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	InitFreeGlobalTimerFunctions(TimerUartReceive,				&UartReceiveTimeoutFunction);
}

/*
 * This function connect a function number and a function, which will be called, when the timer times out
 */
Bool InitFreeGlobalTimerFunctions(u8 bFunctionNumber, voidfunction udtFunc)
{
	if (bFunctionNumber<TIMERNUMBEROFFUNCTIONS)							/*function is defined*/
	{
		udtGlobalFreeTimerValue.functionArray[bFunctionNumber]=udtFunc;	/*set function pointer*/
		return TRUE;
	}
	else
		return FALSE;
}


/*
 * This function is used to control the multi-use timer
 * Max value for variable "Time" is 0x0FFFFFFF! 0x10000000 or higher will return false
 */
Bool CallTimedFunction(u8 bFunctionNumber, u32 dwTime)
{
	if (dwTime>0x0FFFFFFF) /*So the overflow is recognized it needs to be smaller than half max value. This is much harder, but the value is more than high enough, this contains days*/
		return FALSE;

	if (bFunctionNumber<TIMERNUMBEROFFUNCTIONS)				/*function is defined*/
	{
		if (0==dwTime)														/*dwTime==0 means deactivate the function*/
			udtGlobalFreeTimerValue.functionClockValue[bFunctionNumber]=0;	/*deactivate it (zero will never occure, see interrupt above)*/
		else
		{
			udtGlobalFreeTimerValue.functionClockValue[bFunctionNumber]=udtGlobalFreeTimerValue.clock+dwTime;	/*execution time is actual time plus the time given*/
			if (udtGlobalFreeTimerValue.functionClockValue[bFunctionNumber]<dwTime) 							/*moved to or over zero*/
			{
				udtGlobalFreeTimerValue.functionClockValue[bFunctionNumber]++;  								/*so add one because zero is unused so there's one number missing*/
			}
		}
		return TRUE;
	}
	else /*bFunctionNumber is to high*/
		return FALSE;
}
void SysTick_Handler(void)
{
	udtGlobalFreeTimerValue.clock++;										/*count up*/
	if (udtGlobalFreeTimerValue.clock==0)									/*"jump over" the zero as zero stands for "counter not active"*/
		udtGlobalFreeTimerValue.clock=1;

	for (u8 i=0;i<TIMERNUMBEROFFUNCTIONS;i++)									/*all defined Timer*/
	{
		if (udtGlobalFreeTimerValue.clock==udtGlobalFreeTimerValue.functionClockValue[i])	/*time is off*/
		{
			udtGlobalFreeTimerValue.functionClockValue[i]=0;				/*deactivate it*/
			udtGlobalFreeTimerValue.functionArray[i]();						/*call the function from array*/
		}
	}
}
