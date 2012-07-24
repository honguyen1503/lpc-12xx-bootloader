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

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

/*shortened variable type names to write faster*/
typedef uint8_t 	u8;
typedef int8_t 		s8;
typedef uint16_t 	u16;
typedef int16_t 	s16;
typedef uint32_t 	u32;
typedef int32_t 	s32;
typedef uint64_t 	u64;
typedef int64_t 	s64;

/*defining Boolean type. In case of problems use typedef u8 Bool; #define FALSE 0 #define TRUE 1*/
typedef enum { FALSE = 0, TRUE  = 1} Bool;

/* defines a structure type containing the Bootloader and Hardware paramter. Obviously they should not be changed more than once and only
 * if the are set to 0xFFs. It is possible to make a application using IAP to update the bootloader but this is not recommended!*/
typedef	struct
{
	const const u8 mBootloaderName[16];
	const const u8 mBootloaderRevision[4];
	const const u8 mHardwareName[16];
	const const u8 mHardwareRevision[4];
	const const u8 mSerialNumber[16];
}tBootloaderParamFlash;

typedef struct
{
	const const u8 mFW_Name[16];
	const const u8 mFW_Rev[4];
	const const u8 mHW_Req_Name[16];
	const const u8 mHW_Req_Rev[4];
	const const u32 dwFirmwareSizeBytes;
	const const u32 dwFirmwareCrcInMet;
	const const u32 dwFirmwareCrcOnMcu;
}tFirmwareParamFlash;

/*
 * UartSendBuffer
 * Is used to buffer the outgoing data (via Uart).
 * Because there might be some parameters adding additional information to any answer (UHF: SET EPC, SET TRS...)
 * this size (512Bytes) might be needed. The buffer is implemented as line buffer. May become empty after every round
 */

typedef struct
{
	u8 mBuffer[512];		/*The buffer for all chars*/
	volatile u32 dwLength; 	/*position to write, use as bool to check if data available*/
}tUartSendBuffer;

/*
 * UartReceiveBuffer
 * Is used to buffer incoming Uart data
 * The size is not needed, especially on smaller chips 128 Bytes fit more
 * The buffer is implemented as a ringbuffer. 256bytes size makes overflow easy.
 */
#define RECEIVEBUFFER_RINGSIZE 256
typedef struct
{
	u8 mBuffer [RECEIVEBUFFER_RINGSIZE];/*Uart ring buffer*/
	u8 bWrite;							/*write position (when new char is received via Uart)*/
	volatile Bool xPacketAvailable;		/*says if a complete packet if available (important if an CRC error occured and for the main.c)*/
	u8 bActualPacketStart;				/*The pointer to the start of the first complete packet. If no packet started it's identical to write, if no complete packet it's identical to LastPacketStart*/
	u8 bLastPacketStart;				/*The pointer to the start of the last (incomplete) packet. If no packet started it's identical to write*/
	Bool xStopReceivingUntilCR;			/*the bool is set after a receive error occured to make sure the packets rest will not be accepted (so no second error occurs)*/
	u8 bCommCmpStart;					/*used in UartFunctions.c functions to parse*/
	Bool xFieldPowerOn;
}tUartReceiveBuffer;

typedef enum
{
	/*UHF (and partly Iso15)general errors*/
	ecNoError=0x00,
	ecAcknowledge=0x01,
	ecBrownOutDetected=0x02,
	ecBufferOverflow=0x03,
	ecCommunicationCRCError=0x04,
	ecCommandReceiveTimeout=0x05,
	ecDidNotSleep=0x06,
	ecDecimalDecodeError=0x07,
	ecHardwareFailure=0x08,
	ecHexDecodeError=0x09,
	ecNotInCNRMode=0x0A,
	ecNumberOutOfRage=0x0B,
	ecNotSupported=0x0C,
	ecNoRfFieldActive=0x0D,
	ecNoStandardSelected=0x0E,
	ecPhaseLockError=0x0F,
	ecWatchdogOccured=0x10,
	ecUnknownCommand=0x11,
	ecUnknownParameter=0x12,
	ecUartReceiveError=0x13,
	ecWrongDataLength=0x14,
	ecAntennaReflectivityToHigh=0x15,

	/*UHF and partly Iso15 Tag errors*/
	ecRFIDCRCError=0x20,
	ecFifoLengthError=0x21,
	ecHeaderbitError=0x22,
	ecErrorGettingHeaderbitErrorCode=0x23,
	ecReadDataToLong=0x24,
	ecResponseLengthNotAsExpedted=0x25,
	ecTagCommunicationError=0x26,
	ecToManyTags=0x27,
	ecTimeOutError=0x28,
	ecTagOutOfRage=0x29,
	ecTagNotResonding=0x3A,
	ecAccessError=0x3B,
	ecPreambleDetectError=0x3C,
	/*Iso15 errors*/
	ecParameterExpected=0x40, /*is in Unknown parameter in UHF*/
	ecRegisterNoWritten=0x41,
	ecDsynError=0x42,
	ecCollisionDetected=0x43,

	/*SPI internal errors*/
	ecSpi_WrongReceiveCounter=0x50,
	ecSpi_NoNewData=0x51,
	ecSpi_CommunicationError=0x52,

	ecPinHigh=0x60,
	ecPinLow=0x61,

	ecNoTagSelected=0x70,
	ecWrongTagType=0x71,
	ecBlockNotAuthenticated=0x72,
	ecAuthenticationError=0x73,

	/*Iso 15 internal error*/
	ecIso15_AnswerToShortError=0x80,
	ecIso15_DoubleUidError=0x81,
	ecIso15_NotStarted=0x82,

	/*PN532 error*/
	ecPN532_SyntaxError=0x90,
	ecMifareFramingError=0x91,
	ecReceivedUnknownCommand=0x92,
	ecCommandNotUsableInThisState=0x93,

	ecUnknownError=0xFF
}tErrorCode;


typedef void((*voidfunction)(void));	/*functionpointer to function without paramter or return value*/

typedef struct
{
	volatile u32 clock; 											/*Zero is no valid value so zero can be used as mark*/
	voidfunction functionArray[TIMERNUMBEROFFUNCTIONS];	/*an array of functions. if functionClockValue[i]==clock, functionArray[i] is called*/
	u32 functionClockValue[TIMERNUMBEROFFUNCTIONS];		/*Clock Value 0 zero (0) means function not active*/
} tTimerValues;



#endif /* TYPEDEFS_H_ */
