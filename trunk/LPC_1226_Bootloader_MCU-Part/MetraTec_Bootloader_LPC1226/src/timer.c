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

volatile u32 dwClockValue=1;


void SysTick_Handler(void)
{
	dwClockValue++;										/*count up*/
	if (dwClockValue==0)								/*overflow. Allows "0" to mean "off" on uart timeout*/
		dwClockValue=1;
	if(dwUartRxTimeoutClock==dwClockValue)				/*uart timeout*/
		dwUartRxTimeoutClock=0;
}
void delay_ms(u32 dwDelayTime)
{
	u32 dwDelayBaseTime=dwClockValue;					/*get actual time*/
	dwDelayBaseTime+=dwDelayTime;						/*add delay time*/
	if (dwDelayBaseTime<dwDelayTime)					/*overflow. Allows "0" to mean "off" on uart timeout*/
		dwDelayBaseTime++;
	while (dwClockValue!=dwDelayBaseTime);				/*wait until time's up*/
}
