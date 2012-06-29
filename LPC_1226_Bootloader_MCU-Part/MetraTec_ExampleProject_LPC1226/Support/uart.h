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


#ifndef __UART_H 
#define __UART_H

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

void UARTInit(uint32_t Baudrate);
void UART1_IRQHandler(void);

void ISR_UartRX (void);
#define SendNewline() UartSendByte(13,TRUE);
void movePositionForwardInRecvRing(u8* pVariable, u8 bValue);										/*the given pointer will be set to the new value in the receive buffer ring taking care of potential overflow*/
void copyDataFromReceiveRingBufferToBuffer(u8 bStart, u8 length, u8* targetBuffer,u8 OffSet);		/*Copies data from udtUartReceiveBuffer, starting with start+Offset, ending length bytes later, to the targetbuffer. Call by value saves the pointer!*/
u8 RingDiff(u8 start, u8 end); 																		/*Gives the length in Bytes*/
void USART_Flush( void );																			/*Empties the Uart Hardware Receive Buffer*/
void UartSendBuffer ( const const u8* mData, const u8 bLength, const Bool xUseForCRC );				/*Send a Buffer*/
void UartSendByte ( u8 bData, Bool xUseForCRC );
void DeleteUartReceiveBuffer (void); 																/*Delete the whole receive buffer (e.g. after an error occured)*/
void DeleteUartSendBuffer (void); 																	/*Delete the whole send buffer (e.g. after an error occured)*/
void DeleteUartPacket ( void ); 																	/*deletes the first packet in the UartReceiveBuffer and moves the rest to the start*/


//UartFunctions.c
void CommSendStrC ( const u8* const udtString, u8 bSize);														/*Sends a zero-terminated string of progmem-data via Uart*/
#define CommSendStrC13(udtString,bSize) 	do{CommSendStrC(udtString, bSize);SendNewline();}while(0)  /*CommSendStrC + Sending (13), the Line feed*/

void CommSendHex ( u8 bData ); 																	/*Sends one Byte via Uart as two-digit-hex*/
#define CommSendHex13(bData) 				do{CommSendHex(bData);SendNewline();}while(0)		/*Sends one Byte via Uart as two-digit-hex and 0x0D (<CR>)*/

void CommSendDec ( u32 dwData , u8 bMinLength ); 														/*Send Decimal number via Uart with changing appropriate number of digits*/
#define CommSendDec13(udtData,minLength) 	do{CommSendDec(udtData ,minLength);SendNewline();}while(0)	/*sends a decimal number with <CR>*/

void CommSendBufHex ( const u8* const mBuf, const u8 bLen);												/*Sends bLen Bytes from the buffer pBuf via Uart as two-digit-hex'*/
#define CommSendBufHex13(pBuf,bLen) 		do{CommSendBufHex(pBuf,bLen);SendNewline();}while(0)/*CommSendBufHex  + Sending (13), the Line feed*/

#define CommSendBuf(mInBuf,bLen) 			do {UartSendBuffer(mInBuf,bLen,TRUE);}while(0)														/*Sends Len Bytes from buffer pInbuf directly via Uart.*/
#define CommSendBuf13(mInbuf,bLen) 			do{CommSendBuf(mInbuf,bLen);SendNewline();}while(0)	/*CommSendBuf  + Sending (13), the Line feed*/

/**Compares the string udtStr with the UartReceiveBuffer and returns TRUE if the match.
 * If they match the Compare pointer is moved to the following data. If not nothing is changed!
 */
Bool CommCmpStrC ( const u8* const udtString );

/**Decodes the Data in the UartReceiveBuffer beginning at the compare pointer.
 * The Data are written to the buffer beginning with the byte 1. Byte 0 gets the size of the hexdata in nibbles (half bytes).
 * It returns TRUE if the data can be interpreted as hex (no chars above F etc.), else FALSE
 * The received data is written in a ringbuffer being part of the uart receive buffer (see FUartReceiveBuffer  declaration)
 */
Bool CommDecodeHex ( void );

#define CommGetDataH(pData) CommGetDataBaseFunction(pData,16,8)/*Get Data hexadecimal, returns true if hex (only 0-F), 8 digits (4 bytes, u32)*/
#define CommGetDataD(pData) CommGetDataBaseFunction(pData,10,10)/*Get Data hexadecimal, returns true if decimal (only 0-9), 10 digits (more than 4 bytes, u32)*/

Bool CommGetDataBaseFunction ( u32* pData, u8 bBase, u8 bMaxIndex); //Basic function used by CommGetDataH and CommGetDataD. Can also get digital, octal etc.

/*Support functions not for direct use*/
u8 LowerCase 	( u8  bData ); /*returns the lower case of a char (a on A, a on a, b on B or b, z on Z or z etc.)*/
Bool HexToByte4 ( u8* pData ); /*returns TRUE if char can be interpreted as hexdigit and sets the Data on pData* to the value of the hexdigit ('a' => 0xa)*/
u8 Byte4ToHex 	( u8  bData ); /*return hex digit matching the lower 4 bit of bData*/

#define sendOnOff(x) do{if(x)CommSendStrC13(sON,2);else CommSendStrC13(sOFF,3);}while(0);

#define sendArray13(x) do{CommSendStrC((const u8* const)(x),strlen(x));SendNewline();}while(0)
#define sendArray(x)   do{CommSendStrC((const u8* const)(x),strlen(x));}while(0)

#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
