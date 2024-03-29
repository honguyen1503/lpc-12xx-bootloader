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

#ifndef GLOBALVARIABLES_H_
#define GLOBALVARIABLES_H_

extern tUartSendBuffer udtUartSendBuffer;		/*contains the Uart transmitter state. Is accessed by transmitter interrupt and UartSendByte/UartSendBuffer*/
extern tUartReceiveBuffer udtUartReceiveBuffer;	/*contains the Uart receiver state. Is used by receiver interrupt and command parser*/
extern tTimerValues udtGlobalFreeTimerValue;	/*The structure containing all global values for the timers*/
extern u8 bInterruptDisableCounter;				/*counts the number of "interrupt disable" calls so the first "interrupt enable" will not overrule the cascaded Disables*/

extern const tFirmwareParamFlash udtFirmwareParamFlash;	/*contains the Firmware Version, Name etc. at a specific adress so it's accessable from bootloader*/
extern tBootloaderParamFlash udtBootloaderParamFlash;	/*is a local RAM variable containing a COPY of the bootloader Parameter (Name, Version etc)
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 filled at the start of main because the bootloader section is not addressable because every
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 addressing while in ISR-In-RAM-Mode is redirected to the RAM area*/

#endif /* GLOBALVARIABLES_H_ */
