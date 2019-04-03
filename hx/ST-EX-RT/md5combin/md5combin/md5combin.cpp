// md5combin.cpp : Defines the entry point for the console application.
//
#include <windows.h>
#include "stdafx.h"
#include <string.h> 
#include "TriDesEnc.h"
#include "MD5.h"
//#define FW_STM32_LEN (1024*100)				//stm32 fw 100K
#define FW_STM32_LEN (1024*106)
#define FW_CC2530_LEN (1024*(256-8))//(104192)
#define FW_TAIL_LEN	  (1024*512)
#define ERROR -1

#define SPEEDLIGHT_600EX   0
#define ST_EX_RT           1
#define SPEEDLIGHT_910N    2
//#define DEST_DEVICE        SPEEDLIGHT_600EX
//#define DEST_DEVICE        2
#define DEST_DEVICE SPEEDLIGHT_910N

#if (DEST_DEVICE==ST_EX_RT)
#define FW_FILE_NAME "HxStex-1.fmw" 
#elif (DEST_DEVICE==SPEEDLIGHT_600EX)
#define FW_FILE_NAME  "SpeedLight600ex.fmw"
#elif (DEST_DEVICE==SPEEDLIGHT_910N)
#define FW_FILE_NAME "SpeedLight_N.fmw"
#else
#error "no device indicate"
#endif

char *g_desKey = "XiaobaoApple";

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfileRes;
	FILE *pfileRst;
	MD5_ENC md5_enc;
	TriDesEnc triDesEnc;

	int fileLen;
	char rBuf[FW_STM32_LEN+FW_CC2530_LEN+FW_TAIL_LEN];
	unsigned char rMd5[16];
	memset(rBuf,0xFF,FW_STM32_LEN+FW_CC2530_LEN+FW_TAIL_LEN);
	if((pfileRes = fopen("firmware.bin","rb")) == NULL)
	{
		printf("open firmware.bin error\r\n");
		goto Err;
		//return ERROR;
	}
	fseek(pfileRes,0,SEEK_END);
	fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(rBuf,1,fileLen,pfileRes);

	fclose(pfileRes);

	if((pfileRes = fopen("cc2530.bin","rb")) == NULL)
	{
		printf("open cc2530.bin error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(rBuf+FW_STM32_LEN,1,fileLen,pfileRes);

	fclose(pfileRes);

	for(int i=0;i<FW_STM32_LEN+FW_CC2530_LEN;i+=8)
	{
		triDesEnc.DES_EncryptEx(rBuf+i,g_desKey,rBuf+i);
	}

	if((pfileRes = fopen("cc2530_rear.jpg","rb")) == NULL)
	{
		printf("open cc2530_rear.jpg error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(rBuf+FW_STM32_LEN+FW_CC2530_LEN,1,fileLen,pfileRes);

	if ((pfileRst = fopen(FW_FILE_NAME,"wb")) == NULL)
	{
		printf("open HxStex-1.fmw error\r\n");
		goto Err;
		//return ERROR;
	}

	fwrite(rBuf,FW_STM32_LEN+FW_CC2530_LEN+FW_TAIL_LEN,1,pfileRst);

	fclose(pfileRst);

	if(md5_enc.MD5File(FW_FILE_NAME,rMd5))
	{
		printf("MD5 ERROR\r\n");
		goto Err;
		//return ERROR;
	}

	if ((pfileRst = fopen(FW_FILE_NAME,"ab")) == NULL)
	{
		printf("open HxStex-1.fmw error\r\n");
		goto Err;
		//return ERROR;
	}

	//fseek(pfileRst,0,SEEK_END);

	fwrite(rMd5,16,1,pfileRst);

	fclose(pfileRst);

	printf("OK");

	//while (1);
	Sleep(300);

	return 0;

Err:
	while(1);
}

