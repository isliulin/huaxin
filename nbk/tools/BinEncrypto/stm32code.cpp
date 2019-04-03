// CC2531CODE.cpp : 定义控制台应用程序的入口点。
//
#include <windows.h>
#include "stdafx.h"
#include "enc.h"

//char *g_decKey = "WorkForYourDream";
//char *g_decKey = "NeverGiveupNikon";

#define uint8 unsigned char
#define uint32 unsigned int
#define uint16 unsigned short

#define MAX_LEN (128*1024)
//char g_sBuf[256*1024];
//char g_sBuf[512*1024];
//char g_sBuf[100*1024];
char g_sBuf[MAX_LEN];
unsigned long g_fileLen;

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfileRes;
	FILE *pfileRst;
	//FILE *pfileRear;
	EncMethod fEnc;
	char *pSourceName;
	char *pDestName;
	char *decKey;
	//preamble_t preamble;

	//char sBuf[256*1024];
//	uint16 crc[2];

	//(char*)preamble = (g_sBuf+PREAMBLE_OFFSET);
	//memcpy((char*)&preamble,g_sBuf+PREAMBLE_OFFSET,sizeof(preamble_t));

	//uint16 t_crc = 0x321d;
	//t_crc = runPoly(t_crc, 0x44);

	//memset(g_sBuf,0xFF,100*1024);
	memset(g_sBuf,0xFF,MAX_LEN);

	size_t len;
	size_t converted;

	if (argc > 1)
	{
		len = wcslen(argv[1]) + 1;
		converted = 0;
		pSourceName=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, pSourceName, len, argv[1], _TRUNCATE);
	}else
	{
		pSourceName="NikonStm3.bin";
	}

	//if((pfileRes = fopen("NikonStm32.bin","rb")) == NULL)
	if (fopen_s(&pfileRes,pSourceName,"rb") != NULL)
	{
		printf("open nikon source error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	g_fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(g_sBuf,1,g_fileLen,pfileRes);

	//g_fileLen = g_fileLen+(16-(g_fileLen%16));
	//g_fileLen = (g_fileLen+16)&0xFFFFFFF0;
		 
	//ClearFile();

	//for (int i=0;i<g_fileLen;i+=16)
	//for (int i=0;i<100*1024;i+=16)
	if (argc > 3)
	{
		len = wcslen(argv[3]) + 1;
		converted = 0;
		decKey = (char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, decKey, len, argv[3], _TRUNCATE);
	}else
	{
		decKey="";
	}

	for(unsigned long i=0;i<((g_fileLen&0xfffffff0)+16);i+=16)
	{
		fEnc.Boot_Encrypt(g_sBuf+i,decKey,g_sBuf+i,16);
	}


	if (argc > 2)
	{
		len = wcslen(argv[2]) + 1;
		converted = 0;
		pDestName = (char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, pDestName, len, argv[2], _TRUNCATE);
	}else
	{
		pSourceName = "firmware.bin";
	}
	//if ((pfileRst = fopen("firmware.bin","wb+")) == NULL)
	if (fopen_s(&pfileRst,pDestName,"wb+")!=NULL)
	{
		printf("firmware.bin error\r\n");
		goto Err;
	}

	//fwrite(g_sBuf,g_fileLen,1,pfileRst);
	//fwrite(g_sBuf,g_fileLen,1,pfileRst);
	//fwrite(g_sBuf,100*1024,1,pfileRst);
	fwrite(g_sBuf,(g_fileLen&0xfffffff0)+16,1,pfileRst);

	fclose(pfileRes);

	//fclose(pfileRst);

	//fclose(pfileRear);

	//while(1);
	printf("ok");
	Sleep(300);

	return 0;

Err:
	//while(1);
	Sleep(1000);

	return 1;

}


