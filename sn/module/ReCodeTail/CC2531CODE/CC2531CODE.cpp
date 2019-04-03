// CC2531CODE.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define uint8 unsigned char
#define uint32 unsigned int
#define uint16 unsigned short

#define PREAMBLE_OFFSET            0x8C
#define PREAMBLE_LENSET			(0X8C+2)

#define HAL_OAD_RC_START           0x00/*128*1024*///0x1000//0x0800//0x1000//0x2000//0x2000//0x0800//0x5000//0x0800
#define HAL_OAD_CRC_ADDR           0x88

#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

char g_sBuf[512*1024]={0xFF};
unsigned long g_fileLen;
unsigned long g_rearLen;


int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfileRes;
	FILE *pfileRst;
	unsigned long rstLen;
	//preamble_t preamble;

	//char sBuf[256*1024];
	uint16 crc[2];

	//(char*)preamble = (g_sBuf+PREAMBLE_OFFSET);
	//memcpy((char*)&preamble,g_sBuf+PREAMBLE_OFFSET,sizeof(preamble_t));

	//uint16 t_crc = 0x321d;
	//t_crc = runPoly(t_crc, 0x44);


	if((pfileRes = fopen("cc2530.cryp","rb")) == NULL)
	{
		printf("open cc2530.cryp error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	g_fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(g_sBuf,1,g_fileLen,pfileRes);

	fclose(pfileRes);

	if((pfileRes = fopen("cc2530_rear.jpg","rb")) == NULL)
	{
		printf("open cc2530_rear.jpg error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	g_rearLen = ftell(pfileRes); 
	rewind(pfileRes);

	if (g_fileLen+g_rearLen<(512*1024))
	{
		fread(g_sBuf+g_fileLen,1,g_rearLen,pfileRes);
	}else
	{
		fread(g_sBuf+g_fileLen,1,(512*1024)-g_fileLen,pfileRes);
	}	

	if ((pfileRst = fopen("hxCompatibleModle.upfw","wb+")) == NULL)
	{
		printf("open hxCompatibleModle.upfw error\r\n");
		goto Err;
	}

	fwrite(g_sBuf,(512*1024),1,pfileRst);

	fclose(pfileRes);

	fclose(pfileRst);

	Err:
	while(1);

	while(1);

	return 0;

}

