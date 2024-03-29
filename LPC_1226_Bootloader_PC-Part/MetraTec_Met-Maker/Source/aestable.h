/*
 * aesstable.h
 *
 *  Created on: 13.02.2012
 *  The orginal author is Karl Malbrain
 *  Changes: Martin K�hler, metraTec GmbH
 */

#ifndef AESSTABLE_H_
#define AESSTABLE_H_

// AES only supports Nb=4
#define Nb 4			// number of columns in the state & expanded key
#define Nk 4			// number of columns in a key
#define Nr 10			// number of rounds in encryption

typedef uint8_t 	u8;
typedef int8_t 		s8;
typedef uint16_t 	u16;
typedef int16_t 	s16;
typedef uint32_t 	u32;
typedef int32_t 	s32;
typedef int64_t		s64;
typedef enum { FALSE = 0, TRUE  = 1} Bool;


extern const u8 const Sbox[256];		// forward s-box
extern const u8 const InvSbox[256];	// inverse s-box
extern const u8 const Xtime2Sbox[256];
extern const u8 const Xtime3Sbox[256];
extern const u8 const Xtime2[256];
extern const u8 const Xtime9[256];
extern const u8 const XtimeB[256];
extern const u8 const XtimeD[256];
extern const u8 const XtimeE[256];

// exchanges columns in each of 4 rows
// row0 - unchanged, row1- shifted left 1,
// row2 - shifted left 2 and row3 - shifted left 3
void ShiftRows (u8 *state);

// restores columns in each of 4 rows
// row0 - unchanged, row1- shifted right 1,
// row2 - shifted right 2 and row3 - shifted right 3
void InvShiftRows (u8 *state);

// recombine and mix each row in a column
void MixSubColumns (u8 *state);

// restore and un-mix each row in a column
void InvMixSubColumns (u8 *state);

// encrypt/decrypt columns of the key
// n.b. you can replace this with
//      byte-wise xor if you wish.

void AddRoundKey (u8 *state, u8 *key);

extern const u8 const Rcon[11];

// produce Nb bytes for each round
void ExpandKey (const u8* const key, u8 *expkey);

// encrypt one 128 bit block
void AesEncrypt (const u8* const in, u8 *expkey, u8 *out);
void AesDecrypt (const u8* const in, u8 *expkey, u8 *out);


#endif /* AESSTABLE_H_ */
