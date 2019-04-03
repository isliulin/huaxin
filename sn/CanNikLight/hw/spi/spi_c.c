#include "spi.h"
#include"global.h"
#include <stdarg.h> 
#include "..\flash\flash.h"
#include "..\timer\timer.h"
#include "..\a7105\a7105.h"
#include "..\uart\uart.h"

extern u8 g_spiRcvFlag;
extern u8 g_spiDat;
u8 g_sckIntFlag = 0;

//extern u8 g_tmCtrl;

static void SckIntHandle(void);
//void Timer4Start(u8 arr);
void SpiRearUp(u8 tks);
void SpiTimerOut(u16 tks);
void GetParameter(void);
void CodeAnalysis(u8 rcvCode);
void SetSpiTrans(void);

u8 g_spiRcvTotal = 0;
u8 g_spiSendIndex=0;
u8 g_spiRcvIndex=0;//SPI_Receive_Index;
u8 g_spiRcvData[MAX_BUFFER_SIZE] = {0};
u8 g_spiSendData[SEND_MAX_SIZE]={0};
u8 g_cmrPressed=0;
u8 g_isFullFrame=0;
u8 g_getTtlFlashCode=0;
u8 ttl_expCodeRsv=0;
u8 g_AfIndex=0;
u8 g_canCmrLink = 0;
FlaParameter flaPara={0};

const u8 feb_table[10] = {
0x00,0x03,0x05,0x08,0x0b,0x0d,0x10,0x13,0x15,0x18
};

const u8 compensation_table[37]={
0x30,0x2d,0x2b,
0x28,0x25,0x23,
0x20,0x1d,0x1b,
0x18,0x15,0x13,
0x10,0x0d,0x0b,
0x08,0x05,0x03,
0x00,0xfd,0xfb,
0xf8,0xf5,0xf3,
0xf0,0xed,0xeb,
0xe8,0xe5,0xe3,
0xe0,0xcd,0xcb,
0xc8,0xc5,0xc3,
0xc0,
};

const u8 cfnCntMask[] = {0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x00,};

const u16 CannonApertVal[] = {		6,	8,	9,	10,	11,	12,
										14,	16,	18,	20,	22,	25,	
										28,	32,	35,	40,	45,	50,	
										56,	63,	71,	80,	90,	100,
										110, 130, 140,160, 180, 200,
										220, 250, 290,320, 360, 400,
										450, 510	};

#define DisableSckInt() \
{\
	PB_CR2_C25 = 0;\
}

#define EnableSckInt() \
{\
	PB_CR2_C25 = 1;\
}

#define ShutSpiClock() \
{\
	CLK_PCKENR1 &=  ~( 1 << 4 ); \
}

#define OpenSpiClock() \
{\
	CLK_PCKENR1 |=  ( 1 << 4 );  \
}

void InitExtIntPort(void)
{
    CPU_CCR=0X28;  //等级3
    EXTI_CONF1=0x00;//PB6->EXTI6
    //EXTI_CR2=0x08;//下降沿触发
    PB_DDR_DDR5=0;//PB6输入
    //PB_CR1_C15=1;//PB6上拉
   //PB_CR2_C25=1;//PB6中断使能
}

void SetSckInt(u8 tig)
{
	// here add asm("sim") and asm("rim") can not run the speedlight correctly
	// the reason is unknown
	//2017/08/12 add asm("sim") and asm("rim") can work, why?
	//asm("sim");
	PB_CR2_C25 = 0;
	if ( INT_EDGE_R == tig )
	{
		PB_CR1_C15 = 0;      // pull down
		EXTI_CR2 &= 0xF3; 
		EXTI_CR2 |= 0x04;
	}else 
	{
		PB_CR1_C15 = 1;
		EXTI_CR2 &= 0xF3; 
		//EXTI_CR2 = 0x08;             // cause error
                EXTI_CR2 |= 0x08;
	}
	PB_CR2_C25 = 1;
	//EXTI_SR1|=0x20; //清除中断标志
	//asm("rim");
}



void SPI1_Init(void) 
{      
    CLK_PCKENR1 |=  ( 1 << 4 ); 
    //PE_DDR = 0;//PE5输入,上拉 NSS 
    PB_DDR &= 0x9F;//5(SCK).6(mosi)输入 7（miso）输出 
    PB_DDR |= 0x80;// 5.6输入 7输出 
    //PB_DDR &= ~0x80;
    PB_CR1 &= 0x9F; 
    PB_CR1 |= 0x80; 
    //PB_CR1 |= 0xA0; 
    //PB_ODR |= 0x80;
    SPI1_CR1 = 0x03;//从模式，先发高位，空闲是CLK为高，采样第一个开始，时钟/8 
    SPI1_CR2 = 0x02;//软件从模式 
    //SPI1_ICR = 0X40;//接收完成中断 
    //SPI1_CR1 |= 0x40; 
/*
	if (SCK_IN)
	{
		SetSpiTrans();
		g_cmrLinked = CMR_LINK_CAN;
	}else
	{
		SetSckInt(INT_EDGE_R);
	}*/
} 

void SetSpiTrans(void)
{

	SPI_MISO_AUX ( 1 );
	COM_AUX = 1;
	 
	SPI1_CR1 |= 0x40;                // SPI Enable
	SPI1_ICR = 0x40;                  // spi receive int

	SPI1_DR = 0x00;

	//g_comStatue = COM_STATUE_START;
	g_canComStatue = COM_STATUE_START;

	SetSckInt(INT_EDGE_F);
}

void SetSpioOn()
{
	//OpenSpiClock();
	SET_SPIO_ON()
}

static void StartSpiTrans(void)
{
        if ( SCK_IN )
	{
		return;
	}
	SPI_MISO_AUX ( 0 ) ;
	DisableSckInt();
	SpiTimerOut(250);
}

void CameraCfnSet(u8 idx, u8 sel)
{
	if (0x01 == idx)
	{
		g_cfnOption[0] = sel;
	}else if (0x03 == idx)
	{
		g_cfnOption[1] = sel;
	}else if (0x04 == idx)
	{
		g_cfnOption[2] = sel;
	}else if (0x08 == idx)
	{
		g_cfnOption[3] = sel;
	}else if (0x0e == idx)
	{
		g_cfnOption[4] = sel;
	}
}


void FlashHandle(void)
{
	//unsigned int tmp;
	static char info_get=0;

	if (info_get > 0)
	{		
		CameraCfnSet( (g_spiRcvData[1]>>3),g_spiRcvData[1]&0x07);
		g_spiSendData[g_spiSendIndex++] = 0x4C;
		info_get = 0;
		//SpiRearUp(4000);
                SpiRearUp(250);
		return;
	}

	
	switch(g_spiRcvData[0])
	{
		case 0xb4:
			switch(g_spiRcvData[1])
			{
				case 0x23:
				//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_PRE);
				//SC4MD1 &= 0xCF;		//suspend clock syn transfer
				SET_SPIO_OFF( );
				//FLASH_OUT = 0;
				SPI_MISO_AUX(0);
				//g_comStatue= COM_STATUE_PREFLASH;
				g_canComStatue = COM_STATUE_PREFLASH;
				//g_mstPrefla = 1;
				SpiRearUp(100);
				break;

				case 0x3d:
					SET_SPIO_OFF( );
					SPI_MISO_AUX(0);	
					//g_comStatue = COM_STATUE_REALFLASH;
					g_canComStatue = COM_STATUE_REALFLASH;
					SpiRearUp(100);
				break;

				default:
					SpiRearUp(20);
				break;
			}

		break;

		case 0xA5:				//this is in order to compliment with 700d
		//in the case of 600ex&700d, there if no (0xA5 0x6D 0x01) at RF mode, at present at light cmd mode
		if((g_spiRcvData[1]&0xf0)>=0x60)				//
		{
			//g_spiSendData[g_spiSendIndex++] = 0x97;
			g_spiSendData[g_spiSendIndex++] = 0x96;
			if((g_spiRcvData[1]&0x0f)==0x0d)   	//this determined the flash code 0xb4 1d b4 3d
			{
				menuPara.para_a5 = 0x01;
			}else{ 
				menuPara.para_a5 = 0x00;
			}
			g_vCmr = VER_CAMERA_NEW;
		}else
		{
			g_vCmr = VER_CAMERA_OLD;
		}
		//SPI_MISO_AUX=1;	
		SpiRearUp(20);
		break;

		case 0xA1:
			//if(0x5227==MAKEWORD(g_spiRcvData[2],g_spiRcvData[1]))
			if(0x52==g_spiRcvData[1])
			{
				g_spiSendData[g_spiSendIndex++] = 0x4C;
				g_spiSendData[g_spiSendIndex++] = 0x4C;
				info_get = 1;
				//pSnrSet[(g_spiRcvData[2]>>3)&0x1F] = (g_spiRcvData[3]&0x07);
			}
			SpiRearUp(20);
		break;

		case 0xB7:
		if(g_spiRcvData[1]>0)
		{
			if(!g_cmrPressed)
			{
				g_SettingIndex = 0;
			}
			g_cmrPressed = 1;
			
		}else
		{
			g_cmrPressed = 0;
			/*
			SET_SPIO_OFF();                                                      // shut down spi
			g_canComStatue = COM_STATUE_STOP;                   // aviod the sck int
			SpiTimerOut(500);
			SPI_MISO_AUX ( 1 ) ;*/
			//TEST_PIN ^= 1;
			flaPara.cmrCmp1 = 0;
			flaPara.cmrCmp2 = 0;
			flaPara.cmrCmpCode = 0;
			
		}
		g_isFullFrame = 1;
		gWSleepCounter = 0;
			//g_cmrPressed = 1;
		//if(flaPara.apert!=g_spiRcvData[1])
		if(menuPara.cApert!=g_spiRcvData[1])
		{
			//flaPara.apert = g_spiRcvData[1];
			
			menuPara.cApert = g_spiRcvData[1];	
			//tmp1 = menuPara.cApert>>4;
			//tmp2 = menuPara.cApert&0x0F;
			
			flaPara.apert = CannonApertVal[(menuPara.cApert>>4)*6+((menuPara.cApert&0x0F)+2)/3];
			
			//SET_UPDATE_EVT(DSP_UPDATE_LINK);
			//SPI_MISO_AUX=1;
			//g_dspUpdateFlag|=DSP_UPDATE_LINK;
			//SETBIT(bitTerminate,BIT_bitTerminate_APERT);
		}
		SpiRearUp(20);
		break;

		default:
			SpiRearUp(20);
		break;
	}
}

void SpiRearUp(u8 tks)
{
	//u8 rdReg;
	//Timer4Pscr(0x04);              // 1m
	//Timer4Stop();
	//Timer4Pscr(0x04);  
	Timer4Start(tks/8);
	g_tmHandleFlag = TM_HANDLE_REAR;
	//rdReg = TIM4_PSCR;
	//rdReg = TIM4_CNTR;
	
	//SPI_MISO_AUX ( 1 );	
}

void SpiTimerOut(u16 tks)
{
	//Timer4Stop();
	//Timer4Pscr(0x07);              // 1m
	Timer4Start(tks/8);
	g_tmHandleFlag = TM_HANDLE_SHUT;
}

#pragma vector= SPI_RXNE_vector   
 __interrupt void SPI_RCN( void ) 
{    
/*    g_spiRcvFlag = 1; 
    g_spiDat = SPI1_DR; 
    PB_CR2_C25 = 1;
*/
	u8 datRcv;

	if (SPI1_SR & 0x01)
	{
		SET_SPIO_OFF();
		PUSH_MISO();
		
		Timer4Stop();
		
		datRcv = SPI1_DR;

		g_spiRcvTotal ++;
		if (g_spiSendIndex>0)
		{
			if (g_spiRcvIndex < MAX_BUFFER_SIZE)
			{
				g_spiRcvData[g_spiRcvIndex++] = datRcv;
			}else
			{
				g_spiRcvIndex = 0;
				g_spiSendIndex = 0;
				goto spi_exit;
			}
			SPI1_DR = g_spiSendData[--g_spiSendIndex];
			if(!g_spiSendIndex)
			{
				FlashHandle();
				g_spiRcvIndex = 0;
			}else
			{
				//SPI_MISO_AUX=1;	
				//SpiRearUp(15);
				SpiRearUp(12);
			}
		}else
		{
			//get the last parameter
#if 1
			GetParameter();			//extract infomation
			if (g_spiRcvIndex < MAX_BUFFER_SIZE)
			{
				g_spiRcvData[g_spiRcvIndex++]=datRcv;
			}else
			{
				g_spiRcvIndex = 0;
				g_spiSendIndex = 0;
				goto spi_exit;
			}
			
			CodeAnalysis(datRcv);	//load spi data
			if (g_spiSendIndex > 0)
			{
				SPI1_DR = g_spiSendData[--g_spiSendIndex]; 	//send a byte	 
			}
			//SPI_MISO_AUX=1;	
			
                        //SpiRearUp(100);
                        SpiRearUp(60);
#endif

	/*
			//g_spiRcvData[g_spiRcvIndex++]=datRcv;
			//SPI1_DR = g_spiSendData[--g_spiSendIndex];
			SPI1_DR = 0x8e;
			//SPI_MISO_AUX ( 1 );	

			SpiRearUp(100);
	*/
		}

		//SpiRearUp(100);

spi_exit:
		
		EnableSckInt();

	}
	
} 

#pragma vector= EXTI5_vector
__interrupt void EXTI5_IRQHandler(void)
{
   
    if(EXTI_SR1&0x20) //是否外部中断5触发
    {
#ifdef SLEEP_DBG
	//printf("sck int\r\n");
#endif
        //PD_ODR_ODR4 ^= 1;
        SPI_MISO_AUX ( 0 ) ;
        EXTI_SR1|=0x20; //清除中断标志
        //PB_CR2_C25 = 0;  // 禁止中断
        //PB_DDR_DDR7 = 1;
        SckIntHandle();
    }
}

#if 0
void CanSckIntEventLoop(void)
{
	if (g_sckIntFlag > 0)
	{
		g_sckIntFlag = 0;
		//switch ( g_comStatue )
		switch (g_canComStatue)
		{
			case COM_STATUE_START:
				StartSpiTrans();
			break;

			case COM_STATUE_IDLE:
				if (SCK_IN )
				{
					g_cmrLinked = CMR_LINK_CAN;
					SetSpiTrans();
				}
			break;

			case COM_STATUE_PREFLASH:
				g_flashEnable = 2;            // preflash
			break;

			case COM_STATUE_REALFLASH:
				if (!FLASH_IN)				// rear sync
				{
					g_flashEnable = 1;
				}
			break;

			case COM_STATUE_SLEEP:
				// WARNING: add this function cause the delay of enter the interrupt handle belonged.
				// the communicaion is interruped
				SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);
				g_cmrLinked = CMR_LINK_CAN;
				SetSpiTrans();
			break;
			
		}
	}

}
#endif

static void SckIntHandle(void)
{
	//TEST_PIN ^= 1;
/*
	SPI_MISO_AUX ( 0 ) ;
	DisableSckInt();
	g_sckIntFlag = 1;*/

	//if (COM_STATUE_START == g_comStatue)
/*
	if (COM_STATUE_START == g_canComStatue)
	{
		A7105_SCK ^= 1;
		StartSpiTrans();
	}else if (COM_STATUE_IDLE == g_canComStatue)
	{
		if (SCK_IN )
		{
			g_cmrLinked = CMR_LINK_CAN;
			SetSpiTrans();
		}
	}else{
		g_sckIntFlag = 1;
	}
*/
#if 1	
/*
	if (g_sleepTag == 1)
	{
		printf("SCK before sleep\r\n");
		return;
	}
*/
#if 0
	if (COM_STATUE_START == g_canComStatue)
	{
		StartSpiTrans();
	}else if (COM_STATUE_IDLE == g_canComStatue)
	{
		if (SCK_IN )
		{
			g_cmrLinked = CMR_LINK_CAN;
			SetSpiTrans();
			g_sleepTag = SLEEP_TAG_CAN;
		}
	}
#endif
#if 1	
	switch ( g_canComStatue )
	{
		//case COM_STATUE_SLEEP:
			///SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);
			//g_cmrLinked = CMR_LINK_CAN;
		case COM_STATUE_START:
			StartSpiTrans();
		break;

		case COM_STATUE_IDLE:
			if (SCK_IN )
			{
				g_cmrLinked = CMR_LINK_CAN;
				SetSpiTrans();
				g_sleepTag = SLEEP_TAG_CAN;
			}
		break;

		case COM_STATUE_PREFLASH:
			g_flashEnable = 2;            // preflash
		break;

		case COM_STATUE_REALFLASH:
			if (!FLASH_IN)				// rear sync
			{
				g_flashEnable = 1;
			}
		break;
/*
		case COM_STATUE_SLEEP:
			g_sleepTag = SLEEP_TAG_CAN;
		break;
*/

		case COM_STATUE_STOP:
			SPI_MISO_AUX(1);
		break;

/*		default:
			SetSpiTrans();
		break;*/

/*
		case COM_STATUE_SLEEP:
			// WARNING: add this function cause the delay of enter the interrupt handle belonged.
			// the communicaion is interruped
			//SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);
			g_cmrLinked = CMR_LINK_CAN;
			SetSpiTrans();
		break;*/
		
	}
#endif
#endif
}

void WaitCmrHandle(void)
{
	ShutSpiClock();
	SET_SPIO_OFF( );
	//g_comStatue = COM_STATUE_IDLE;
	g_canComStatue = COM_STATUE_IDLE;
	g_nikComStatue = COM_STATUE_IDLE;
	g_spiRcvTotal = 0;
	/*20180124*/
	g_spiSendIndex = 0;
	g_spiRcvIndex = 0;
	g_cmrLinked = CMR_LINK_NULL;
	g_canCmrLink = 0;
	cmrPara.cls = 0;
	//SetSckInt(INT_EDGE_R);
	//EnableSckInt();
	OpenSpiClock();
	//CLK_PCKENR1 |=  ( 1 << 4 ); 
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
#ifdef CAN_COM_DBG
       printf("can unlinked\r\n");
#endif
}

void GetParameter(void)
{
//	int paraIndex=0;
	//u8 search_index;
	//u8 tmp;
//	signed char cvtTmp;
	//static unsigned char modCodeRsv;
	static u8 motoRsv;

	switch(g_spiRcvData[0])
	{
		case 0xa6:
			//Parameter_Array[PARA_SRD]=g_spiRcvData[4];			//get flash surranding parameter
			//if(menuPara.feb!=g_spiRcvData[4])
			if (feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2] != g_spiRcvData[4])
			{
				//menuPara.feb = g_spiRcvData[4];
				flaPara.ttl_FEB1 = H_COV(g_spiRcvData[4]+1);
				flaPara.ttl_FEB2 = L_COV(g_spiRcvData[4]+1);
				flaPara.FEB_count = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}

			//if(menuPara.mode!=g_spiRcvData[3])
			if(((g_spiRcvData[3]&0x07)!=mainPara.workMode)
				||(((g_spiRcvData[3]>>3)&0x03)!=mainPara.synMode))
			{
				//menuPara.mode = g_spiRcvData[3];
				if ((g_spiRcvData[3]&0x07) <= WORKINGMODE_MULTI)
				{
					mainPara.workMode = (g_spiRcvData[3]&0x07);
					mainPara.synMode = (g_spiRcvData[3]>>3)&0x03;
					/*20170828 No fp support*/
					if (SYNC_MODE_FP == mainPara.synMode)
					{
						mainPara.synMode = SYNC_MODE_FRON;
					}
				}

				//menuPara.syn2 = (0x5d|((SYNC_MODE_REAR==mainPara.synMode)<<1));
				//menuPara.syn3 = (0x53|((SYNC_MODE_FP==mainPara.synMode)<<2));
				//Parameter_Array[PARA_SYNC2]=(0x5d|((SYNC_MODE_REAR==mainPara.synMode)<<1));
				//Parameter_Array[PARA_SYNC3]=(0x53|((SYNC_MODE_FP==mainPara.synMode)<<2));

				SET_UPDATE_EVT(DSP_UPDATE_MODE);//g_dspUpdateFlag |= DSP_UPDATE_MODE;			//update the display

			}
			//modCodeRsv = g_spiRcvData[3];
		break;

		case 0xa2:									 //focus lenghth
		//case 0xC8:
			if(motoRsv!=g_spiRcvData[3])
			{
				motoRsv = g_spiRcvData[3];
				//g_dspUpdateFlag |= DSP_UPDATE_MOTO;
				SET_UPDATE_EVT(DSP_UPDATE_MOTO);
				switch(g_spiRcvData[3])
				{
					case 0x01:				  //auto
	//				Parameter_Array[PARA_SYNC2]=0x7d;
	//				main_parameter.moto_location= 0x00;
					//main_parameter.moto_mode = MOTO_MODE_AUTO;
					*motoSet.mode = MOTO_MODE_AUTO;
					//Parameter_Array[PARA_FOCIND]=0x01;
					menuPara.motoCod = 0x01;
					//Parameter_Array[PARA_FOCLEN]=0x50;
					//Parameter_Array[PARA_FOCLEN]=*motoSet.dest_location;//main_parameter.moto_location;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x11:				 //0x18
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x18;//main_parameter.moto_location= 0x18;
					motoSet.destRsv = 0x18;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x11;
					menuPara.motoCod = 0x11;
					//Parameter_Array[PARA_FOCLEN]=0x18;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x12:				 //0x1c
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x1C;//main_parameter.moto_location= 0x1c;
					motoSet.destRsv = 0x1C;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x12;
					menuPara.motoCod = 0x12;
					//Parameter_Array[PARA_FOCLEN]=0x1c;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x13:				//0x23
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x23;//main_parameter.moto_location= 0x23;
					motoSet.destRsv= 0x23;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x13;
					menuPara.motoCod = 0x13;
					//Parameter_Array[PARA_FOCLEN]=0x23;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x14:				//0x32
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x32;//main_parameter.moto_location= 0x32;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x14;
					menuPara.motoCod = 0x14;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x15:				//0x46
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x46;//main_parameter.moto_location= 0x46;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x15;
					menuPara.motoCod = 0x15;
					//Parameter_Array[PARA_FOCLEN]=0x46;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x16:			   //0x50
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x50;//main_parameter.moto_location= 0x50;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x16;
					menuPara.motoCod = 0x16;
					//Parameter_Array[PARA_FOCLEN]=0x50;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x17:			   //0x69
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x69;//main_parameter.moto_location= 0x69;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x17;
					menuPara.motoCod = 0x17;
					//Parameter_Array[PARA_FOCLEN]=0x69;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					default:
					motoSet.destRsv= 0x50;//main_parameter.moto_location= 0x50;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_AUTO;
					//Parameter_Array[PARA_FOCIND]=0x01;
					menuPara.motoCod = 0x01;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

				}
			}
		break;

		case 0xC8:
		switch(g_spiRcvData[3])
		{
			case 0x35:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 20;
				motoSet.focIndex = 0;
			break;

			case 0x05:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 24;
				motoSet.focIndex = 1;
			break;

			case 0x09:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 28;
				motoSet.focIndex = 2;
			break;

			case 0x0D:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 35;
				motoSet.focIndex = 3;
			break;

			case 0x11:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 50;
				motoSet.focIndex = 4;
			break;

			case 0x15:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 70;
				motoSet.focIndex = 5;
			break;

			case 0x19:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 80;
				motoSet.focIndex = 6;
			break;

			case 0x1D:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 105;
				motoSet.focIndex = 7;
			break;

			case 0x39:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 135;
				motoSet.focIndex = 8;
			break;

			case 0x45:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 200;
				motoSet.focIndex = 9;
			break;

			case 0x44:
			case 0x08:
				*motoSet.mode = MOTO_MODE_AUTO;
				//menuPara.motoLoc = 0;
				menuPara.motoLoc = *motoSet.dest_location;   //20180105
				//*motoSet.dest_location = 20;
			break;

			default:  // in case of invalid data 20180105
				*motoSet.mode = MOTO_MODE_AUTO;
				//menuPara.motoLoc = 0;
				menuPara.motoLoc = *motoSet.dest_location;    //20180105

			break;
		}
		//motoSet.focLen = g_spiRcvData[3];
		SET_UPDATE_EVT(DSP_UPDATE_MOTO);
		//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
		break;

		case 0xa5:
		{
			switch(g_spiRcvData[1]&0x0f)
			{
				case 0x0d:
				//case 0x2d:
				//case 0x6d:
				//Parameter_Array[PARA_A5]=0x01;
				menuPara.para_a5 = 0x01;
				//g_isFullFrame = 2;
				//SETBIT(bitTerminate,BIT_bitTerminate_A5FLAG);
				break;

				case 0x0c:
				//Parameter_Array[PARA_A5]=0x00;
				menuPara.para_a5 = 0x00;
				//g_isFullFrame = 2;
				break;

				default:
				break;
			/*	case 0x0c:
				Parameter_Array[PARA_A5]=0x00;
				break;*/
			}
		}
		break;
			
////////////////////////////////////////////////////////
		case 0xa7:
			//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
			//Parameter_Array[PARA_FREQ]=g_spiRcvData[4];
			//Parameter_Array[PARA_MOUT]=g_spiRcvData[5];
#ifdef _MULTIPLY_FLASH_
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				//Parameter_Array[PARA_MOUT]=g_spiRcvData[5];
				if((*multiSet.frequence !=g_spiRcvData[4])||(*multiSet.eTimes != g_spiRcvData[5])||
					(output_table[(7-(*multiSet.expose>>4))*3]!=g_spiRcvData[3]))//||(menuPara.WlmMul[WL_ARRARY_A]!=g_spiRcvData[3]))
				{
					*multiSet.frequence =g_spiRcvData[4] ;//Parameter_Array[PARA_FREQ];
					*multiSet.eTimes = g_spiRcvData[5];//Parameter_Array[PARA_MOUT];
					//menuPara.WlmMul[WL_ARRARY_A] = g_spiRcvData[3];
					mstPara->multi_exp = EXPH_COV(g_spiRcvData[3]&0x7F)<<4;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
				
			}else if(WORKINGMODE_MANU==mainPara.workMode)
#else 
			if(WORKINGMODE_MANU==mainPara.workMode)				
#endif
			{
				//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
				//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
				//if(WLM_OFF==*wlSet.m_on)
				//if(CMD_MODE_OFF==*g_cmdMode)
				{
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					//if(menuPara.otpCod[MASTER]!=g_spiRcvData[3])
					if ((output_table[(7-mstPara->output1)*3-mstPara->output2] != (g_spiRcvData[3]&0x7f))
						&& (g_spiRcvData[3] <= 0x38))   // 20180105  /* 通讯错误导致显示错乱*/
					{
						//menuPara.otpCod[MASTER] = g_spiRcvData[3];
						//mstPara->output1 =EXPH_COV(menuPara.otpCod[MASTER]&0x7F);//(7- H_COV(menuPara.otpCod[MASTER]));
						//mstPara->output2 = EXPL_COV(menuPara.otpCod[MASTER]&0x7F);//(2-L_COV(menuPara.otpCod[MASTER]));
						mstPara->output1 =EXPH_COV(g_spiRcvData[3]&0x7F);//(7- H_COV(menuPara.otpCod[MASTER]));
						mstPara->output2 = EXPL_COV(g_spiRcvData[3]&0x7F);//(2-L_COV(menuPara.otpCod[MASTER]));
						/*20180124*/
						if ((mstPara->output1 > 7) || (mstPara->output1 < 0)
							|| (mstPara->output2 < 0) ||(mstPara->output2 > 2))
						{
							mstPara->output2 = 0;
							mstPara->output1 = 0;
						}
						
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
					}
				}
				
				//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
				//wlSet.m_arrPara.AotpCode = Parameter_Array[PARA_VOUT];
				//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
				//if(*wlSet.m_on==WLM_ON)
				//{
					//wlSet.m_arrPara.AotpCode=Parameter_Array[PARA_VOUT];
					//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
					//SETBIT(bitWLMaster,BIT_bitWLM_Aotp);
					
				//}else
				//{
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
				//}
			}
		    	
		//	for(search_index=0;search_index<sizeof(output_table);search_index++)
		//	{
		//		if((Parameter_Array[PARA_VOUT]&0x7f)==output_table[search_index])
		//		{
		//			main_parameter.outputChangeCode=search_index/*(0xc0|search_index)*/;
		//			main_parameter.VIP_output1=(8-(search_index+2)/3);
		//			main_parameter.VIP_output2=(2-(search_index+2)%3);
		//			break;
		//		}
		//	}
		//	if(mainPara.workMode==WORKINGMODE_MANU)
		//	{
		//		SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
		//	}
		break;
////////////////////////////////////////////////////////
		case 0xaa:			
		break;

		case 0xab:
		break;

		case 0xac:
		break;

		case 0xb2:		
		break;
		

		case 0xbd:
		//main_parameter.focLen = g_spiRcvData[2];
		//if((main_parameter.moto_mode == MOTO_MODE_AUTO)&&(menuPara.motoLoc!=g_spiRcvData[2]))
		if(menuPara.motoLoc!=g_spiRcvData[2])
		{
			//main_parameter.moto_location= main_parameter.focLen;
			//menuPara.motoLoc=g_spiRcvData[2];
			menuPara.motoLoc = g_spiRcvData[2];
			
			if(MOTO_MODE_AUTO==*motoSet.mode)
			{
				//if(((*motoSet.destRsv>menuPara.motoLoc)&&(*motoSet.destRsv<g_spiRcvData))
					//||(*motoSet.destRsv<menuPara.motoLoc)&&(*motoSet.destRsv>g_spiRcvData))
					//if(*motoSet.destRsv<menuPara.motoLoc)&&(menuPara.mo)
				//{
					//menuPara.motoLoc = g_spiRcvData[2];
					//*motoSet.destRsv = g_spiRcvData[2];
				//}
				SET_UPDATE_EVT(DSP_UPDATE_MOTO);
			}
			//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
			//menuPara.motoLoc = g_spiRcvData[2];
		}
		break;

#if 0
		case 0xb7:
		if(g_spiRcvData[1]>0)
		{
			if(!g_cmrPressed)
			{
				g_SettingIndex = 0;
			}
			g_cmrPressed = 1;
		}else
		{
			g_cmrPressed = 0;
		}
		g_isFullFrame = 1;
			//g_cmrPressed = 1;
		//if(flaPara.apert!=g_spiRcvData[1])
		if(menuPara.cApert!=g_spiRcvData[1])
		{
			//flaPara.apert = g_spiRcvData[1];
			
			menuPara.cApert = g_spiRcvData[1];	
			//tmp1 = menuPara.cApert>>4;
			//tmp2 = menuPara.cApert&0x0F;
			
			flaPara.apert = CannonApertVal[(menuPara.cApert>>4)*6+((menuPara.cApert&0x0F)+2)/3];
			
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
			//SPI_MISO_AUX=1;
			//g_dspUpdateFlag|=DSP_UPDATE_LINK;
			//SETBIT(bitTerminate,BIT_bitTerminate_APERT);
		}
		break;
#endif

/***************************************************************************
		-3          -2          -1           0		1		2		3
		|___|___|___|___|___|___|___|___|___|___|___|___|				//步长为0.5EV
		0x18     0x10       0x08       0x00      0xF8       0xF0       0xE8
		     0x14       0x0C      0x04        0xFC      0xF4        0xEC

		-3          -2              -1             0		1		 2		   3
		|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|
		18,15, 13, 10,0D,0B,08, 05, 03, 0,   FD,FB,F8, F5,  F3, F0, ED, EB E8

***************************************************************************/

		case 0xA8:							//TTL compensation
			//cvtTmp = (signed char)g_spiRcvData[1];
			//flaPara.cmrCmp1 = CMPH_COV(g_spiRcvData[1]);
			//flaPara.cmrCmp2 = CMPL_COV(g_spiRcvData[1]);
			if(flaPara.cmrCmpCode!=g_spiRcvData[1])
			{
				flaPara.cmrCmpCode = g_spiRcvData[1];

				flaPara.cmrCmp1 = CMPH_COV(g_spiRcvData[1]);
				flaPara.cmrCmp2 = CMPL_COV(g_spiRcvData[1]);
				
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SET_UPDATE_EVT(DSP_UPDATE_LINK);
			}
		break;


		case 0xb8:
//#ifdef _MULTIPLY_FLASH_
		//if(multiSet.sCode!=g_spiRcvData[1])
		//{
			//multiSet.sCode = g_spiRcvData[1];
			//g_dspUpdateFlag |= DSP_UPDATE_MULCAL;
			//SETBIT(bitExtra,BIT_bitExtra_Shutter);
		//}
//#endif
#ifdef CANON_RF
		if(flaPara.shutter!=g_spiRcvData[1])
		{
			flaPara.shutter = g_spiRcvData[1];
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
#endif
		break; 



		case 0xe6:
		break;

		case 0xfc:
		break;

		case 0xb1:
			//g_hpLev = g_spiRcvData[1];
		break;

		case 0xbb:
/*
		if(flaPara.isoCmp!=g_spiRcvData[1])
		{
			flaPara.isoCmp = g_spiRcvData[1];
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
*/
/*		if(mainPara.workMode==WORKINGMODE_TTL)
		{
			for(search_index=0;search_index<sizeof(follow_bb_table);search_index++)
			{
				if(g_spiRcvData[1]==follow_bb_table[search_index])
				{
					main_parameter.flashCompensation=(0xa0|search_index);
						
				}
			}
		}
		SETBIT(paraBit,BIT_paraBit_TTL_COMPENSATION_VARY); 	   */
		break;

		case 0xb0:
//		if(ttl_time_calculate)
		//if(GETBIT(bitIndex,BIT_bitIndex_TTLTIMECALC))
		if(g_getTtlFlashCode)                       // pre flash set 1
		{
			ttl_expCodeRsv = g_spiRcvData[1];
			g_getTtlFlashCode = 0;
				 
		 }
		break;	 

		case 0xC2:   // rf relative
		break;

		case 0xC7:						//multiply parameter
		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			//multiSet.cmdCode = g_spiRcvData[3];
			//if((menuPara.WlmMul[WL_ARRARY_A] != g_spiRcvData[3])||
			if ((output_table[(7-(*multiSet.expose>>4))*3]  != g_spiRcvData[3]) ||
			(*multiSet.frequence !=MAKEWORD(g_spiRcvData[5], g_spiRcvData[4])/* g_spiRcvData[5]*/)||
			(*multiSet.eTimes != g_spiRcvData[6]))
			{
				//menuPara.WlmMul[WL_ARRARY_A] = (g_spiRcvData[3]&0x7F);
				//wlSet.m_arrPara[WL_ARRARY_A].multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A])<<4;
				//wlSet.m_arrPara[WL_ARRARY_A].multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A]);
				*multiSet.expose = EXPH_COV(g_spiRcvData[3]&0x7F)<<4;
				*multiSet.frequence = MAKEWORD(g_spiRcvData[5], g_spiRcvData[4]);//g_spiRcvData[5];
				*multiSet.eTimes = g_spiRcvData[6];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			
			//SETBIT(bitMultip,BIT_bitMultip_Exp);
			//SETBIT(bitMultip,BIT_bitMultip_TmFrq);
		}else
		{
			//if(menuPara.otpCod[MASTER]!=g_spiRcvData[3])
			if ((output_table[(7-mstPara->output1)*3-mstPara->output2] !=(g_spiRcvData[3]&0x7f)) 
				&& ((g_spiRcvData[3]&0x7f) <= 0x38))
			{
				menuPara.otpCod[MASTER] = g_spiRcvData[3];
				mstPara->output1 = EXPH_COV(g_spiRcvData[3]&0x7F);
				mstPara->output2 = EXPL_COV(g_spiRcvData[3]&0x7f);
				/*20180124*/
					if ((mstPara->output1 > 7) || (mstPara->output1 < 0)
						|| (mstPara->output2 < 0) ||(mstPara->output2 > 2))
					{
						mstPara->output2 = 0;
						mstPara->output1 = 0;
					}
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
		}
		
		break;

		case 0xb5:			//step length 2015/07/06
		break;

		case 0xB3:
			if((0X12==g_spiRcvData[1])&&(0X46==g_spiRcvData[2]))
			{
				g_AfIndex = 0;
			}else if((0X12==g_spiRcvData[1])&&(0X47==g_spiRcvData[2]))
			{
				g_AfIndex = 1;
			}else if((0x13==g_spiRcvData[1])&&(0x46==g_spiRcvData[2]))
			{
				g_AfIndex = 2;
			}else
			{
				g_AfIndex = 0;
			}
		break;
	
/*		case 0xa8:
			switch(SPI_Data_Array[1])
			{
			}*/

//		break;
		default:
		break;
}
}

void CodeAnalysis(u8 rcvCode)
{
	//GetParameter();
	s8 i=0;
//	INT8U tmrErr;
	//g_spiRcvIndex= 0 ;
	//switch(g_spiRcvData[0])
	switch(rcvCode)
	{
		case 0xff:
		//g_spiRcvData[g_spiRcvIndex++]=0xff;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		/*case 0xb5: 
		g_spiRcvData[g_spiRcvIndex++]=0xb5;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xde;
			break;*/
		case 0xb5:
		//g_spiRcvData[g_spiRcvIndex++]=0xb5;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xde;	
			//if(g_paraUpdateFlag>0)
			//{
			//	OSTmrStop(g_paraUpdateTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);
			//	g_paraUpdateFlag = 0;
			//}
			break;
		case 0xe5:														//*ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xe5;
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			//g_spiSendData[g_spiSendIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x11;                                    // 2017/08/10 for moto test
#if 0
			switch(*g_cmdMode)
			{
				case CMD_MODE_RF_MST:
					g_spiSendData[g_spiSendIndex++]=0x03;
					//g_spiSendData[g_spiSendIndex++]=0x13;
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						g_spiSendData[g_spiSendIndex++]=0x15;				//RF LINK_SHOT with flash open
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x07;
					}
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x01;
					//g_spiSendData[g_spiSendIndex++]=0x11;                              // FOR 5DMARKIII NO FLASH 2016/08/11
				break;

				
			}
#endif
			g_spiSendData[g_spiSendIndex++]=0x48;
			g_spiSendData[g_spiSendIndex++]=0x4d;
			break;
		case 0xa9:														//*ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xa9;  
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			//g_spiSendData[g_spiSendIndex++]=0x97;
			break;
		case 0xa1:														//*ST-E3
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			//g_spiSendData[g_spiSendIndex++]=0x4C;
			/*
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x29;
				g_spiSendData[g_spiSendIndex++]=0x29;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x4B;
				g_spiSendData[g_spiSendIndex++]=0x4B;
			}*/
			break;
		case 0xa5:
			//g_spiRcvData[g_spiRcvIndex++]=0xa5;
			g_spiSendData[g_spiSendIndex++]=0x96;
			//g_spiSendData[g_spiSendIndex++]=0x97;
			g_spiSendData[g_spiSendIndex++]=menuPara.para_a5;//0x00;//0x00;//this should never ignored, otherwith cause error when work on the canon 600d
			//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_A5];//0x00;	           //2010/10/20  for ttl test 
			//if(GETBIT(bitTerminate,BIT_bitTerminate_A5FLAG))
			//{
			//	g_spiSendData[g_spiSendIndex++]=0x01;//Parameter_Array[PARA_A5];//0x00;	           //2010/10/20  for ttl test 
			//	CLRBIT(bitTerminate,BIT_bitTerminate_A5FLAG);
			//}else
			//{
			//	g_spiSendData[g_spiSendIndex++]=0x00;
			//}
			//g_spiSendData[g_spiSendIndex++]=0x01;
			break;
			
		case 0xb9:
			//g_spiRcvData[g_spiRcvIndex++]=0xb9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//switch(flaPara.surrand_state)
			switch(flaPara.FEB_count)
			{
				//case 0:
				//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_COMP];//0x00;  //....this place index a surrand flash
				//break;
				case 0:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
				break;

				case 1:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
				break;

				case 2:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6+flaPara.ttl_FEB1)*3+mstPara->cmp2+flaPara.ttl_FEB2];
				break;

			}			
			break;
			
		case 0xbd:                                                         // 2017/08/10 for moto test
			if (MOTO_MODE_AUTO == *motoSet.mode)
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;//Parameter_Array[PARA_FOCLEN];
				g_spiSendData[g_spiSendIndex++]=0x59;//0x69;//moto range
				g_spiSendData[g_spiSendIndex++]=0xAA;//0x18;
			}else
			{/*
				g_spiSendData[g_spiSendIndex++]=0x02;
				g_spiSendData[g_spiSendIndex++]=*motoSet.dest_location;//Parameter_Array[PARA_FOCLEN];
				g_spiSendData[g_spiSendIndex++]=0xC8;//0x69;//moto range
				g_spiSendData[g_spiSendIndex++]=0x14;//0x18;
				*/
				g_spiSendData[g_spiSendIndex++]=0x02;                       
				g_spiSendData[g_spiSendIndex++]=*motoSet.dest_location;//Parameter_Array[PARA_FOCLEN];
				g_spiSendData[g_spiSendIndex++]=0xC8;//0x69;//moto range
				g_spiSendData[g_spiSendIndex++]=0x14;//0x18;
				//g_spiSendData[g_spiSendIndex++]=0x02;
			}
			//g_spiSendData[g_spiSendIndex++]=0x18;//0x18;
		break;
			
		case 0xfb:
			//g_spiRcvData[g_spiRcvIndex++]=0xfb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			
			g_spiSendData[g_spiSendIndex++] = (0x5d|((SYNC_MODE_REAR==mainPara.synMode)<<1));

			g_spiSendData[g_spiSendIndex++]=0x8e;

			if(WORKINGMODE_TTL==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x02;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}							
			break;
			
		case 0xf9:
			//g_spiRcvData[g_spiRcvIndex++]=0xf9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=(((g_chargeOk>AD_CHARGE_LOW)?0x55:0x54)|((SYNC_MODE_REAR==mainPara.synMode)<<1));
			//g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54;
			//g_spiSendData[g_spiSendIndex++]=0x54;
/*
//#ifdef _ST_EX_
			switch(*g_cmdMode)
			{
				//case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					//g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54; 			//in the rf Master Mode, if there are SLAVEs linked, it send 0x55
					g_spiSendData[g_spiSendIndex++]=(RfMasterReadyCheck()>AD_CHARGE_LOW)?0x55:0x54; 
				break;

				case CMD_MODE_LIGH_MST:
					g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_RDY)?0x55:0x54;
				break;
				
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54;
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x55;	
					}
				break;

				case CMD_MODE_OFF:
					g_spiSendData[g_spiSendIndex++]=(((g_chargeOk>AD_CHARGE_LOW)?0x55:0x54)|((SYNC_MODE_REAR==mainPara.synMode)<<1));

				break;

				default:
					//g_spiSendData[g_spiSendIndex++]=0x55;	
					g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54;
				break;
			}
//#else
//			g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2]; 
//#endif
*/
			break;

		case 0xb2:
			//g_spiRcvData[g_spiRcvIndex++]=0xb2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;
		
		case 0xb3:                      //2017/08/10 for moto test
			//g_spiRcvData[g_spiRcvIndex++]=0xb3;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x08;
			//g_spiSendData[g_spiSendIndex++]=0x08;
#if 0
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
						{
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
						}else
						{
							g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
							g_spiSendData[g_spiSendIndex++]=abRation_fill2;
							g_spiSendData[g_spiSendIndex++]=abRation_fill1;
						}
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x00;
						g_spiSendData[g_spiSendIndex++]=0x00;
						g_spiSendData[g_spiSendIndex++]=0x00;
					/*
						if(WLM_MODE_ALL==*wlSet.arr_mode)
						{
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
						}else
						{
							if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
							{
								g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
								g_spiSendData[g_spiSendIndex++]=abRation_fill2;
								g_spiSendData[g_spiSendIndex++]=abRation_fill1;
							}else
							{
								g_spiSendData[g_spiSendIndex++]=0x00;
								g_spiSendData[g_spiSendIndex++]=0x00;
								g_spiSendData[g_spiSendIndex++]=0x00;
							}
							
						}
					*/
					}
				break;
				
/*
				case CMD_MODE_RF:
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;
*/			
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_OFF:		
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;

			}
			//SETBIT(paraBit,0);
#endif
			break;

		case 0xf5:
			//g_spiRcvData[g_spiRcvIndex++]=0xf5;
			g_spiSendData[g_spiSendIndex++] = 0x00;
			//g_spiSendData[g_spiSendIndex++]=0x0f;
			//g_spiSendData[g_spiSendIndex++]=(0x0f|((!g_isRgbFilter)<<4));
			g_spiSendData[g_spiSendIndex++] = 0x0f;
			g_spiSendData[g_spiSendIndex++] = 0x5c;
#if 0
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=0x5d+*wlSet.ttl_Ratio;
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x5d+*wlSet.arr_mode;
				}
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=(0x5D+*wlSet.ttl_Ratio);
					//g_spiSendData[g_spiSendIndex++]=(0x4D+*wlSet.ttl_Ratio);
				}else
				{
					//2015/08/24 0X4D is invalid when modeling
					g_spiSendData[g_spiSendIndex++]=(0x5D+*wlSet.arr_mode);//0x4d+*wlSet.arr_mode+(WORKINGMODE_GR==mainPara.workMode);
				}
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x5c;
			}
#endif
			//if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
			if( WORKINGMODE_TTL==mainPara.workMode )
			{
				//g_spiSendData[g_spiSendIndex++]=(0x49|((flaPara.surrand_state<<4)&0x30));
				g_spiSendData[g_spiSendIndex++]=0x41;                    
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x48;
			}
/******************************************************************************************
//			here restrain shtter speed in mode except TTL, MANU, Gr
*******************************************************************************************/
			//if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_MANU==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
			if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_MANU==mainPara.workMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x53|((SYNMODE_HP==mainPara.synMode)<<2);//Parameter_Array[PARA_SYNC3];
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x53;
			}
			break;
		case 0xbb:
			//g_spiRcvData[g_spiRcvIndex++]=0xbb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=menuPara.feb;//Parameter_Array[PARA_SRD];//0x00;			//feb
			g_spiSendData[g_spiSendIndex++]=feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];
//#ifdef _ST_EX_
			g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];//menuPara.mstTtl;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//if(!g_paraUpdateFlag)
			//{
			//	OSTmrStart(g_paraUpdateTmr, &tmrErr) ;
			//	g_paraUpdateFlag = 1;
			//}
//#else
//			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
//#endif
			//value = Parameter_Array[PARA_COMP];
			break;

		//indicate how many devices have linked to the master
		case 0xD6:				//surpose all groups have device monted
			//g_spiRcvData[g_spiRcvIndex++]=0xD6;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
#if 0			
			//switch(*wlSet.arr_mode)
			switch((WORKINGMODE_TTL==mainPara.workMode)?*wlSet.ttl_Ratio:*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
					//g_spiSendData[g_spiSendIndex++]=((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0));//0x01;
					g_spiSendData[g_spiSendIndex++]=((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0)||(*wlSet.is_masterOn>0));
				break;

				case WLM_MODE_AB:
					//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2));//0x06;
					//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|(*wlSet.is_masterOn>0));
					g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((*wlSet.is_masterOn>0)<<1));
				break;

				case WLM_MODE_RES:
					//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3));//0x0E;
					//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3)|(*wlSet.is_masterOn>0));
					g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3)|((*wlSet.is_masterOn>0)<<1));
				break;
			}
#endif
			
			//g_spiSendData[g_spiSendIndex++]=0x01;
		break;

		case 0xD2:
			//g_spiRcvData[g_spiRcvIndex++]=0xD2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x00;		
			if(UART_CC_REL_FLA==g_shutterFlag)						//shutter control, only used in camera issued after 2011
			{
				g_spiSendData[g_spiSendIndex++]=0x11;//g_shutterFlag?(0x11;g_shutterFlag=0):0x00;
				g_shutterFlag = 0;
			}else if(UART_CC_REL_MODEL==g_shutterFlag)
			{
				g_spiSendData[g_spiSendIndex++]=0x22;
				g_shutterFlag = 0;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}
			//0x00;
		break;

		case 0x01:
			//g_spiRcvData[g_spiRcvIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;
		
		case 0xe2:
			//g_spiRcvData[g_spiRcvIndex++]=0xe2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=menuPara.motoCod;//Parameter_Array[PARA_FOCIND];			//焦距模式
			break;
		case 0xea:						//determine the Wireless command mode: RF,LIGHT,OFF
			//g_spiRcvData[g_spiRcvIndex++]=0xea;
			g_spiSendData[g_spiSendIndex++]=0x8e;							
			//g_spiSendData[g_spiSendIndex++]=*wlSet.m_chan;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x03;//0x03;
			g_spiSendData[g_spiSendIndex++]=0x00;	
#if 0		
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.ttl_Ratio)<<4)|((*wlSet.is_masterOn)<<3));
					}else
					{
						g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
					}
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x01;
				break;

				default:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.ttl_Ratio)<<4));
					}else
					{
						g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
					}
				break;
			}
#endif
			/*
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
				
			}*/
			break;
			
		case 0xeb:
			//g_spiRcvData[g_spiRcvIndex++]=0xeb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x06;
#if 0
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;//0x00;
				//g_spiSendData[g_spiSendIndex++]=0x00;
				//switch(*wlSet.arr_mode)
				switch(*wlSet.ttl_Ratio)
				{
					case WLM_MODE_AB:
						g_spiSendData[g_spiSendIndex++]=*wlSet.abRatio;
					break;

					case WLM_MODE_RES:
						//g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
						g_spiSendData[g_spiSendIndex++]=*wlSet.abRatio;
						//wlSet.m_arrPara.ABRation = g_spiRcvData[3];
					break;

					default :
						g_spiSendData[g_spiSendIndex++]=0;
					break;
				}
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_OFF:
					g_spiSendData[g_spiSendIndex++]=0x8e;
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x06;
				break;
				
			}
#endif

		break;
		
		case 0xec:
		g_spiSendData[g_spiSendIndex++]=0x8e;
		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			//g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_C];
			//g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_B];
			g_spiSendData[g_spiSendIndex++]=0x00;//menuPara.WlmMul[WL_ARRARY_C];
			g_spiSendData[g_spiSendIndex++]=0x00;//menuPara.WlmMul[WL_ARRARY_B]
			//g_spiSendData[g_spiSendIndex++]=multiSet.WlmMul[WL_ARRARY_A];
			//g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_A];//menuPara.WlmMul[WL_ARRARY_A]&0x7F;//multiSet.cmdCode&0x7F;
			g_spiSendData[g_spiSendIndex++]=output_table[(7-(*multiSet.expose>>4))*3];
		}else
		{						
			//g_spiSendData[g_spiSendIndex++]=wlSet.m_arrPara.CotpCode;
			//g_spiSendData[g_spiSendIndex++]=wlSet.m_arrPara.BotpCode;
			//g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[WL_ARRARY_C];//wlSet.otpCode[WL_ARRARY_C];
			//g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[WL_ARRARY_B];//wlSet.otpCode[WL_ARRARY_B];
			g_spiSendData[g_spiSendIndex++]=0x00;//wlSet.otpCode[WL_ARRARY_C];
			g_spiSendData[g_spiSendIndex++]=0x00;//wlSet.otpCode[WL_ARRARY_B];
			g_spiSendData[g_spiSendIndex++]=output_table[(7-mstPara->output1)*3-mstPara->output2];//menuPara.otpCod[MASTER]&0x7f;//wlSet.m_arrPara.AotpCode;
		}
		break;
		
		case 0xe6:
			//g_spiRcvData[g_spiRcvIndex++]=0xe6;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=menuPara.feb;//Parameter_Array[PARA_SRD];			//闪光包围
			g_spiSendData[g_spiSendIndex++]=feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];
			//g_spiSendData[g_spiSendIndex++]=(VER_CAMERA_OLD==g_vCmr)?(WORKINGMODE_GR==mainPara.workMode)?():((mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
			if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(WORKINGMODE_TTL&0x07));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(mainPara.workMode&0x07));
			}
			//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC1];//mainPara.workMode;//Parameter_Array[PARA_SYNC1];//mainPara.workMode;//0x00;//Parameter_Array[PARA_SYNC1];			//mode闪光模式
			//g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=0)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07)|(g_activeControl<<5));
//#ifdef _ST_EX_
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#else
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#endif
			g_spiSendData[g_spiSendIndex++]=0x7F;//0x3f;
			break;
			
		case 0xe7:
			//g_spiRcvData[g_spiRcvIndex++]=0xe7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x10;//0x64;//multiSet.maxTimes;//0x10;
			//g_spiSendData[g_spiSendIndex++]=multiSet.eTimes;//Parameter_Array[PARA_MOUT];
			//g_spiSendData[g_spiSendIndex++]=multiSet.frequence;//Parameter_Array[PARA_FREQ];
			//g_spiSendData[g_spiSendIndex++]=0x38;
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=(multiSet.maxTimes>0)?multiSet.maxTimes:10;//0x5a;//0x64;//0x10;//0x64;//multiSet.maxTimes;//0x10;
				g_spiSendData[g_spiSendIndex++]=*multiSet.eTimes;//Parameter_Array[PARA_MOUT];
				g_spiSendData[g_spiSendIndex++]=*multiSet.frequence;//Parameter_Array[PARA_FREQ];
				g_spiSendData[g_spiSendIndex++]=0x38;
				//g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[MASTER];
				g_spiSendData[g_spiSendIndex++]=output_table[(7-(*multiSet.expose>>4))*3];
				//g_spiSendData[g_spiSendIndex++]=multiSet.cmdCode;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x38;
				//g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];//Parameter_Array[PARA_VOUT];menuPara.otpCod
				g_spiSendData[g_spiSendIndex++]=output_table[(7-mstPara->output1)*3-mstPara->output2];
/*
				if(WLM_ON==*wlSet.m_on)
				{
					g_spiSendData[g_spiSendIndex++] = wlSet.m_arrPara.AotpCode;
				}else
				{
					g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_VOUT];
				}
*/
			}
			break;
		case 0xa7:
			//g_spiRcvData[g_spiRcvIndex++]=0xa7;
#ifdef CANON_RF
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
#endif
			break;
		case 0xa6:
			//g_spiRcvData[g_spiRcvIndex++]=0xa6;
#ifdef CANON_RF
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
#endif
			break;
		case 0xb4:
			//g_spiRcvData[g_spiRcvIndex++]=0xb4;
//							g_spiSendData[g_spiSendIndex++]=0x8e;
//							g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xb0:
			//g_spiRcvData[g_spiRcvIndex++]=0xb0;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
//							pre_flash = 1;
			break;
		case 0xb1:
			//g_spiRcvData[g_spiRcvIndex++]=0xb1;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xf8:
			//g_spiRcvData[g_spiRcvIndex++]=0xf8;
//			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0xff;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			g_spiSendData[g_spiSendIndex++]=0xff;
			//g_spiSendData[g_spiSendIndex++]=0x5d;
			break;
		case 0xb7:
			//g_spiRcvData[g_spiRcvIndex++]=0xb7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xb8:
			//g_spiRcvData[g_spiRcvIndex++]=0xb8;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x91;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xa8:
			//g_spiRcvData[g_spiRcvIndex++]=0xa8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x97;//0x96;//0x91;
			//g_spiSendData[g_spiSendIndex++] = 0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			g_spiSendData[g_spiSendIndex++] = 0x96;
			break;

		case 0xC4:
			//g_spiRcvData[g_spiRcvIndex++]=0xC4;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x8e;	
		break;

		case 0xC2:
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
			g_spiSendData[g_spiSendIndex++]=0x8e;	
#if 0
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[4];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[3];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[2];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[1];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[0];//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	
			}else
			{
				if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
				{
					g_spiSendData[g_spiSendIndex++]=0x8e;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
					g_spiSendData[g_spiSendIndex++]=0x8e;	
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x8e;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;	
					g_spiSendData[g_spiSendIndex++]=abRation_fill2;	
					g_spiSendData[g_spiSendIndex++]=abRation_fill1;//0x80;//0x00;	cause serious problem 
					g_spiSendData[g_spiSendIndex++]=0x8e;	
				}
				
			}
#endif
#if 0
			//g_spiRcvData[g_spiRcvIndex++]=0xC2;
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
			g_spiSendData[g_spiSendIndex++]=0x8e;	
#endif
		break;

		case 0xC8:
			//g_spiRcvData[g_spiRcvIndex++]=0xC8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xCA:
			//g_spiRcvData[g_spiRcvIndex++]=0xCA;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xC7:
			//g_spiRcvData[g_spiRcvIndex++]=0xC7;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xCC:
			//g_spiRcvData[g_spiRcvIndex++]=0xCC;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;

		case 0xD7:
			//g_spiRcvData[g_spiRcvIndex++]=0xD7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=(multiSet.maxTimes>0)?multiSet.maxTimes:10;//multiSet.maxTimes;
				g_spiSendData[g_spiSendIndex++]=*multiSet.eTimes;
				g_spiSendData[g_spiSendIndex++]=(*multiSet.frequence)&0xFF;
				g_spiSendData[g_spiSendIndex++]=(*multiSet.frequence)>>8;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
			}
			g_spiSendData[g_spiSendIndex++]=0x38;
			switch(mainPara.workMode)
			{
				case WORKINGMODE_TTL:
				//case WORKINGMODE_GR:
					g_spiSendData[g_spiSendIndex++]=0x00;
				break;

				case WORKINGMODE_MANU:
					//g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];
					g_spiSendData[g_spiSendIndex++]=output_table[(7-mstPara->output1)*3-mstPara->output2];
				break;

				case WORKINGMODE_MULTI:
					//g_spiSendData[g_spiSendIndex++]=multiSet.cmdCode;
					//g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[MASTER];
					g_spiSendData[g_spiSendIndex++] = output_table[(7-(*multiSet.expose>>4))*3];
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x00;
				break;
			}
		break;

		case 0xD8:										//ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xD8;
#ifdef _ST_EX_
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x7f;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x13;
			g_spiSendData[g_spiSendIndex++]=0x7f;
			g_spiSendData[g_spiSendIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=(MOTO_MODE_AUTO == *motoSet.mode ? 0x44 : focLenCode[motoSet.focIndex]);//motoSet.focLen;//0x05;
#endif
		break;
		
		case 0xa3:
			//g_spiRcvData[g_spiRcvIndex++]=0xa3;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
			
		case 0xf2:
			//g_spiRcvData[g_spiRcvIndex++]=0xf2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xa0;//cameraSetting.prefla_com;
			break;
		case 0xfc:	 
			//g_spiRcvData[g_spiRcvIndex++]=0xfc;
#if 1
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x32;
			g_spiSendData[g_spiSendIndex++]=0x85;
			g_spiSendData[g_spiSendIndex++]=0x21;
			g_spiSendData[g_spiSendIndex++]=0x1a;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xC0;
			g_spiSendData[g_spiSendIndex++]=ttl_expCodeRsv;
#else
			
			g_spiSendData[g_spiSendIndex++]=0x8E;//0x8e;
			g_spiSendData[g_spiSendIndex++]=g_colorTempRt&0xFF;//0x32;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>8)&0xFF;//0x85;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>16)&0xFF;//0x21;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>24)&0xFF;//0x1a;
			g_spiSendData[g_spiSendIndex++]=0xC0;
			g_spiSendData[g_spiSendIndex++]=ttl_expCodeRsv;
#endif
			break;
		case 0xa2:
			//g_spiRcvData[g_spiRcvIndex++]=0xa2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;
		case 0xe1:
			//g_spiRcvData[g_spiRcvIndex++]=0xe1;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x91;
			//SETBIT(bitExtra,BIT_bitExtra_A8index);
#if 0							
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xf8;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x3c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x3c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c; 
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xf8;

			for(i=23;i>19;i--)
			{
				g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C);
			}
			for(i=19;i>13;i--)
			{
				g_spiSendData[g_spiSendIndex++] = 0x00;
			}
			for(i=13;i>=0;i--)
			{
				g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C);;
			}
			//g_spiSendData[g_spiSendIndex++] = 0xF8;
			
			//g_spiSendData[g_spiSendIndex++]=0x0C;
			//g_spiSendData[g_spiSendIndex++]=0x1C;
			//g_spiSendData[g_spiSendIndex++]=0x1C;
			//g_spiSendData[g_spiSendIndex++]=0x0D;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0C;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x3F;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
#endif
			break;	
		case 0xaa:
			//g_spiRcvData[g_spiRcvIndex++]=0xaa;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			break;	
		case 0xab:
			//g_spiRcvData[g_spiRcvIndex++]=0xab;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
		break;
		case 0xac:
			//g_spiRcvData[g_spiRcvIndex++]=0xac;
			g_spiSendData[g_spiSendIndex++]=0x8e; 						
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
		break;

		case 0xDA:											//just for st-e3
			//g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x00; 						
			g_spiSendData[g_spiSendIndex++]=0x00; 
			g_spiSendData[g_spiSendIndex++]=0x00;
			/*
			g_spiSendData[g_spiSendIndex++]=(radioSet->id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(radioSet->id>>8); 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfChan;//wlSet.rfSet.chan;*/
			g_spiSendData[g_spiSendIndex++]=0x1F;
			g_spiSendData[g_spiSendIndex++]=0x00;
#if 0
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.ttl_Ratio<<5)|(*wlSet.is_masterOn<<4));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				}
			}else
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=((*wlSet.ttl_Ratio<<5)|(*wlSet.is_masterOn<<4));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=((*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				}
			}
#endif
			//g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		/*
		if(CMD_MODE_RF==*g_cmdMode)
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id>>8); 
			g_spiSendData[g_spiSendIndex++]=wlSet.rfSet.chan;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			//g_spiSendData[g_spiSendIndex++]=(0x14|(radioSet.ratio<<5));
			g_spiSendData[g_spiSendIndex++]=(0x14|(wlSet.arr_mode<<5));
			//g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		}else if(CMD_MODE_LIGH==*g_cmdMode)
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x00; 						
			g_spiSendData[g_spiSendIndex++]=0x00; 
			g_spiSendData[g_spiSendIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		}else
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id>>8); 
			g_spiSendData[g_spiSendIndex++]=wlSet.rfSet.chan;;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			g_spiSendData[g_spiSendIndex++]=0x10;
		}*/
		break;

		case 0xC3:
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
		break;

		//RF mode 
		//0x00: ETTL
		//0x01: MANU
		//0x04: Ext.A Auto external lightting measure
		//0x06: OFF
		case 0xDB:
			//g_spiRcvData[g_spiRcvIndex++]=0xDB;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x00; //(radioSet->rfGr[4].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[4]:menuPara.rfGrTmp[4];//flaPara.rfCmuPara[4].optCode;//rfGrPara.rfGr_arrPara[4].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=0x00; //GrModConvert(4);//radioSet->rfGr[4].mode;//rfGrPara.rfGr_arrPara[4].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=0x00; //(radioSet->rfGr[3].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[3]:menuPara.rfGrTmp[3];//flaPara.rfCmuPara[3].optCode;//rfGrPara.rfGr_arrPara[3].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=0x00; //GrModConvert(3);//radioSet->rfGr[3].mode;//rfGrPara.rfGr_arrPara[3].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=0x00; //(radioSet->rfGr[2].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[2]:menuPara.rfGrTmp[2];//flaPara.rfCmuPara[2].optCode;//rfGrPara.rfGr_arrPara[2].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=0x00; //GrModConvert(2);//radioSet->rfGr[2].mode;//rfGrPara.rfGr_arrPara[2].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=0x00; //(radioSet->rfGr[1].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[1]:menuPara.rfGrTmp[1];//flaPara.rfCmuPara[1].optCode;//rfGrPara.rfGr_arrPara[1].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=0x00; //GrModConvert(1);//radioSet->rfGr[1].mode;//rfGrPara.rfGr_arrPara[1].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=0x00; //(radioSet->rfGr[0].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[0]:menuPara.rfGrTmp[0];//flaPara.rfCmuPara[0].optCode;//rfGrPara.rfGr_arrPara[0].optCode; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=0x00; //GrModConvert(0);//radioSet->rfGr[0].mode;//rfGrPara.rfGr_arrPara[0].mode;//0x01; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
		break;

		case 0xCB:										//just for st-e3
			//g_spiRcvData[g_spiRcvIndex++]=0xCB;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
		break;
		
		case 0xbe:
			//g_spiRcvData[g_spiRcvIndex++]=0xbe;
		    	g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			break;
			
		case 0x7e:
			//SPIx_Disable();
			//g_spiRcvData[g_spiRcvIndex++]=0x7e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//SPIx_Enable();
			g_spiSendData[g_spiSendIndex++]=0xd9;
			g_spiSendData[g_spiSendIndex++]=0x82;
			g_spiSendData[g_spiSendIndex++]=0xbc;
			g_spiSendData[g_spiSendIndex++]=0x18;
			g_spiSendData[g_spiSendIndex++]=0xbe;
			g_spiSendData[g_spiSendIndex++]=0xbe; 
			break;	
			
		default:
			//SPIx_Disable();
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//SPIx_Enable();
			break;
	}
}



