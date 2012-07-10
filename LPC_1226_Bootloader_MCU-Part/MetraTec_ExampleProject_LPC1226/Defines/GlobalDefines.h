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

#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

//extern char ISR_Buffer[0x100];

/* Bit values, BITx = 2^x. Used for Bit operations like if (var & BIT3), which is TRUE if bit3 of var is set or |=BIT6, which sets bit 6*/
#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

#ifndef NULL
#define NULL 0
#endif

#define NumberOfElementsInArray(x) (sizeof(x)/sizeof(x[0]))	/*gives the number of array elements, for example u16 wDummy[7]: sizeof is 14, sizeof wDummy[0]=2, number of elements is 14/2=7*/


#define TIMERNUMBEROFFUNCTIONS		1	/*Number of "free to use" functions*/
#define TimerUartReceive			0	/*shall be used for UART*/

#define ENABLE_GLOBAL_IRQ()  if (bInterruptDisableCounter) {bInterruptDisableCounter--; 	if (!bInterruptDisableCounter) {__enable_irq() ;}}
#define DISABLE_GLOBAL_IRQ() do{__disable_irq();bInterruptDisableCounter++; } while(0)


#endif /* GLOBALDEFINES_H_ */
