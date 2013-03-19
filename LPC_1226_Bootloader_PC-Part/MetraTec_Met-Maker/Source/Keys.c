/*
 * Keys.c
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

const u8 const mAES_Key[16]= KEY_AES;
const u8 Key[18] = KEY_XOR;	//TODO: change the Key
u8 mExpandedKey[4 * Nb * (Nr + 1)];
