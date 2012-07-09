/*
 * main.c
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

int main(int argc, char *argv[])
{
	u8 mMetData[500000]={0};
	int iMetDataLength=0;
	int iDeviceNumber=-1;
	if (argc==3)
	{
		iDeviceNumber=0;
		for (int i=0;argv[1][i]>='0'&&argv[1][i]<='9';i++)
			iDeviceNumber=iDeviceNumber*10+argv[1][i]-'0';
	}
	if (FALSE==ftdiCOMM_Init(iDeviceNumber))
	{
		system("Pause");
		return 0;
	}
	if (argc==3)
		iMetDataLength=read_file(mMetData,sizeof(mMetData),argv[2]);
	else //should never occure
	{
		system("Pause");
		return -1;
	}

	printf("Hardware Name: ");
	for (u8 i=0;i<16;i++)
		fputc(mMetData[i],stdout);
	printf("\nHardware Revision: ");
	for (u8 i=32;i<36;i++)
		fputc(mMetData[i],stdout);
	printf("\nFirmware Name: ");
	for (u8 i=16;i<32;i++)
		fputc(mMetData[i],stdout);
	printf("\nFirmware Revision: ");
	for (u8 i=36;i<40;i++)
		fputc(mMetData[i],stdout);
	printf("\n");

	u16 wNumberOfBlocks=mMetData[40]+(((u16)mMetData[41])<<8);
	if (iMetDataLength!=42+wNumberOfBlocks*261)
	{
		printf("met-file length does not match: %i %i\n",iMetDataLength,42+wNumberOfBlocks*261);
		system("Pause");
		return 0;
	}

	u8 mCommand[20];
	u8 mAnswer[50];
	int iNumberRead=0;

	mCommand[0]=0x0A;
	mCommand[1]=0xDD;
	mCommand[2]=0xA0;
	do
	{
		FT_SetTimeouts(ftHandle,5000,20);
		if (ftdiCOMM_WriteCommand(mCommand,3))
		{
			iNumberRead=ftdiCOMM_ReadAnswer(mAnswer,sizeof(mAnswer));
//			printf("%i\n",iNumberRead);
//			for (int i=0;i<iNumberRead;i++)
//				printf("%02x ",mAnswer[i]);
//			printf("\n");
			if (4==iNumberRead)
			{
				if (mAnswer[0]==0x8A || mAnswer[1]==0x00)
					continue;
			}
		}
		printf("Erase Failed");
		system("Pause");
		return 0;
	}while(0);

	FT_SetTimeouts(ftHandle,150,50);

	for (u16 i=0;i<wNumberOfBlocks;i++)
	{
		printf("%03i of %03i\n",i,wNumberOfBlocks);
		if (ftdiCOMM_WriteCommand(mMetData+42+i*261,261))
			if (4==ftdiCOMM_ReadAnswer(mAnswer,sizeof(mAnswer)))
				if (mAnswer[0]==0x82&&mAnswer[1]==0x00)
					continue;
		printf("Write Failed");
		system("Pause");
		return 0;
	}

	FT_Close(ftHandle);
	printf("Success on Update!");
	system("Pause");
	return 0;
}
