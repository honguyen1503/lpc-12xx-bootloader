/*
 * CRC.h
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#ifndef CRC_H_
#define CRC_H_

#define CRCPOLYNOM 	0x8408
#define CRCSTART	0xffff
#define _Calc_CRC(bData,wCrc) 												\
	do																		\
	{																		\
		(wCrc)^=(bData);													\
		unsigned char iii;													\
		for (iii=0; iii<8; iii++)					/*for all bits*/		\
		{																	\
			if ((wCrc) & 1)						/*if last bit is set*/		\
			{																\
				(wCrc)>>=1;						/*move by one bit*/			\
				(wCrc)^=CRCPOLYNOM;				/*invert some bits (XOR)*/	\
			}																\
			else															\
				(wCrc)>>=1;						/*move by one bit*/			\
		}																	\
	}while(0)

Bool SetMetCRC(u8* buffer);
#endif /* CRC_H_ */
