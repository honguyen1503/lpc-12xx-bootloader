/*
 * Defines.h
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define CMD_WRITE				0x02
#define CMD_HANDSHAKE			0x01

#define CODE_FW_NAME_Addr					0x0100
#define CODE_FW_NAME_Length					16
#define CODE_FW_Rev_Addr					0x0110
#define CODE_FW_Rev_Length					4
#define CODE_HW_NAME_Addr					0x0114
#define CODE_HW_NAME_Length					16
#define CODE_HW_Rev_Addr					0x0124
#define CODE_HW_Rev_Length					4
#define CODE_FW_LENGTH_IN_BYTE_ADDR			0x0128
#define CODE_FW_CRC_IN_MET					0x012C

#define metFileNameSize (CODE_HW_NAME_Length+CODE_HW_Rev_Length+CODE_FW_NAME_Length+CODE_FW_Rev_Length+15)



#endif /* DEFINES_H_ */
