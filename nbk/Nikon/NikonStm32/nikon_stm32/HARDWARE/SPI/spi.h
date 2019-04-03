#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

#define SPI_TIMEOUT		10
#define VERI_DATA_LEN 21

#define WLM_SETARR_OPPARA(x,y) (0x2a-((x*3+y)*2-6))

//#define SPI_MISO_AUX	PAout(4)	
//#define TEST_SIGNAL		PAout(8)


#define RCVDAT_LOCATION_APERTURE 0x05
#define RCVDAT_LOCATION_ISO 0x03
#define RCVDAT_LOCATION_ISO_1 12
#define RCVDAT_LOCATION_ISO_2 13
#define RCVDAT_LOCATION_FOCLEN 0x06
#define RCVDAT_LOCATION_MODE 0x01
#define RCVDAT_LOCATION_BKMOD 0x00
#define RCVDAT_LOCATION_EXPOSE 0x01
#define RCVDAT_LOCATION_CAMERACMP 0x07
#define RCVDAT_LOCATION_SHUTTER	0x04

#define COM_DAT_MODE	       1
//#define COM_DAT_AFON		3
#define COM_DAT_STATUE        3
#define COM_DAT_MASTPAR	5
#define COM_DAT_RPTFRQ        10
#define COM_DAT_RPTTIMES    11
#define COM_DAT_MSL_A		16
#define COM_DAT_MSL_BC		17
#define COM_DAT_SLPARA_A 	18
#define COM_DAT_SLPARA_B 	19
#define COM_DAT_SLPARA_C 	20
#define COM_DAT_REAR            21
						  	    													  
void SPIx_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void EXTIx_Init(void);
void SPIx_Disable(void);
void WlmRatioVary(void);
#endif




