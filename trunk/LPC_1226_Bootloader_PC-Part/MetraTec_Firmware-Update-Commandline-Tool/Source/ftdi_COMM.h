/*
 * ftdi_COMM.h
 *
 *  Created on: 21.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#ifndef FTDI_COMM_H_
#define FTDI_COMM_H_

extern FT_HANDLE ftHandle;
Bool ftdiCOMM_Init(int DeviceToOpen);
Bool ftdiCOMM_WriteCommand(u8* mSendData, u32 dwLength);
u32 ftdiCOMM_ReadAnswer(u8* mRecvData, u32 dwMaxLength);

#endif /* FTDI_COMM_H_ */
