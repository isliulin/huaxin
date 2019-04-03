// CC2531CODE.cpp : 定义控制台应用程序的入口点。
//
#include <windows.h>
#include "stdafx.h"
#include "enc.h"

//char *g_decKey = "WorkForYourDream";
//char *g_decKey = "MakeTomorrowBest";


#define uint8 unsigned char
#define uint32 unsigned int
#define uint16 unsigned short

#define PREAMBLE_OFFSET            0x8C
#define PREAMBLE_LENSET			(0X8C+2)

#define HAL_OAD_RC_START           0x00/*128*1024*///0x1000//0x0800//0x1000//0x2000//0x2000//0x0800//0x5000//0x0800
#define HAL_OAD_CRC_ADDR           0x88

#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

//#define STM_FW_LEN 56*1024

//char g_sBuf[256*1024];
//char g_sBuf[512*1024];
//char g_sBuf[100*1024];
//char g_sBuf[STM_FW_LEN];
char *g_sBuf;
unsigned long g_fileLen;


int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfileRes;
	FILE *pfileRst;
	char *srcPath;
	char *decKey;
	char *destPath;
	//FILE *pfileRear;
	EncMethod fEnc;
	//preamble_t preamble;

	//char sBuf[256*1024];

	size_t len;
	size_t converted;

	if (argc > 1)
	{
		len = wcslen(argv[1]) + 1;
		converted = 0;
		srcPath=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, srcPath, len, argv[1], _TRUNCATE);
	}

	if (argc > 2)
	{
		len = wcslen(argv[2]) + 1;
		converted = 0;
		decKey=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, decKey, len, argv[2], _TRUNCATE);
	}

	if (argc > 3)
	{
		len = wcslen(argv[3]) + 1;
		converted = 0;
		destPath=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, destPath, len, argv[3], _TRUNCATE);
	}
	

	if ((!srcPath) || (!decKey))
	{
		printf("no path or decKey");
		goto Err;
	}else
	{
		printf("path is %s, Decrytp key is %s\n",srcPath, decKey);
		Sleep(1000);
	}

	//(char*)preamble = (g_sBuf+PREAMBLE_OFFSET);
	//memcpy((char*)&preamble,g_sBuf+PREAMBLE_OFFSET,sizeof(preamble_t));

	//uint16 t_crc = 0x321d;
	//t_crc = runPoly(t_crc, 0x44);

	//memset(g_sBuf,0xFF,STM_FW_LEN);

	//if((pfileRes = fopen_s(&pfileRes,"CanStm32.bin","rb")) == NULL)
	if(fopen_s(&pfileRes,srcPath,"rb"))
	{
		printf("open source path error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	g_fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	g_sBuf = new char[(g_fileLen&~0xff) + 256];

	memset(g_sBuf,0xFF,(g_fileLen&~0xff) + 256);

	fread(g_sBuf,1,g_fileLen,pfileRes);

	//g_fileLen = g_fileLen+(16-(g_fileLen%16));
	//g_fileLen = (g_fileLen+16)&0xFFFFFFF0;
		 
	//ClearFile();

	//for (int i=0;i<g_fileLen;i+=16)
	//for (int i=0;i<STM_FW_LEN;i+=16)
	for (unsigned long i = 0; i < (g_fileLen&~0xff) + 256; i+=16)
	{
		fEnc.Boot_Encrypt(g_sBuf+i,decKey,g_sBuf+i,16);
	}

	//if ((pfileRst = fopen("firmware.bin","wb+")) == NULL)
	if (!destPath)
	{
		if (fopen_s(&pfileRst,"firmware.bin","wb+"))
		{
			printf("firmware.bin error\r\n");
			goto Err;
		}
	}else
	{
		if (fopen_s(&pfileRst,destPath,"wb+"))
		{
			printf("dest path error\r\n");
			goto Err;
		}
	}
	

	//fwrite(g_sBuf,g_fileLen,1,pfileRst);
	//fwrite(g_sBuf,g_fileLen,1,pfileRst);
	//fwrite(g_sBuf,STM_FW_LEN,1,pfileRst);
	fwrite(g_sBuf,(g_fileLen&~0xff) + 256,1,pfileRst);

	fclose(pfileRes);

	//fclose(pfileRst);

	//fclose(pfileRear);

	free(srcPath); 
	free(decKey);
	free(destPath);

	delete [] g_sBuf;

	//while(1);
	printf("ok");
	Sleep(300);

	return 0;

Err:
	//while(1);
	Sleep(2000);
	if (g_sBuf != NULL)
	{
		delete [] g_sBuf;
	}

	return 1;

}


