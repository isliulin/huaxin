// CC2531CODE.cpp : 定义控制台应用程序的入口点。
//
#include <windows.h>
#include "stdafx.h"
#include "enc.h"

char *g_decKey = "WorkForYourDream";

#define uint8 unsigned char
#define uint32 unsigned int
#define uint16 unsigned short

#define PREAMBLE_OFFSET            0x8C
#define PREAMBLE_LENSET			(0X8C+2)

#define HAL_OAD_RC_START           0x00/*128*1024*///0x1000//0x0800//0x1000//0x2000//0x2000//0x0800//0x5000//0x0800
#define HAL_OAD_CRC_ADDR           0x88

#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

//char g_sBuf[256*1024];
char g_sBuf[512*1024];
unsigned long g_fileLen;

typedef struct {
	uint8 magic[2];
	uint32 len;
	uint16  vers;
	uint16  manu;
	uint16  prod;
} preamble_t;

typedef enum {
	HAL_OAD_RC,  /* Run code / active image.          */
	HAL_OAD_DL   /* Downloaded code to be activated later. */
} image_t;

static uint16 runPoly(uint16 crc, uint8 val)
{
	const uint16 poly = 0x1021;
	uint8 cnt;

	for (cnt = 0; cnt < 8; cnt++, val <<= 1)
	{
		uint8 msb = (crc & 0x8000) ? 1 : 0;

		crc <<= 1;
		if (val & 0x80)  crc |= 0x0001;
		if (msb)         crc ^= poly;
	}

	return crc;
}

void HalOADRead(uint32 oset, uint8 *pBuf, uint16 len, image_t type)
{
	oset += HAL_OAD_RC_START;
	memcpy(pBuf,g_sBuf+oset,len);
	//HalFlashRead(oset / HAL_FLASH_PAGE_SIZE, oset % HAL_FLASH_PAGE_SIZE, pBuf, len);
}

static uint16 crcCalc()
{
	//preamble_t preamble;
	uint32 oset;
	uint16 crc = 0;

	//HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);

	// Run the CRC calculation over the active body of code.
	for (oset = 0; oset < g_fileLen; oset++)
	{
		if ((oset < HAL_OAD_CRC_OSET) || (oset >= HAL_OAD_CRC_OSET+4))
		{
			uint8 buf;
			HalOADRead(oset, &buf, 1, HAL_OAD_RC);
			crc = runPoly(crc, buf);
			
		}
	}

	crc = runPoly(crc, 0);
	crc = runPoly(crc, 0);

	return crc;
}

void ClearFile()
{
	uint32 index;
	for (index=g_fileLen-1;index>0;index--)
	{
		if (g_sBuf[index]!=(char)0xff)
		{
			g_fileLen = index+1;
			return;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfileRes;
	FILE *pfileRst;
	//FILE *pfileRear;
	EncMethod fEnc;
	//preamble_t preamble;

	//char sBuf[256*1024];
	uint16 crc[2];

	//(char*)preamble = (g_sBuf+PREAMBLE_OFFSET);
	//memcpy((char*)&preamble,g_sBuf+PREAMBLE_OFFSET,sizeof(preamble_t));

	//uint16 t_crc = 0x321d;
	//t_crc = runPoly(t_crc, 0x44);


	if((pfileRes = fopen("SampleApp.bin","rb")) == NULL)
	{
		printf("open SampleApp.bin error\r\n");
		goto Err;
		//return ERROR;
	}

	fseek(pfileRes,0,SEEK_END);
	g_fileLen = ftell(pfileRes); 
	rewind(pfileRes);

	fread(g_sBuf,1,g_fileLen,pfileRes);
	//g_fileLen += (g_fileLen%16);
	//g_fileLen = (g_fileLen+16)&0xFFFFFFF0;
	ClearFile();
	g_fileLen = (g_fileLen+16)&0xFFFFFFF0;

//	memcpy((char*)&preamble,g_sBuf+PREAMBLE_OFFSET,sizeof(preamble_t));
	memcpy(g_sBuf+PREAMBLE_LENSET,(char*)&g_fileLen,sizeof(int));

	crc[1] = crcCalc();


	g_sBuf[HAL_OAD_CRC_ADDR+2] = crc[1]&0xFF;
	g_sBuf[HAL_OAD_CRC_ADDR+3] = (crc[1]>>8)&0xFF;

	for (int i=0;i<g_fileLen;i+=16)
	{
		fEnc.Boot_Encrypt(g_sBuf+i,g_decKey,g_sBuf+i,16);
	}

	if ((pfileRst = fopen("cc2530.bin","wb+")) == NULL)
	{
		printf("open cc2530.bin error\r\n");
		goto Err;
	}

	fwrite(g_sBuf,g_fileLen,1,pfileRst);
	//fwrite(g_sBuf,512*1024,1,pfileRst);

	fclose(pfileRes);

	fclose(pfileRst);

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


