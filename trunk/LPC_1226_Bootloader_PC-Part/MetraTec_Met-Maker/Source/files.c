/*
 * files.c
 *
 *  Created on: 18.06.2012
 *      Author: Martin Köhler, metraTec GmbH
 */

#include "GlobalIncludes.h"

int read_file (unsigned char* inbuf, unsigned int length, char* filename)
{
	int rtrn;
	FILE *file;
	if ((file = fopen(filename, "rb")) == 0)
		return 0;
	rtrn = fread((void*)inbuf,1,length,file);
	fclose(file);
	return rtrn;
}

int write_file (unsigned char* inbuf, unsigned int length, const char* filename)
{
	int rtrn;
	FILE *file;
	if ((file = fopen(filename, "wb")) == 0)
		return 0;
	rtrn = fwrite((void*)inbuf,1,length,file);
	fclose(file);
	return rtrn;
}

//unsigned char *build_output_filename (unsigned char *buffer, unsigned char Hardware)
//{
// static unsigned char String_Buf[metFileNameSize];
// unsigned char* String;
// unsigned char i,j;
// String = String_Buf;
// *(String++) = '\\';
// for (i=CODE_FW_NAME_Length-1; i>0; i--)
//     if (*(buffer+CODE_FW_NAME_Addr+i) > ' ')
//        break;
// for (j=0; j<i+1; j++)
//     *(String++) = *(buffer+CODE_FW_NAME_Addr+j);
// *(String++) = '_';
// *(String++) = 'r';
// if (*(buffer+CODE_FW_Rev_Addr) != '0')
//    *(String++) = *(buffer+CODE_FW_Rev_Addr);
// *(String++) = *(buffer+CODE_FW_Rev_Addr+1);
// *(String++) = '-';
// if (*(buffer+CODE_FW_Rev_Addr+2) != '0')
//    *(String++) = *(buffer+CODE_FW_Rev_Addr+2);
// *(String++) = *(buffer+CODE_FW_Rev_Addr+3);
//
// if (Hardware)
// {
//  *(String++) = '_';
//  *(String++) = 'o';
//  *(String++) = 'n';
//  *(String++) = '_';
//  for (i=CODE_HW_NAME_Length-1; i>0; i--)
//     if (*(buffer+CODE_HW_NAME_Addr+i) > ' ')
//        break;
//  for (j=0; j<i+1; j++)
//     *(String++) = *(buffer+CODE_HW_NAME_Addr+j);
// *(String++) = '_';
// *(String++) = 'r';
// if (*(buffer+CODE_HW_Rev_Addr) != '0')
//    *(String++) = *(buffer+CODE_HW_Rev_Addr);
// *(String++) = *(buffer+CODE_HW_Rev_Addr+1);
// *(String++) = '-';
// if (*(buffer+CODE_HW_Rev_Addr+2) != '0')
//    *(String++) = *(buffer+CODE_HW_Rev_Addr+2);
// *(String++) = *(buffer+CODE_HW_Rev_Addr+3);
// }
// *(String++) = '.';
// *(String++) = 'm';
// *(String++) = 'e';
// *(String++) = 't';
// *(String++) = 0;
// return String_Buf;
//}
