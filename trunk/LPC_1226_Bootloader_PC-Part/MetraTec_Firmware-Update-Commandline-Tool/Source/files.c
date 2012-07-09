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

int write_file (unsigned char* inbuf, unsigned int length, char* filename)
{
	int rtrn;
	FILE *file;
	if ((file = fopen(filename, "wb")) == 0)
		return 0;
	rtrn = fwrite((void*)inbuf,1,length,file);
	fclose(file);
	return rtrn;
}
