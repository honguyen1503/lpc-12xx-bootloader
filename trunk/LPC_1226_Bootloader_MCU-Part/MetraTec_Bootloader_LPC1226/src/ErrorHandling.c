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


/*this sends all types of answer, builds the frame for this*/
void sendAnswer(u8 bCommand, const tErrorCode eError, const u8* const pAnswerParameter, const u32 dwParameterLength)
{
	u16 wCrc=0xFFFF;												/*start crc value*/

	u8 mLength[2]={(dwParameterLength+2)>>8,(dwParameterLength+2)};
	UartSendBuffer(mLength,2);
	GetCRC_CCITT(CRCMODE_UARTCOMM_EXCL_END,mLength,2,&wCrc);

	bCommand|=0x80;													/*take the command and add the bit to show the command the answer belongs to*/
	GetCRC_CCITT(CRCMODE_UARTCOMM_EXCL_END,&bCommand,1,&wCrc);		/*add command to crc*/
	UartSendByte(bCommand);											/*send byte 1*/
	if (dwParameterLength==0||pAnswerParameter==NULL)				/*if there is nothing more to send than the error code*/
	{
		GetCRC_CCITT(CRCMODE_UARTCOMM_INCL_END,&eError,1,&wCrc);	/*add the error code to crc*/
		UartSendByte(eError);										/*and send the error code*/
	}
	else
	{
		GetCRC_CCITT(CRCMODE_UARTCOMM_EXCL_END,&eError,1,&wCrc);	/*add the error code to crc*/
		UartSendByte(eError);										/*and send the error code*/
		GetCRC_CCITT(CRCMODE_UARTCOMM_INCL_END,pAnswerParameter,dwParameterLength,&wCrc);	/*also add the data to send (for read commands)*/
		UartSendBuffer(pAnswerParameter,dwParameterLength);			/*and send the data*/
	}
	UartSendByte(wCrc>>8);												/*send the crc*/
	UartSendByte(wCrc);
}
