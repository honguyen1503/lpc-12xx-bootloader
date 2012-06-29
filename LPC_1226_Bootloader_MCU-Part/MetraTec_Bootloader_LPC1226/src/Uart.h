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

#ifndef UART_H_
#define UART_H_

//define answer signs

#define USED_UART 0

#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01

#define LSR_RDR         (0x01<<0)
#define LSR_OE          (0x01<<1)
#define LSR_PE          (0x01<<2)
#define LSR_FE          (0x01<<3)
#define LSR_BI          (0x01<<4)
#define LSR_THRE        (0x01<<5)
#define LSR_TEMT        (0x01<<6)
#define LSR_RXFE        (0x01<<7)



extern volatile u32 dwUartRxTimeoutClock;
extern volatile u32 dwNumberOfInputByte;
extern volatile u8 mInputBytes[300];

#define CRCMODE_UARTCOMM_INCL_END 5
#define CRCMODE_UARTCOMM_EXCL_END 1

void UARTInit(uint32_t Baudrate);
void UART1_IRQHandler(void);
void UARTSend(uint32_t portNum, uint8_t *BufferPtr, uint32_t Length);

void ISR_UartRX (void);
#define SendNewline() UartSendByte(13);																		/*Empties the Uart Hardware Receive Buffer*/
void UartSendByte ( u8 bData );
void CommSendDec ( u32 dwData, u8 bMinLength );
void UartSendBuffer ( const u8* const mData, const u8 bLength );
u8 Byte4ToHex ( u8 bData );
void sendU32Hex(u32 sendByte);
#define UartSendHex(x) do{UartSendByte(Byte4ToHex((x)>>4));UartSendByte(Byte4ToHex(x));}while(0)

#endif /* UART_H_ */
