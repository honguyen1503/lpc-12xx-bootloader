/*
 * CRC.c
 *
 *  Created on: 25.06.2012
 *  Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

#define CRCPOLYNOM 0x8408			/*iso 15693 crc polynom*/
static void IsoGetCrc ( u8* mData, u16 wPolynom, u32 bLength, u16* pCrc)
{
 	for (u32 i=0; i<bLength; i++)
  	{
 		(*pCrc)^=mData[i];							/*XOR the init value and the data*/
 		for (u8 j=0; j<8; j++)						/*for all bits*/
 		{
 			if ((*pCrc) & 0x01)						/*if last bit is set*/
			{
 				(*pCrc)>>=1;						/*move by one bit*/
 				(*pCrc)^=wPolynom;					/*invert some bits (XOR)*/
			}
 			else
 				(*pCrc)>>=1;						/*move by one bit*/
 		}
  	}
}

Bool SetMetCRC(u8 *buffer)
{
	if (*((u32*)(buffer+CODE_FW_CRC_IN_MET))!=0xFFFFFFFF)
		return FALSE;
	if (*((u32*)(buffer+CODE_FW_CRC_IN_MET+4))!=0xFFFFFFFF)
		return FALSE;
	/*
	 * 	CRC of Code was not computed by now so it's the default (important value for program code to set it to 0xFFFFFFFF.
	 * 	Just  using the same value is NOT sufficient because the flash must be writable!
	 */
	u16 wCRC=0xFFFF;	//use preset value
	u32 dwCRC;			//variable to set Crc to 4 Byte sized memory area
	u32 dwLength=*((u32*)(buffer+CODE_FW_LENGTH_IN_BYTE_ADDR));
	IsoGetCrc(									//get the CRC first part (pre CRC saving area)
			buffer,								//Start of computation address is the start of application address
			CRCPOLYNOM,
			CODE_FW_CRC_IN_MET,					//length is the difference between the start address and the address of the first CRC data
			&wCRC);								//the variable to get the start value from and to save the crc value to
	IsoGetCrc(									//get the CRC second part (post CRC saving area)
			buffer+(CODE_FW_CRC_IN_MET+8),		//starts after CRC
			CRCPOLYNOM,
			dwLength-(CODE_FW_CRC_IN_MET+8),	//length is the length of the whole code
			&wCRC);								//the variable to get the start value from and to save the crc value to
	dwCRC=wCRC;									//and copy to 4Byte variable. THis is the smallest number of bytes to write afaik
	memcpy(buffer+CODE_FW_CRC_IN_MET,&dwCRC,sizeof(u32));
	return TRUE;
}
