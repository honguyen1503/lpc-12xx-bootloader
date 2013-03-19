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

volatile u32 dwUartRxTimeoutClock=0;
volatile u32 dwNumberOfInputByte=0;
volatile u8 mInputBytes[300]={0};
u32 dwCommandReceiveTimeout=5;

#if (USED_UART!=1)
volatile u8 bMaxFifoLevel=0;
#endif


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

void sendU32Hex(u32 sendByte)
{
	UartSendByte(Byte4ToHex(sendByte>>4));
	UartSendByte(Byte4ToHex(sendByte>>0));

	UartSendByte(Byte4ToHex(sendByte>>12));
	UartSendByte(Byte4ToHex(sendByte>>8));

	UartSendByte(Byte4ToHex(sendByte>>20));
	UartSendByte(Byte4ToHex(sendByte>>16));

	UartSendByte(Byte4ToHex(sendByte>>28));
	UartSendByte(Byte4ToHex(sendByte>>24));
	UartSendByte('|');

	UartSendByte(Byte4ToHex(sendByte>>28));
	UartSendByte(Byte4ToHex(sendByte>>24));
	UartSendByte(Byte4ToHex(sendByte>>20));
	UartSendByte(Byte4ToHex(sendByte>>16));
	UartSendByte(Byte4ToHex(sendByte>>12));
	UartSendByte(Byte4ToHex(sendByte>>8));
	UartSendByte(Byte4ToHex(sendByte>>4));
	UartSendByte(Byte4ToHex(sendByte>>0));
	SendNewline();
}
#if (USED_UART==1)
void UART1_IRQHandler(void)
{
	DISABLE_GLOBAL_IRQ();
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy;

	IIRValue = LPC_UART1->IIR;

	IIRValue >>= 1;			/* skip pending bit in IIR */
	IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	if (IIRValue == IIR_RLS)		/* Receive Line Status */
	{
		LSRValue = LPC_UART1->LSR;
		/* Receive Line Status */
		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			//UARTStatus1 = LSRValue;
			Dummy = LPC_UART1->RBR;	/* Dummy read on RX to clear interrupt, then bail out */
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
		sendAnswer(0,ecUartTimeout,NULL,0);
	}
	ENABLE_GLOBAL_IRQ();
	return;
}
#else
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
		sendAnswer(0,ecUartTimeout,NULL,0);
	}
	else if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
	{
		LPC_UART0->IER&=~(1<<1);
		bMaxFifoLevel=0;
	}
	ENABLE_GLOBAL_IRQ();
	return;
}
#endif

#if (USED_UART==1)
/*
 * UartDataReceived Interrupt
 * CRC check of received bytes is done here also
 */
void ISR_UartRX (void)
{
	while ((0x0F&LPC_UART1->FIFOLVL)!=0x00)
	{
		mInputBytes[dwNumberOfInputByte] = LPC_UART1->RBR;	/*get the byte */
		dwNumberOfInputByte++;
	}
	dwUartRxTimeoutClock=dwClockValue+5;
	if (dwUartRxTimeoutClock<5)
		dwUartRxTimeoutClock++;
}
#else
/*
 * UartDataReceived Interrupt
 * CRC check of received bytes is done here also
 */
void ISR_UartRX (void)
{
	mInputBytes[dwNumberOfInputByte] = LPC_UART0->RBR;	/*get the byte */
	dwNumberOfInputByte++;
	dwUartRxTimeoutClock=dwClockValue+dwCommandReceiveTimeout;
	if (dwUartRxTimeoutClock<dwCommandReceiveTimeout)
		dwUartRxTimeoutClock++;
}
#endif

void UartSendByte ( u8 bData)
{
#if (USED_UART==1)
	while (((0x0F)&(LPC_UART1->FIFOLVL>>8))==0x0F); /*protection agains SendBufferOverflow*/
	LPC_UART1->THR=bData;
#else
	while (bMaxFifoLevel>=16);/*protection agains SendBufferOverflow*/
	LPC_UART0->THR=bData;
	bMaxFifoLevel++;
	LPC_UART0->IER|=(1<<1);
#endif
}
#if (USED_UART==1)

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

	NVIC_DisableIRQ(UART1_IRQn);

	LPC_IOCON->PIO0_8 &= ~0x07;    /* UART1 I/O config */
	LPC_IOCON->PIO0_8 |= 0x02;     /* UART1 RXD LOC0 */
	LPC_IOCON->PIO0_9 &= ~0x07;
	LPC_IOCON->PIO0_9 |= 0x02;     /* UART1 TXD LOC0 */

	/* Enable UART 1 clock */
	LPC_SYSCON->PRESETCTRL |= (0x1<<3);
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<13);
	LPC_SYSCON->UART1CLKDIV = 0x1;     /* divided by 1 */

	LPC_UART1->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
	regVal = LPC_SYSCON->UART1CLKDIV;
	Fdiv = (((SystemCoreClock/regVal)/16)+(baudrate/2))/baudrate ;	/*baud rate */

	LPC_UART1->DLM = Fdiv / 256;
	LPC_UART1->DLL = Fdiv % 256;
	LPC_UART1->LCR = 0x03;		/* DLAB = 0 */
	LPC_UART1->FDR = 0x10;		/* set to default value: 0x10 */
	LPC_UART1->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	/* Read to clear the line status. */
	regVal = LPC_UART1->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while ( (LPC_UART1->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART1->LSR & LSR_RDR )
	{
	  regVal = LPC_UART1->RBR;	/* Dump data from RX FIFO */
	}


	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART1_IRQn);

	LPC_UART1->IER = IER_RBR | IER_RLS;	/* Enable UART interrupt */
	return;
}
#else
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
#endif

void UartSendBuffer ( const u8* const mData, const u8 bLength )
{
	for (u8 i=0;i<bLength;i++)
		UartSendByte(mData[i]);
}
