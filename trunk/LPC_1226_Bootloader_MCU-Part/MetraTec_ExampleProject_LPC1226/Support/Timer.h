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

#ifndef TIMER_H_
#define TIMER_H_

void delay_ms(u32 dwDelayTime);
Bool InitFreeGlobalTimerFunctions(u8 bFunctionNumber,voidfunction udtFunc);	/*Initialize the Free global timer construction. */
void InitFreeGlobalTimer(void);												/*This function connect a function number and a function, which will be called, when the timer times out */
Bool CallTimedFunction(u8 bFunctionNumber, u32 dwTime);						/*calls the multi-use timer. Needs the function number (switch-case inside, uses #defines)and the time in milliseconds */
void SysTick_Handler(void);

#endif /* TIMER_H_ */
