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

#include "../Defines/GlobalIncludes.h"
volatile u8 bMaxFifoLevel=0;
tUartSendBuffer udtUartSendBuffer;
tUartReceiveBuffer udtUartReceiveBuffer;
/*
 * moves the pointer, regarding the overflow
 * pVariable is a pointer to an array element value. array[*pVariable] is the form of usage
 */
inline void movePositionForwardInRecvRing(u8* pVariable, u8 bValue)
{
#if RECEIVEBUFFER_RINGSIZE==256								/*if the Ringsize is 256 the overflow is no problem*/
*pVariable+=bValue;											/*no problem, just add it*/
#else
if (bValue>RECEIVEBUFFER_RINGSIZE)						/*more then the buffer size*/
{
	SendUnknownError();										/*no specific error code*/
}
else if ((u32)(*pVariable)+bValue>=(u32)RECEIVEBUFFER_RINGSIZE)	/*if there is an overflow*/
{
	*pVariable-=RECEIVEBUFFER_RINGSIZE-bValue;				/*var=var+value-Size*/
}
else														/*else*/
{
	*pVariable+=bValue;										/*no problem, just add it*/
}
#endif
}

/*
 * Send zero terminated string
 */
void CommSendStrC ( const u8* const  mString, u8 bSize )
{
	for (u8 i=0;i<bSize;i++)
	{
		UartSendByte(mString[i],TRUE);
	}
}

/*
 * Send Byte in two byte ASCII-Byte Hex-Coded via Uart
 */
void CommSendHex ( u8 bData )
{
	UartSendByte(Byte4ToHex(bData>>4),TRUE);		/*send higher nibble*/
	UartSendByte(Byte4ToHex(bData&0x0F),TRUE);		/*send lower nibble*/
}


/*
 * Send array of bytes ASCII-Byte Hex-Coded (2 chars per byte) via Uart
 */
void CommSendBufHex ( const u8* const mBuf, const u8 bLen)
{
	u8 bBufferHexSize=bLen<<1;
	u8 mBuffer[bBufferHexSize];
	 for (u8 bIndex=0; bIndex<bLen; bIndex++)
	 {
		mBuffer[bIndex<<1]=Byte4ToHex(mBuf[bIndex]>>4);
		mBuffer[(bIndex<<1)+1]=Byte4ToHex(mBuf[bIndex]&0x0F);
	 }
	 /*if (xAdd13)
	 {
		 mBuffer[bBufferHexSize]=13;
		 UartSendBuffer(mBuffer,bBufferHexSize+1,TRUE);
	 }
	 else*/
	 UartSendBuffer(mBuffer,bBufferHexSize,TRUE);
}


/*
 * sends a decimal number via Uart.
 * The number of bytes is minimized (0-9 1 byte, 10-99 2 bytes and so on)
 *  but can be set to a minimum (minLen=3,dwData=1  =>  send 001)
 */
void CommSendDec ( u32 dwData, u8 bMinLength )
{
	u8 String[10]={0};	/*local string*/
	u8 i=0;				/*digit counter*/
	u8 bLastDigit=0; 	/*help variable in computing*/

	while (i<10) 		/*the lowest digit comes first but will be stored last*/
	{
		i++;
		bLastDigit=(dwData%10);				/*compute last digit*/
		String[10-i]=bLastDigit+'0'; 		/*print it as hex to string*/
		dwData=(dwData-bLastDigit)/10;		/*remove last digit from number*/

		if (i>=bMinLength && dwData==0)		/*have the minimal length and no remaining value to work*/
			break;							/*go to print operation*/
	}
	CommSendBuf(String+10-i,i);				/*send the string beginning at the highest digit set*/
}



/*
 * Compares the String udtString with the UartReceiveBuffer beginning from CommCmpStart.
 * If the string matches it sets the comparison pointer (CommCmpStart) to the end of the tested string.
 * The string matches if all signs are equal including the case both are empty (\0 for input, <CR> for UartReceiveBufffer at CommCmpStart.
 * space chars at start of RecvBuffer will be passed and not are not subject of comparison. They will be deleted in the next comparison!
 * Case is not subject of comparison
 */
Bool CommCmpStrC ( const u8* const  mString )
{
	u8 bCounter=0;
	u8 bLocStr=LowerCase(mString[0]);
	u8 bIndex=0;

	while (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]==' ') 	/*move Start to the first byte not being a space*/
	{
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);
	}
	bIndex=udtUartReceiveBuffer.bCommCmpStart;										/*make a copy of the ComparePointer*/

	while (bLocStr!=0 &&
		   udtUartReceiveBuffer.mBuffer[bIndex]!=13 &&
		   udtUartReceiveBuffer.mBuffer[bIndex]!=' ')								/*While there is no end in RecvBuffer or udtString. udtString is zero terminated. RecvBuffer by Line feed or space*/
	{
		if (bLocStr==LowerCase(udtUartReceiveBuffer.mBuffer[bIndex]))				/*if chars are identical*/
		{
			movePositionForwardInRecvRing(&bIndex,1);								/*go on*/
			bCounter++;																/*go on*/
		}
		else
		{
			return FALSE; 															/*return "not identical"*/
		}
		bLocStr=LowerCase(mString[bCounter]); 						/*get new char*/
	}
	if (bLocStr==0 && (
		udtUartReceiveBuffer.mBuffer[bIndex]==13 ||
		udtUartReceiveBuffer.mBuffer[bIndex]==' ')) 								/*If udtString is done AND RecvBuffer done (space or lineFeed) and obviously no difference occurred before*/
		{
			udtUartReceiveBuffer.bCommCmpStart=bIndex;								/*move the comparison pointer to the end of the word/param/whatever*/
			return TRUE; 															/*return "identical"*/
		}
	else 																			/*different length, so not identical*/
	{
		return FALSE;																/*dont change the comparison poiter and return*/
	}
}

/*
 * Decodes the Hexcodes in receive buffer starting at comparison pointer
 * returns TRUE if the data is hex and not to long (max. 255 nibbles!) or to short (no sign at all, 0 nibbles)
 * the data to use replaces the original data
 * (first byte, the length in NIBBLES! will be set on udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bActualPacketStart],
 * the (formerly) hex data follows (first byte: udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bActualPacketStart+1]...))
 * This function might cause an error if the hexstring starts right at the start of the buffer (but normally there are command, other params etc.)
 */
Bool CommDecodeHex ( void )
{
	while (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]==' ') 	/*pass space characters*/
	{
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);
	}
	if (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]==13)		/*if no hex char the function is not successful*/
	{
		return FALSE;
	}

	u8 RdIndex=udtUartReceiveBuffer.bCommCmpStart;									/*Index for reading Hex*/
	u8 WrIndex=udtUartReceiveBuffer.bActualPacketStart;								/*Index for writing decoded data*/
	u8 NibbleCounter=0; 															/* counter for the number of nibbles*/

	while (udtUartReceiveBuffer.mBuffer[RdIndex]!=13 &&								/*hex is terminated by space (more parameters) or 13 (line feed)*/
		   udtUartReceiveBuffer.mBuffer[RdIndex]!=' ')
	{
		if (!HexToByte4(udtUartReceiveBuffer.mBuffer+RdIndex))						/*if no hex char, the function will decode the hex signs*/
		{
			udtUartReceiveBuffer.bCommCmpStart=RdIndex;								/*move it to the wrong sign to be sure no sideeffects appear (like decoding a 0x44 ('D') to 0x0D causing errors on packet delete function*/
			return FALSE;
		}
		movePositionForwardInRecvRing(&RdIndex,1);									/*next byte*/
	}

	movePositionForwardInRecvRing(&WrIndex,1);										/*first byte is for counter*/
	while (RdIndex!=udtUartReceiveBuffer.bCommCmpStart)								/*go on until all decoded hex bytes are copied to the buffer*/
	{
		if (NibbleCounter==255)														/*to long*/
		{
			udtUartReceiveBuffer.bCommCmpStart=RdIndex;								/*move it to the wrong sign to be sure no sideeffects appear (like decoding a 0x44 ('D') to 0x0D causing errors on packet delete function*/
			return FALSE;
		}
		if (BIT0&NibbleCounter)														/*nibblecounter is odd*/
		{
			udtUartReceiveBuffer.mBuffer[WrIndex]|=udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart];
																					/*add the nibble to existing byte*/
			movePositionForwardInRecvRing(&WrIndex,1);								/*go to next byte*/
		}
		else
		{
			udtUartReceiveBuffer.mBuffer[WrIndex]=udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]<<4;
																					/*start a new byte*/
		}
		NibbleCounter++;															/*count nibble up*/
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);		/*move compare pointer (until it reaches the end of the hex string marked by Rdindex now)*/
	}
	udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bActualPacketStart]=NibbleCounter;
	return TRUE;
}

/*
 * Get a number from Uart receive buffer
 * if the buffer contains chars of false base it returns FALSE
 * The read out value will be stored in pData (call by Reference)
 * bMaxIndex gives the maximal number of digits. For hex and digital this fits the u32, for octal and decimal there might occur errors if the values exceed the u32 value
 * The base must be min 2 and max 16
 */

Bool CommGetDataBaseFunction ( u32* pData, u8 bBase, u8 bMaxIndex)
{
	if (bBase>16 || bBase<2)		/*base wrong value*/
	{
		return FALSE;
	}

	/*BLOCK 1 Set CommCmpStart on first char except space and block if end of packet is already reached*/
	while (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]==' ')	/*spaces to pass*/
	{
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);
	}
	if (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]==13)		/*no data*/
	{
		return FALSE;
	}
	/*END BLOCK 1*/

	*pData=0;
	u8 bCharNumber=0;
	u8 bIndex=udtUartReceiveBuffer.bCommCmpStart;
	bMaxIndex++;									/*Offset. The end sign will not be detected otherwise (' ' or \13)*/
	while (bMaxIndex) 								/*max index shows number of max digits to do*/
	{
		bCharNumber=udtUartReceiveBuffer.mBuffer[bIndex];			/*get the first digit*/
		if ((bCharNumber==' ')||(bCharNumber==13)) 					/*end of number*/
		{
			break;													/*then stop*/
		}
		if (!HexToByte4(&bCharNumber)) 								/*get digit to a max of base=16 (Hex-Code)*/
		{
			return FALSE; 											/*other char than hex value (or space, but this may never occur. see lines before)*/
		}
		if (bCharNumber<bBase)										/*char is part of base*/
		{
			*pData=(*pData * bBase) + bCharNumber; 					/*Formula to add a digit at the end of a number*/
		}
		else														/*hex value to high (e.g. E(14) in decimal base)*/
		{
			return FALSE;
		}
		bMaxIndex--;												/*one digit done*/
		movePositionForwardInRecvRing(&bIndex,1);					/*move source byte on*/
	}
	if (bMaxIndex==0)												/*data is to long*/
	{
		return FALSE;
	}
	else															/*all ok*/
	{
		udtUartReceiveBuffer.bCommCmpStart=bIndex;					/*set comparison pointer to the end of the number*/
		return TRUE;
	}
}

/*
 * returns the character in lower case
 */
u8 LowerCase ( u8 bData )
{
 if ((bData>='A')&&(bData<='Z'))		/*if higher case*/
	 bData+='a'-'A';					/*make it a lower case*/
 /*else everything stays the same*/
 return(bData);
}

/*
 * Ascii Byte with Hex Meaning ('0' to 'F') to unsigned 4 Bit (0x00 to 0x0F)
 */
Bool HexToByte4 ( u8* pData )
{
	if ((*pData>='0')&&(*pData<='9'))
	{
		*pData-='0';
	}
	else if ((*pData>='A')&&(*pData<='F'))
	{
		*pData=*pData-'A'+10;
	}
	else if ((*pData>='a')&&(*pData<='f'))
	{
		*pData=*pData-'a'+10;
	}
	else
	{
		return FALSE; 				/*no character that can be interpreted as Hex*/
	}
	return (TRUE);					/*Done ok*/
}

/*
 *  return the lower 4 byte of u8 as a character with hex meaning ('0' to 'F')
 */
u8 Byte4ToHex ( u8 bData )
{
	bData&=0x0F;				/* cut of higher nibble*/
	if (bData<10)				/*decimal digit*/
	{
		return bData+'0';
	}
	else						/*hex digit*/
	{
		return bData-10+'A';
	}
}

/*
 * Ascii Byte with decimal meaning ('0' to '9') to unsigned 4 Bit (0x00 to 0x09)
 * returns true if value is 0 to nine, else false
 */
Bool DecToByte ( u8* pData )
{
	if ((*pData>='0')&&(*pData<='9'))	/*check boundaries*/
	{
		*pData-='0';
		return TRUE;				/*Success*/
	}
	else
	{
		return FALSE;				/*error*/
	}
}
/*
 * Give the length from bStart to bEnd
 */
u8 RingDiff(u8 bStart, u8 bEnd)
{
#if RECEIVEBUFFER_RINGSIZE==256				/*if the Ringsize is 256 the overflow is no problem*/
	return bEnd-bStart;						/*no problem, just add it*/
#else
	s16 iDiff=bEnd-bStart;					/*difference*/
	if (iDiff<0)							/*start is higher then end, so there will be an overflow*/
	{
		iDiff+=RECEIVEBUFFER_RINGSIZE;		/*add the ringsize, so the differece will match*/
	}
	return iDiff;							/*return it*/
#endif
}

/*
 * Copy bLength bytes from udtUartReceiveBuffer.mBuffer to the memory buffer, starting at (bStart+bOffSet) regarding a overflow.
 */
void copyDataFromReceiveRingBufferToBuffer(u8 bStart, u8 bLength, u8* mTargetBuffer,u8 bOffSet)
{
	movePositionForwardInRecvRing(&bStart,bOffSet);					/*add the offset regarding the overflow*/
	for (u8 i=0;i<bLength;i++)										/*for the length of bytes*/
	{
		mTargetBuffer[i]=udtUartReceiveBuffer.mBuffer[bStart];		/*copy the byte*/
		movePositionForwardInRecvRing(&bStart,1);					/*move to next byte*/
	}
}

/*
 * Read all data from UART Receiver. Data will not be used
 */
void USART_Flush( void )
{
	u8 bDummy;
	while ( LPC_UART0->LSR & LSR_RDR )
	{
		bDummy = LPC_UART0->RBR;	/* Dump data from RX FIFO */
	}
}

#define UartCrcAdding_NONEND(bData) do{LPC_CRC->MODE=CRCMODE_UARTCOMM_EXCL_END<<2;LPC_CRC->SEED=CRC_Check;LPC_CRC->WR_DATA_8=bData;CRC_Check=LPC_CRC->SUM;}while(0)
#define UartCrcAdding_END(bData) do{LPC_CRC->MODE=CRCMODE_UARTCOMM_INCL_END<<2;LPC_CRC->SEED=CRC_Check;LPC_CRC->WR_DATA_8=bData;CRC_Check=LPC_CRC->SUM;}while(0)

/*
 * UartSendByte writes a byte bData to the send buffer
 * if xUseForCRC==TRUE and the global parameter xCRC_On==TRUE, the character will be used for Uart CRC
 * returns TRUE as "done", FALSE on send buffer overflow
 */
void UartSendByte ( u8 bData, Bool xUseForCRC )
{
	DISABLE_GLOBAL_IRQ();

	if (udtUartSendBuffer.dwLength==NumberOfElementsInArray(udtUartSendBuffer.mBuffer) -1) /*protection agains SendBufferOverflow*/
	{
		DeleteUartSendBuffer();
		SendNewline();
		SendErrorByCode(ecBufferOverflow);
	}
	else
	{
		if (udtUartSendBuffer.dwLength==0 && (LPC_UART0->LSR)&BIT5)
			LPC_UART0->THR=bData;
		else
		{
			udtUartSendBuffer.mBuffer[udtUartSendBuffer.dwLength]=bData;	/*copy the byte*/
			udtUartSendBuffer.dwLength++;
			LPC_UART0->IER|=IER_THRE;
		}
	}
 	ENABLE_GLOBAL_IRQ();
}
/*
 * UartSendByte writes a byte bData to the send buffer
 * if xUseForCRC==TRUE and the global parameter xCRC_On==TRUE, the character will be used for Uart CRC
 * returns TRUE as "done", FALSE on send buffer overflow
 */
void UartSendBuffer ( const const u8* mData, const u8 bLength, const Bool xUseForCRC )
{
	for (u8 i=0;i<bLength;i++)
		UartSendByte(mData[i],xUseForCRC);
}
/*
 * Delete the whole receive buffer (e.g. after an error occurred)
 */
void DeleteUartReceiveBuffer (void)
{
	udtUartReceiveBuffer.xPacketAvailable = FALSE;
	udtUartReceiveBuffer.bWrite = 0;
	udtUartReceiveBuffer.bCommCmpStart = 0;
	udtUartReceiveBuffer.bLastPacketStart=0;
	udtUartReceiveBuffer.xStopReceivingUntilCR=FALSE;
	udtUartReceiveBuffer.bActualPacketStart=0;
}

/*
 * Delete the whole send buffer (e.g. after an error occurred)
 */
void DeleteUartSendBuffer (void)
{
	udtUartSendBuffer.dwLength = 0;
}

/*
 * Delete one packet, normally after parsing
 */
void DeleteUartPacket ( void )
{
	if (!udtUartReceiveBuffer.xPacketAvailable)										/*there is no packet*/
		return;

	while (udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bCommCmpStart]!=13) 	/*as long as the ComparePointer does not point to the end of a packet*/
	{
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);
	}
	movePositionForwardInRecvRing(&udtUartReceiveBuffer.bCommCmpStart,1);			/*go to the first sign of the next packet*/
	if (udtUartReceiveBuffer.bCommCmpStart==udtUartReceiveBuffer.bLastPacketStart)	/*if the next packet is not done then the lastPacketStart is after the actual full packet*/
	{
		udtUartReceiveBuffer.xPacketAvailable=FALSE;
	}
	else
	{
		/*else case is not needed, because packet available=TRUE is already secured at the start*/
	}
	udtUartReceiveBuffer.bActualPacketStart=udtUartReceiveBuffer.bCommCmpStart;
}


/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART0_IRQHandler(void)
{
	DISABLE_GLOBAL_IRQ();
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy;

	IIRValue = LPC_UART0->IIR;

	IIRValue >>= 1;			/* skip pending bit in IIR */
	IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	if (IIRValue == IIR_RLS)		/* Receive Line Status */
	{
		LSRValue = LPC_UART0->LSR;
		/* Receive Line Status */
		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			//UARTStatus1 = LSRValue;
			Dummy = LPC_UART0->RBR;	/* Dummy read on RX to clear interrupt, then bail out */
		}
		else if (LSRValue & LSR_RDR)	/* Receive Data Ready */
			ISR_UartRX();
	}
	else if (IIRValue == IIR_RDA)	/* Receive Data Available */
	{
		/* Receive Data Available */
		ISR_UartRX();
	}
	else if (IIRValue == IIR_CTI)	/* Character timeout indicator */
	{
		/* Character Time-out indicator */
		SendErrorByCode(ecUartReceiveError);
	}
	else if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
	{
		if (udtUartSendBuffer.dwLength)
		{
			u8 bSendLength=16;
			if (udtUartSendBuffer.dwLength<bSendLength)
				bSendLength=udtUartSendBuffer.dwLength;
			udtUartSendBuffer.dwLength-=bSendLength;
			for (u8 i=0;i<bSendLength;i++)
			{
				LPC_UART0->THR=udtUartSendBuffer.mBuffer[i];
			}
			if (udtUartSendBuffer.dwLength)
			{
				u32 dwLocRemainingLength=udtUartSendBuffer.dwLength;
				u8* pStartPosition=udtUartSendBuffer.mBuffer;
				while (1)
				{
					if (dwLocRemainingLength>16)
					{
						memcpy(pStartPosition,pStartPosition+16,16);
						dwLocRemainingLength-=16;
						pStartPosition+=16;
					}
					else
					{
						memcpy(pStartPosition,pStartPosition+16,dwLocRemainingLength);
						break;
					}
				}
			}
			else
				LPC_UART0->IER&=~(1<<1);
		}
		else
		{
			LPC_UART0->IER&=~(1<<1);
			SendErrorByCode(ecUnknownError);
		}
	}
	ENABLE_GLOBAL_IRQ();
	return;
}


/*
 * UartDataReceived Interrupt
 * CRC check of received bytes is done here also
 */
void ISR_UartRX (void)
{
	u8 bData;
	CallTimedFunction(TimerUartReceive,5);

	bData = LPC_UART0->RBR;	/*get the byte */

	if (udtUartReceiveBuffer.xStopReceivingUntilCR)/* no error but an error occurred earlier so it waits for the packet end */
	{
		if (bData==13)	/*<CR>*/
		{
			CallTimedFunction(TimerUartReceive,0);				/*stop timer */
			udtUartReceiveBuffer.xStopReceivingUntilCR=FALSE;	/*stop receiving into void */
		}
		return;
	}/*end error afterwork*/

	if (RingDiff(udtUartReceiveBuffer.bWrite,udtUartReceiveBuffer.bActualPacketStart)==1)/*buffer overflow (write position is on compare position) */
	{
		SendErrorByCode(ecBufferOverflow);
		if (bData!=13)											/*if not end of the packet */
			udtUartReceiveBuffer.xStopReceivingUntilCR=TRUE;	/*wait for the end of packet */
		udtUartReceiveBuffer.bWrite=udtUartReceiveBuffer.bLastPacketStart;	/*go back to last packet */

	}
	else /*size is ok */
	{
		udtUartReceiveBuffer.mBuffer[udtUartReceiveBuffer.bWrite] = bData;/*get Byte to buffer */
		movePositionForwardInRecvRing(&udtUartReceiveBuffer.bWrite,1);
		if (bData == 13) /*if end of packet */
		{
			CallTimedFunction(TimerUartReceive,0);
			udtUartReceiveBuffer.bLastPacketStart=udtUartReceiveBuffer.bWrite;
			udtUartReceiveBuffer.xPacketAvailable=TRUE;
		}
	}
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UARTx port, setup pin select,
**						clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART port number, baudrate
** Returned value:		None
**
*****************************************************************************/
void UARTInit(uint32_t baudrate)
{
	u32 Fdiv;
	u32 regVal;

	NVIC_DisableIRQ(UART0_IRQn);

	LPC_IOCON->PIO0_1 &= ~0x07;    /* UART0 I/O config */
	LPC_IOCON->PIO0_1 |= 0x02;     /* UART0 RXD LOC0 */
	LPC_IOCON->PIO0_2 &= ~0x07;
	LPC_IOCON->PIO0_2 |= 0x02;     /* UART0 TXD LOC0 */

	/* Enable UART 0 clock */
	LPC_SYSCON->PRESETCTRL |= (0x1<<2);
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<12);
	LPC_SYSCON->UART0CLKDIV = 0x1;     /* divided by 1 */

	LPC_UART0->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
	regVal = LPC_SYSCON->UART0CLKDIV;
	Fdiv = (((SystemCoreClock/regVal)/16)+(baudrate>>1))/baudrate ;	/*baud rate */

	LPC_UART0->DLM = Fdiv / 256;
	LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
	LPC_UART0->FDR = 0x10;		/* set to default value: 0x10 */
	LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	/* Read to clear the line status. */
	regVal = LPC_UART0->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while ( (LPC_UART0->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART0->LSR & LSR_RDR )
	{
	  regVal = LPC_UART0->RBR;	/* Dump data from RX FIFO */
	}

	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART0_IRQn);

	LPC_UART0->IER = IER_RBR | IER_RLS;	/* Enable UART interrupt */

	return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
