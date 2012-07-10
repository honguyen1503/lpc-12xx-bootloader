/*
Copyright (c) 2012 Martin Koehler, metraTec GmbH (metraTec.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS",
WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file uses code from public domain, see below for the disclaimer:
// advanced encryption standard
// author: karl malbrain, malbrain@yahoo.com
/*
This work, including the source code, documentation
and related data, is placed into the public domain.

The orginal author is Karl Malbrain.

THIS SOFTWARE IS PROVIDED AS-IS WITHOUT WARRANTY
OF ANY KIND, NOT EVEN THE IMPLIED WARRANTY OF
MERCHANTABILITY. THE AUTHOR OF THIS SOFTWARE,
ASSUMES _NO_ RESPONSIBILITY FOR ANY CONSEQUENCE
RESULTING FROM THE USE, MODIFICATION, OR
REDISTRIBUTION OF THIS SOFTWARE.
*/

#ifndef AESSTABLE_H_
#define AESSTABLE_H_



// AES only supports Nb=4
#define Nb 4			// number of columns in the state & expanded key
#define Nk 4			// number of columns in a key
#define Nr 10			// number of rounds in encryption


extern const u8 const mAES_Key[16];
extern u8 mExpandedKey[4 * Nb * (Nr + 1)];

extern const u8 const Sbox[256];		// forward s-box
extern const u8 const InvSbox[256];	// inverse s-box

// combined Xtimes2[Sbox[]]
extern const u8 const Xtime2Sbox[256];
// combined Xtimes3[Sbox[]]
extern const u8 const Xtime3Sbox[256];

// modular multiplication tables
// based on:

// Xtime2[x] = (x & 0x80 ? 0x1b : 0) ^ (x + x)
// Xtime3[x] = x^Xtime2[x];

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
