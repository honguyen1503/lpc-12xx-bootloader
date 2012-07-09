/*
 * ftdi_COMM.c
 *
 *  Created on: 21.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

FT_HANDLE ftHandle=0;

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
	return TRUE;
}

Bool ftdiCOMM_WriteCommand(u8* mSendData, u32 dwLength)
{
	DWORD ftStatus=0;
	DWORD Written=0;
	ftStatus=FT_Write(ftHandle,mSendData, dwLength,&Written);
	if (FT_OK!= ftStatus)
	{
		printf("Did not write\nError code %ld\n",ftStatus);
		return FALSE;
	}
	else
	{
		if (dwLength!=Written)
		{
			printf("Write timeout\n");
			return FALSE;
		}
		else
			return TRUE;
	}
}

u32 ftdiCOMM_ReadAnswer(u8* mRecvData, u32 dwMaxLength)
{
	DWORD TotalRead=0;
	DWORD ftStatus=FT_Read(ftHandle,mRecvData,dwMaxLength,&TotalRead);
	if (FT_OK!= ftStatus)
	{
		printf("Read failed\nError code %ld\n",ftStatus);
		return FALSE;
	}
	else
		return TotalRead;
}
