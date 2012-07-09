/*
 * Keys.c
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

const u8 const mAES_Key[16]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};	//TODO: Change the key
const u8 Key[18] = {"MetraTecBootloader"};	//TODO: change the Key
u8 mExpandedKey[4 * Nb * (Nr + 1)];
