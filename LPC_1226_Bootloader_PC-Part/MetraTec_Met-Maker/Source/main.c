#include "GlobalIncludes.h"

/**************************************************************************************************
 * @fn          Encrypt
 *
 * @brief       encrypt program data.
 *
 * input parameters
 *
 * @param       Data - data to be encrypted.
 * @param       Length - length of data (has to be even).
 *
 * output parameters
 *
 * None.
 *
 * @return      
 **************************************************************************************************
 */

static void Encrypt(unsigned char *Data, unsigned short Length)
{
	unsigned short i = 0;		//for counter
	static unsigned char p = 0;	//XOR key pointer

	if (0==Length)				//reset pointer
		p=0;
	
	//AES
	if (Length==256)
    	for (i=0;i<256;i+=16)
        	AesEncrypt(Data+i, mExpandedKey, Data+i);
	
	//XOR
	for(i = 0; i < Length; i++)
	{
		*(Data+i) ^= Key[p];
		p++;
		if(p >= 18) 
		  p = 0;
	}
}

int main(int argc, char *argv[])
{
	unsigned char buffer[300000]={0};	//buffers the input data
	unsigned char output[300000]={0};	//buffers the output data
	int i, wNumberOfBlocks;			//count variables
	unsigned char Show_Flag = 0;
	unsigned char* pOutputPointer=output;
	unsigned char* pBufferPointer=buffer;
	if (argc < 3)
		return 0;
	ExpandKey (mAES_Key, mExpandedKey);  //INITS AES
	for (i=3; i<argc; i++)
	{
		if (strcmp(argv[i],"-d") == 0)			// show dialogue
		{
			Show_Flag = 1;
			continue;
		}
		return 0;
	}

	unsigned int read_size;
	read_size = read_file(buffer, sizeof(buffer), argv[1]);
	if (read_size&0xFF)
	   wNumberOfBlocks=(read_size>>8)+1;
    else
        wNumberOfBlocks=read_size>>8;
    memset(buffer+read_size,0xFF,0x100-(read_size&0xFF));
    *((u32*)(buffer+CODE_FW_LENGTH_IN_BYTE_ADDR))=read_size;

    if (FALSE==SetMetCRC(buffer))
    {
    	printf("Crc not empty\n");
    	system("Pause");
    	return -3;
    }
    write_file(buffer,read_size+0x100-(read_size&0xFF),"NotEncrypted.hex");

	printf("Hardware Name   : ");
	for (i=0; i<CODE_HW_NAME_Length; i++)
	{
		*pOutputPointer = buffer[CODE_HW_NAME_Addr+i];
		pOutputPointer++;
		printf("%c",buffer[CODE_HW_NAME_Addr+i]);
	}
	printf("\n");
	
	printf("Firmware Name   : ");
	for (i=0; i<CODE_FW_NAME_Length; i++)
	{
		*pOutputPointer = buffer[CODE_FW_NAME_Addr+i];
		pOutputPointer++;
		printf("%c",buffer[CODE_FW_NAME_Addr+i]);
	}
	printf("\n");
	
	printf("Hardware Rev   : ");
	for (i=0; i<CODE_HW_Rev_Length; i++)
	{
	    *pOutputPointer = buffer[CODE_HW_Rev_Addr+i];
		pOutputPointer++;
		printf("%c",buffer[CODE_HW_Rev_Addr+i]);
	}
	printf("\n");
	
	printf("Firmware Rev   : ");
	for (i=0; i<CODE_FW_Rev_Length; i++)
	{
		*pOutputPointer = buffer[CODE_FW_Rev_Addr+i];
		pOutputPointer++;
		printf("%c",buffer[CODE_FW_Rev_Addr+i]);
	}
	printf("\n");
	
	*pOutputPointer = (unsigned char)(wNumberOfBlocks&0xff);
    pOutputPointer++;
    *pOutputPointer = (unsigned char)(wNumberOfBlocks>>8);
	pOutputPointer++;
    printf("Code Blocks    : %d\n", wNumberOfBlocks);

	
	unsigned short strBufLen=strlen(argv[2]);               //get length of target file name
	const const unsigned char mCompareString[4]={".met"};                  //compare end (.met)
	
	if (!(strBufLen>4 && 0==memcmp(argv[2]+strBufLen-4,mCompareString,4))) //long enough (at least 1 char before .met) and ends on .met
    {
    	printf("metFile Name not OK!\n");
    	return -2;
    }

	
	for (i=0; i<wNumberOfBlocks; i++)
	{
		Encrypt(pBufferPointer, 256);

		*pOutputPointer= CMD_WRITE;
		pOutputPointer++;
		
		*pOutputPointer = (unsigned char)(i&0xff);
		pOutputPointer++;
		*pOutputPointer = (unsigned char)(i>>8);
		pOutputPointer++;
		
        memcpy(pOutputPointer,pBufferPointer,256);
		pOutputPointer+=256;
		pBufferPointer+=256;
	}		
	
	printf("\n%d of %d Bytes written to met-File %s\n\n", write_file(output, pOutputPointer-output, (char*)argv[2]),pOutputPointer-output,argv[2]);
	if (Show_Flag)
		system("PAUSE");
  return 0;
}
