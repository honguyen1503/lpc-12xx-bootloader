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
/*defining the values of the used commands*/
#define Command_Handshake 		0x01
#define Command_WriteCode 		0x02
#define Command_Read_SN 		0x03
#define Command_Read_HW_Name 	0x04
#define Command_Read_HW_Rev 	0x05
#define Command_Read_FW_Name 	0x06
#define Command_Read_FW_Rev 	0x07
#define Command_Read_Flash_Size 0x08
#define Command_Erase_Flash 	0x0A
#define Command_Read_BTL_Rev 	0x0C
#define Command_Read_BTL_Name	0x0D
#define Command_SetCommandReceiveTimeout 0x10
#define Command_Reset 			0x41
#define Command_Write_SN 		0x43
#define Command_Write_HW_Name 	0x44
#define Command_Write_HW_Rev 	0x45

static void decodeApplicationCode(void)
{
	u16 i = 0;
	static unsigned char p = 0;		/*counter*/
	const u8 Key[18] = KEY_XOR;		/*external key is loaded here from passwords.h*/

	//XOR
	for(i = 0; i < 256; i++)			/*for a data block (256 byte)*/
	{
		*(mProgramBuffer+i) ^= Key[p];	/*XOR with the XOR-Key*/
		p++;							/*add to pointer*/
		if(p >= 18)						/*close the ring*/
		  p = 0;
	}

	/*Aes*/
	for (u16 i=0;i<256;i+=16)			/*  16*16 = 256 Bytes 16 Bytes =128 bit (for AES 128) */
		AesDecrypt (mProgramBuffer+i, mExpandedKey, mProgramBuffer+i);
}

void Parser(void)
{
	static u32 dwBlockCounter_Parser=0;	/*counts how many blocks are already written so no wrong block will be loaded*/
	static u32 dwDataLength=0;
	static const u8 const mBlankBlock[256]={	/*just a blank block to blank check the data area*/
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	};

	if (dwNumberOfInputByte>=4+dwDataLength || (dwUartRxTimeoutClock==0 && dwNumberOfInputByte))		/*if there is some data or the clock is up (timout) */
	{
		dwDataLength=(mInputBytes[0]<<8)+mInputBytes[1];
		if (dwDataLength>296 || dwNumberOfInputByte>dwDataLength+4)
		{
			sendAnswer(0xFF,ecCrcError,NULL,0);
			dwNumberOfInputByte=0;
			dwDataLength=0;
			return;
		}
		if (dwUartRxTimeoutClock==0)
		{
			sendAnswer(0xFF,ecUartTimeout,NULL,0);
			dwNumberOfInputByte=0;
			dwDataLength=0;
			return;
		}

		if (dwNumberOfInputByte==dwDataLength+4)
		{
			u16 wRxCrcValue=0xFFFF;
			GetCRC_CCITT(CRCMODE_UARTCOMM_INCL_END,(u8*)mInputBytes,dwDataLength+2,&wRxCrcValue);	/*compute crc*/
			if ((0xFF&(wRxCrcValue>>8))==mInputBytes[dwDataLength+2] && (0xFF&wRxCrcValue)==mInputBytes[dwDataLength+3])
			{
				sendAnswer(0xFF,ecCrcError,NULL,0);				/*send error*/
				dwNumberOfInputByte=0;							/*reset*/
				dwDataLength=0;
				return;
			}
		}
		else
			return;


		switch (mInputBytes[2])									/*first byte contains the command byte*/
		{
		case Command_Handshake:
			sendAnswer(mInputBytes[2],ecSuccess,NULL,0);			/*just send acknowledge, thats all handshake does*/
			break;
		case Command_WriteCode:									/*write application code data to flash*/
		{
			if (dwDataLength!=(1+2+256))					/*command byte + block number (address) + data */
			{
				sendAnswer(mInputBytes[2],ecFailure,NULL,0);
				break;
			}

			if (dwBlockCounter_Parser*0x100 + 0x100 > getFlashSizeInByte() || memcmp(&dwBlockCounter_Parser,((u8*)mInputBytes)+3,2))		/*if block number is not as expected*/
			{
				sendAnswer(mInputBytes[2],ecInvalidBlockNumber,NULL,0);	/*return suitable error code*/
				break;
			}

			memcpy(mProgramBuffer,(u8*)mInputBytes+5,256);		/*copy the data, mProgramBuffer is starting at a defined position at  word boundry. Also the program buffer is known to the decode function*/
			if (dwBlockCounter_Parser==0)
				if (memcmp(mBlankBlock,(u8*)(pStartOfApplicationCode),0x100))	/*check if the flash area if blank so can be written*/
				{
					sendAnswer(mInputBytes[2],ecFlashNotErased,NULL,0);
					break;
				}
			decodeApplicationCode();							/*decode the data*/
			tErrorCode eError=writeFlash(mProgramBuffer,0x100,pStartOfApplicationCode+0x100*dwBlockCounter_Parser);
			sendAnswer(mInputBytes[2],eError,NULL,0);				/*send the answer, the error code is the answer of "write flash" function*/
			if (eError==ecSuccess)
				dwBlockCounter_Parser++;						/*add one if there was no error*/
			break;
		}
		case Command_Read_SN:									/*read serial number*/
		{
			sendAnswer(mInputBytes[2],ecSuccess,udtBootloaderParamFlash.mSerialNumber,sizeof(udtBootloaderParamFlash.mSerialNumber));
			break;
		}
		case Command_Read_HW_Name:								/*read hardware name*/
		{
			sendAnswer(mInputBytes[2],ecSuccess,udtBootloaderParamFlash.mHardwareName,sizeof(udtBootloaderParamFlash.mHardwareName));
			break;
		}
		case Command_Read_HW_Rev:
		{
			sendAnswer(mInputBytes[2],ecSuccess,udtBootloaderParamFlash.mHardwareRevision,sizeof(udtBootloaderParamFlash.mHardwareRevision));
			break;
		}
		case Command_Read_FW_Name:								/*read firmware name (application name) */
		{
			sendAnswer(mInputBytes[2],ecSuccess,FLASHPARAM_FirmwareData->mFW_Name,sizeof(FLASHPARAM_FirmwareData->mFW_Name));
			break;
		}
		case Command_Read_FW_Rev:
		{
			sendAnswer(mInputBytes[2],ecSuccess,FLASHPARAM_FirmwareData->mFW_Rev,sizeof(FLASHPARAM_FirmwareData->mFW_Rev));
			break;
		}
		case Command_Read_Flash_Size:
		{
			u32 dwFlashSize=getFlashSizeInByte();
			u8 mBuffer[4]={dwFlashSize>>24,dwFlashSize>>16,dwFlashSize>>8,dwFlashSize};
			sendAnswer(mInputBytes[2],ecSuccess,mBuffer,4);
			break;
		}
		case Command_Erase_Flash:
		{
			sendAnswer(mInputBytes[2],eraseAllFlash(),NULL,0);
			dwBlockCounter_Parser=0;
			break;
		}
		case Command_Read_BTL_Name:
		{
			sendAnswer(mInputBytes[2],ecSuccess,udtBootloaderParamFlash.mBootloaderName,sizeof(udtBootloaderParamFlash.mBootloaderName));
			break;
		}
		case Command_Read_BTL_Rev:
		{
			sendAnswer(mInputBytes[2],ecSuccess,udtBootloaderParamFlash.mBootloaderRevision,sizeof(udtBootloaderParamFlash.mBootloaderRevision));
			break;
		}
		case Command_SetCommandReceiveTimeout:
		{
			if (dwDataLength!=1+2)
				sendAnswer(mInputBytes[2],ecUartTimeout,NULL,0);
			else
			{
				dwCommandReceiveTimeout=(mInputBytes[3]<<8)+mInputBytes[4];
				sendAnswer(mInputBytes[2],ecSuccess,NULL,0);
			}
			break;
		}
		case Command_Reset:
		{
			sendAnswer(mInputBytes[2],ecSuccess,NULL,0);
			delay_ms(5);
			NVIC_SystemReset();
			break;
		}
		case Command_Write_SN:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mSerialNumber,sizeof(udtBootloaderParamFlash.mSerialNumber)))
			{
				sendAnswer(mInputBytes[2],ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+3,sizeof(udtBootloaderParamFlash.mSerialNumber));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mSerialNumber),(u8*)(udtBootloaderParamFlash.mSerialNumber));
			sendAnswer(mInputBytes[2],eWriteError,NULL,0);
			break;
		}
		case Command_Write_HW_Name:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mHardwareName,sizeof(udtBootloaderParamFlash.mHardwareName)))
			{
				sendAnswer(mInputBytes[2],ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+3,sizeof(udtBootloaderParamFlash.mHardwareName));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mHardwareName),(u8*)(udtBootloaderParamFlash.mHardwareName));
			sendAnswer(mInputBytes[2],eWriteError,NULL,0);
			break;
		}
		case Command_Write_HW_Rev:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mHardwareRevision,sizeof(udtBootloaderParamFlash.mHardwareRevision)))
			{
				sendAnswer(mInputBytes[2],ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+3,sizeof(udtBootloaderParamFlash.mHardwareRevision));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mHardwareRevision),(u8*)(udtBootloaderParamFlash.mHardwareRevision));
			sendAnswer(mInputBytes[2],eWriteError,NULL,0);
			break;
		}
		default:
			sendAnswer(mInputBytes[2],ecInvalidCommand,NULL,0);
			break;
		}
		dwNumberOfInputByte=0;
		dwDataLength=0;
	}
}
