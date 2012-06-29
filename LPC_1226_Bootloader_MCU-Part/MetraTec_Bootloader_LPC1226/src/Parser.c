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
#define Command_Handshake 0x01
#define Command_WriteCode 0x02
#define Command_Read_SN 0x03
#define Command_Read_HW_Name 0x04
#define Command_Read_HW_Rev 0x05
#define Command_Read_FW_Name 0x06
#define Command_Read_FW_Rev 0x07
#define Command_Erase_Flash 0x0A
#define Command_Read_BTL_Rev 0x0C
#define Command_Reset 0x41
#define Command_Write_SN 0x43
#define Command_Write_HW_Name 0x44
#define Command_Write_HW_Rev 0x45

const u8 const mAES_Key[16]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};	//TODO: Change the key
u8 mExpandedKey[4 * Nb * (Nr + 1)];
void InitAES (void)
{
	ExpandKey (mAES_Key, mExpandedKey);
}
static void decodeApplicationCode(void)
{
	u16 i = 0;
	static unsigned char p = 0;
	const u8 Key[18] = {"MetraTecBootloader"};	//TODO: change the Key

	//XOR
	for(i = 0; i < 256; i++)
	{
		*(mProgramBuffer+i) ^= Key[p];
		p++;
		if(p >= 18)
		  p = 0;
	}

	//Aes

	for (u16 i=0;i<256;i+=16)
		AesDecrypt (mProgramBuffer+i, mExpandedKey, mProgramBuffer+i);
}

void Parser(void)
{
	static u32 dwBlockCounter_Parser=0;
	const u8 const mBlankBlock[256]={
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	};

	if (dwNumberOfInputByte && dwUartRxTimeoutClock==0)
	{
		u32 dwParsingLength=dwNumberOfInputByte; //so the

		if(dwParsingLength<3)//1Byte Data, 2 Byte CRC
		{
			sendAnswer(0x00,ecUartTimeout,NULL,0);
			dwNumberOfInputByte=0;
			return;
		}
		u16 wRxCrcValue=0xFFFF;
		GetCRC_CCITT(CRCMODE_UARTCOMM_INCL_END,(u8*)mInputBytes,dwParsingLength-2,&wRxCrcValue);
		if (memcmp(&wRxCrcValue,((u8*)mInputBytes+dwParsingLength-2),2))
		{
			sendAnswer(mInputBytes[0],ecCrcError,NULL,0);
			dwNumberOfInputByte=0;
			return;
		}

		switch (mInputBytes[0])
		{
		case Command_Handshake:
			sendAnswer(*mInputBytes,ecSuccess,NULL,0);
			break;
		case Command_WriteCode:
		{
			if (dwParsingLength<(1+1+256+2))
			{
				sendAnswer(*mInputBytes,ecUartTimeout,NULL,0);
				break;
			}
			u16 wBlockNumber;
			memcpy(&wBlockNumber,(u8*)mInputBytes+1,2);
			if (wBlockNumber!=dwBlockCounter_Parser)
			{
				sendAnswer(*mInputBytes,ecInvalidBlockNumber,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+3,256);
			if (memcmp(mBlankBlock,(u8*)(pStartOfApplicationCode+(wBlockNumber*0x100)),0x100))
			{
				sendAnswer(*mInputBytes,ecFlashNotErased,NULL,0);
				break;
			}
			decodeApplicationCode();
			sendAnswer(*mInputBytes,writeFlash(mProgramBuffer,0x100,pStartOfApplicationCode+0x100*wBlockNumber),NULL,0);
			dwBlockCounter_Parser++;
			break;
		}
		case Command_Read_SN:
		{
			sendAnswer(*mInputBytes,ecSuccess,udtBootloaderParamFlash.mSerialNumber,sizeof(udtBootloaderParamFlash.mSerialNumber));
			break;
		}
		case Command_Read_HW_Name:
		{
			sendAnswer(*mInputBytes,ecSuccess,udtBootloaderParamFlash.mHardwareName,sizeof(udtBootloaderParamFlash.mHardwareName));
			break;
		}
		case Command_Read_HW_Rev:
		{
			sendAnswer(*mInputBytes,ecSuccess,udtBootloaderParamFlash.mHardwareRevision,sizeof(udtBootloaderParamFlash.mHardwareRevision));
			break;
		}
		case Command_Read_FW_Name:
		{
			sendAnswer(*mInputBytes,ecSuccess,FLASHPARAM_FirmwareData->mFW_Name,sizeof(FLASHPARAM_FirmwareData->mFW_Name));
			break;
		}
		case Command_Read_FW_Rev:
		{
			sendAnswer(*mInputBytes,ecSuccess,FLASHPARAM_FirmwareData->mFW_Rev,sizeof(FLASHPARAM_FirmwareData->mFW_Rev));
			break;
		}
		case Command_Erase_Flash:
		{
			sendAnswer(*mInputBytes,eraseAllFlash(),NULL,0);
			dwBlockCounter_Parser=0;
			break;
		}
		case Command_Read_BTL_Rev:
		{
			sendAnswer(*mInputBytes,ecSuccess,udtBootloaderParamFlash.mBootloaderRevision,sizeof(udtBootloaderParamFlash.mBootloaderRevision));
			break;
		}
		case Command_Reset:
		{
			sendAnswer(*mInputBytes,ecSuccess,NULL,0);
			delay_ms(5);
			NVIC_SystemReset();
			break;
		}
		case Command_Write_SN:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mSerialNumber,sizeof(udtBootloaderParamFlash.mSerialNumber)))
			{
				sendAnswer(*mInputBytes,ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+1,sizeof(udtBootloaderParamFlash.mSerialNumber));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mSerialNumber),(u8*)(udtBootloaderParamFlash.mSerialNumber));
			sendAnswer(*mInputBytes,eWriteError,NULL,0);
			break;
		}
		case Command_Write_HW_Name:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mHardwareName,sizeof(udtBootloaderParamFlash.mHardwareName)))
			{
				sendAnswer(*mInputBytes,ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+1,sizeof(udtBootloaderParamFlash.mHardwareName));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mHardwareName),(u8*)(udtBootloaderParamFlash.mHardwareName));
			sendAnswer(*mInputBytes,eWriteError,NULL,0);
			break;
		}
		case Command_Write_HW_Rev:
		{
			if (memcmp(mBlankBlock,udtBootloaderParamFlash.mHardwareRevision,sizeof(udtBootloaderParamFlash.mHardwareRevision)))
			{
				sendAnswer(*mInputBytes,ecFailure,NULL,0);
				break;
			}
			memcpy(mProgramBuffer,(u8*)mInputBytes+1,sizeof(udtBootloaderParamFlash.mHardwareRevision));
			tErrorCode eWriteError=writeFlash(mProgramBuffer,sizeof(udtBootloaderParamFlash.mHardwareRevision),(u8*)(udtBootloaderParamFlash.mHardwareRevision));
			sendAnswer(*mInputBytes,eWriteError,NULL,0);
			break;
		}
		default:
			sendAnswer(mInputBytes[0],ecInvalidCommand,NULL,0);
			break;
		}
		dwNumberOfInputByte=0;
	}
}
