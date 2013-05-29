/*
 * ftdi_COMM.c
 *
 *  Created on: 21.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

FT_HANDLE ftHandle=0;

u16 GetCRCu8(u8 Data, u16 CRC) //Makes a crc word
{
	CRC^=Data;
	for (u8 j=0; j<8; j++)
	{
		if ((CRC & 0x0001)==0)
			CRC>>=1;
		else
			CRC=(CRC>>1)^0x8408;
	 }
	return CRC;
}

u16 IsoGetCrc ( unsigned char *AData, unsigned char ASize, unsigned int APolynom )
{
 	u16 BCrc=0xFFFF;
 	u8 i, j;

 	for (i=0; i<ASize; i++)
  	{
   		BCrc^=AData[i];
   		for (j=0; j<8; j++)
    	{
     		if ((BCrc & 0x0001)==0) BCrc>>=1;
     		else BCrc=(BCrc>>1)^APolynom;
    	}
  	}
	 return (BCrc);
}

Bool ftdiCOMM_Init(int DeviceToOpen)
{
	DWORD ftStatus=0;
	DWORD dwNumberOfFtdis=0;
	ftStatus=FT_CreateDeviceInfoList(&dwNumberOfFtdis);

	if (DeviceToOpen==-1)
	{
		FT_DEVICE_LIST_INFO_NODE DeviceInfos[dwNumberOfFtdis];
		ftStatus=FT_GetDeviceInfoList(DeviceInfos,&dwNumberOfFtdis);
		if (FT_OK==ftStatus)
		{
			printf("NumberOfDevices=0x%08x\n",(u32)dwNumberOfFtdis);
			for (int i=0;i<dwNumberOfFtdis;i++)
			{
				ftHandle=DeviceInfos[i].ftHandle;
				printf("Dev %d:\n",i);
				printf("  Flags=0x%08x\n",(u32)DeviceInfos[i].Flags);
				printf("  Type=0x%08x\n",(u32)DeviceInfos[i].Type);
				printf("  ID=0x%08x\n",(u32)DeviceInfos[i].ID);
				printf("  LocId=0x%08x\n",(u32)DeviceInfos[i].LocId);
				printf("  SerialNumber=%s\n",DeviceInfos[i].SerialNumber);
				printf("  Description=%s\n",DeviceInfos[i].Description);
				printf("  ftHandle=0x%08x\n",(u32)DeviceInfos[i].ftHandle);
			}
			return FALSE;
		}
		else
		{
			printf("Getting Device List failed\nError code: %ld\n",ftStatus);
			return FALSE;
		}
	}
	else if (DeviceToOpen>dwNumberOfFtdis-1)
	{
		printf("Device does not exist\nNumber of Devices: %i\n",(u32)dwNumberOfFtdis);
		return FALSE;
	}

	if (FT_OK!= FT_Open(DeviceToOpen,&ftHandle))
	{
		printf("Could not open Device\n");
		return FALSE;
	}
	ftStatus=FT_SetBaudRate(ftHandle, FT_BAUD_115200);
	ftStatus=FT_SetDataCharacteristics(ftHandle,FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE);
	ftStatus=FT_SetTimeouts(ftHandle,1000,1000);
	ftStatus=FT_ClrDtr(ftHandle);
	ftStatus=FT_SetDtr(ftHandle);
	ftStatus=FT_SetLatencyTimer(ftHandle,1);
	return TRUE;
}

Bool ftdiCOMM_WriteCommand(u8* mSendData, u32 dwLength)
{
	DWORD ftStatus=0;
	DWORD Written=0;
	u8  mLength[2]={dwLength>>8,dwLength};

	ftStatus=FT_Write(ftHandle,mLength,2,&Written);
	if (FT_OK!= ftStatus)
	{
		printf("Did not write\nError code %ld\n",ftStatus);
		return FALSE;
	}
	else if (Written!=2)
	{
		printf("Write timeout\n");
		return FALSE;
	}

	ftStatus=FT_Write(ftHandle,mSendData, dwLength,&Written);
	if (FT_OK!= ftStatus)
	{
		printf("Did not write\nError code %ld\n",ftStatus);
		return FALSE;
	}
	else if (dwLength!=Written)
	{
		printf("Write timeout\n");
		return FALSE;
	}

	u16 wCrc=0xFFFF;
	wCrc=GetCRCu8(dwLength>>8,wCrc);
	wCrc=GetCRCu8(dwLength,wCrc);
	for (u32 dwI=0;dwI<dwLength;dwI++)
		wCrc=GetCRCu8(mSendData[dwI],wCrc);

	ftStatus=FT_Write(ftHandle,(u8*)&wCrc, 2,&Written);
	if (FT_OK!= ftStatus)
	{
		printf("Did not write\nError code %ld\n",ftStatus);
		return FALSE;
	}
	else if (2!=Written)
	{
		printf("Write timeout\n");
		return FALSE;
	}
	else
		return TRUE;
}

s32 ftdiCOMM_ReadAnswer(u8* mRecvData, u32 dwMaxLength)
{
	DWORD TotalRead=0;
	u16 wLength=0;
	u16 wCrc=0xFFFF;
	u8 mBuffer[2];

	DWORD ftStatus=FT_Read(ftHandle,mBuffer,2,&TotalRead);
	if (FT_OK!= ftStatus)
	{
		printf("Read failed\nError code %ld\n",ftStatus);
		return -1;
	}
	else if (TotalRead!=2)
	{
		printf("Read timeout\n");
		return -1;
	}
	wLength=(mBuffer[0]>>8) + mBuffer[1];

	if (wLength>dwMaxLength)
	{
		printf("Read Buffer Overflow\n");
		u8 mHelpBuffer[wLength+2];
		FT_Read(ftHandle,mHelpBuffer,wLength+2,&TotalRead);
		return -1;
	}

	if (wLength!=0)
	{
		ftStatus=FT_Read(ftHandle,mRecvData,wLength,&TotalRead);
		if (FT_OK!= ftStatus)
		{
			printf("Read failed\nError code %ld\n",ftStatus);
			return -1;
		}
		else if (TotalRead!=wLength)
		{
			printf("Read timeout\n");
			return -1;
		}
	}

	wCrc=GetCRCu8(mBuffer[0],wCrc);
	wCrc=GetCRCu8(mBuffer[1],wCrc);
	for (u32 i=0;i<wLength;i++)
		wCrc=GetCRCu8(mRecvData[i],wCrc);

	ftStatus=FT_Read(ftHandle,mBuffer,2,&TotalRead);
	if (FT_OK!= ftStatus)
	{
		printf("Read failed\nError code %ld\n",ftStatus);
		return -1;
	}
	else if (TotalRead!=2)
	{
		printf("Read timeout\n");
		return -1;
	}
	else if (wCrc!=(mBuffer[0]<<8) + mBuffer[1])
	{
		printf("CRC Error\n");
		return -1;
	}
	else
		return wLength;
}
