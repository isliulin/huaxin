//#include <stm32f10x_lib.h>		 
#include "stm32f10x.h"
#include "sys.h"
#include "../../user/golbal.h" 
#include "../../user/Display.h" 
#include "../../HARDWARE/STMFLASH/stmflash.h"
#include "../../HARDWARE/lcd/Lcd.h"
#include "usart.h"
#include "../../user/canon.h"



//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量
//CHECK OK
//091207
typedef  void (*iapfun)(void);
u8 g_sleepFlag=0;
extern unsigned char uart_tran_flag;
extern unsigned char g_keyDir;
extern RadioStatue radioStatue;
extern void RfParaSend(void);
extern void BkLiteAutoOff(void);
extern unsigned char *g_cmdMode;
extern char g_cc2530Wake;
extern void BkLiteOn(void);
extern void ResetKeyInt(void);
extern char *pSnrSet;
void iap_load_app(u32 appxaddr);
extern char g_SettingIndex;
extern char g_chargeOk;
extern unsigned char g_comStatue;
extern char g_cmrLinked;
extern unsigned char g_tmHandleFlag;

void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 
  	//检查参数合法性
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 
//CHECK OK
//091209
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先
//CHECK OK
//100329
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u32 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	u8 IPRADDR=NVIC_Channel/4;  //每组只能存4个,得到组地址 
	u8 IPROFFSET=NVIC_Channel%4;//在组内的偏移
	IPROFFSET=IPROFFSET*8+4;    //得到偏移的确切位置
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//取低四位

	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;//使能中断位(要清除的话,相反操作就OK)
	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);    
	//NVIC->IPR[IPRADDR]|=temp<<IPROFFSET;//设置响应优先级和抢断优先级   	    	  				   
	NVIC->IP[IPRADDR]|=temp<<IPROFFSET;//设置响应优先级和抢断优先级   	    	  				   
}

//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:GPIOx:0~6,代表GPIOA~G;BITx:需要使能的位;TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   
//待测试...
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//得到中断寄存器组的编号
	EXTOFFSET=(BITx%4)*4;

	RCC->APB2ENR|=0x01;//使能io复用时钟

	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx
	
	//自动设置
	//EXTI->IMR|=1<<BITx;//  开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	//if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
	//if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
	//not active rasing eadge and falling eadge at the same time
	if(DU_TIR==(TRIM&0x03))
	{
		EXTI->FTSR|=1<<BITx;
		EXTI->RTSR|=1<<BITx;
	}else if(TRIM&0x01)
	{
		EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
		EXTI->RTSR&=~(1<<BITx);
 	}else if(TRIM&0x02)
	{
		EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
		EXTI->FTSR&=~(1<<BITx);
	}

	EXTI->IMR|=1<<BITx;//  开启line BITx上的中断

} 

void Ex_NVIC_Enable(u8 BITx)
{
	EXTI->IMR |= 1<<BITx;
}

void Ex_NVIC_Disable(u8 BITx)
{
	EXTI->IMR &= ~(1<<BITx);
}


//不能在这里执行所有外设复位!否则至少引起串口不工作.		    
//把所有时钟寄存器复位
//CHECK OK
//091209
void MYRCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000;//复位结束			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR = 0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.	  
  	RCC->APB2ENR = 0x00000000; //外设时钟关闭.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //使能内部高速时钟HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //复位HSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //复位HSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //关闭所有中断
	//配置向量表				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else   
	//MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x5000);	 
#endif
}
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI
//CHECK OK
//091209
__asm void WFI_SET(void)
{
	WFI;    
}
//进入待机模式	 
//check ok 
//091202
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//使能SLEEPDEEP位 (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
 	PWR->CSR|=1<<8;          //设置WKUP用于唤醒
	PWR->CR|=1<<2;           //清除Wake-up 标志
	PWR->CR|=1<<1;           //PDDS置位		  
	WFI_SET();				 //执行WFI指令		 
}	 

void Sys_StopMode()
{
	//g_sleepFlag = 1;
	SCB->SCR|=1<<2;
	RCC->APB1ENR|=1<<28;
	PWR->CR |= 1<<2;
	PWR->CR &= 0xFD;
	PWR->CR |= 0x01;		//在停机模式下电压调压器处于低功耗模式
	WFI_SET();
} 



void RCC_Configuration(void)
{
	/* RCC system reset(for debug purpose) */
	/* 复位所有的RCC外围设备寄存器，不改变内部高速振荡器调整位（HSITRIM[4..0]）以及
	备份域控制寄存器（RCC_BDCR）,控制状态寄存器RCC_CSR */
	RCC_DeInit();

	/* Enable HSE */
	/* 开启HSE振荡器 */
	/* 三个参数
	RCC_HSE_ON 开启
	RCC_HSE_OFF 关闭
	RCC_HSE_BYPASS 使用外部时钟振荡器
	*/
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	/* HSEStartUpStatus为枚举类型变量，2种取值，0为ERROR，非0为SUCCESS
	等待HSE准备好，若超时时间到则退出*/
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();

	/* 当HSE准备完毕切振荡稳定后 */
	if(RCC_WaitForHSEStartUp()== SUCCESS)
	{
	/* HCLK = SYSCLK */
	/* 配置AHB时钟，这个时钟从SYSCLK分频而来
	分频系数有1，2，4，8，16，64，128，256，512
	*/
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	/* 设置低速APB2时钟，这个时钟从AHB时钟分频而来
	分频系数为1，2，4，8，16 */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK/2 */
	/* 设置低速APB1时钟，这个时钟从AHB时钟分频而来
	分频系数为1，2，4，8，16 */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	/* PLLCLK = 8MHz * 9 = 72 MHz */
	/* 设置PLL的时钟源和乘法因子
	第一个入口参数为时钟源，共有3个
	RCC_PLLSource_HSI_Div2
	RCC_PLLSource_HSE_Div1
	RCC_PLLSource_HSE_Div2
	乘法因子RCC_PLLMul_2~RCC_PLLMul_16，之间参数连续
	*/
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	/* Enable PLL */
	/* 输入参数为ENABLE及DISABLE */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source */
	/*选择系统时钟源，三个参数
	RCC_SYSCLKSource_HSI 内部高速振荡器
	RCC_SYSCLKSource_HSE 外部高速振荡器
	RCC_SYSCLKSource_PLLCLK PLL时钟
	*/
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	/* 返回系统当前的时钟源
	返回值有3种
	0x00 HSI是当前时钟源
	0x04 HSE是当前时钟源
	0x08 PLL是当前时钟源
	*/
	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
	}

	/* Enable GPIOB, GPIOC and AFIO clocks */
	/* 使能低速总线APB2上的外围设备
	这里打开了IO端口B的时钟
	AFIO是重映射功能的时钟，一般未使用REMAP功能时，此时钟不必开启
	输入参数见UM0472 PAGE 426
	*/
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

void PortUninit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_Init(GPIOE,  &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//
       //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       //GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//	PC5 peripheral power
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOC, &GPIO_InitStructure);
	   
	GPIO_Write(GPIOA, 0X0000);
	GPIO_Write(GPIOB, 0X0000);
	GPIO_Write(GPIOC, 0X0000);
	GPIO_Write(GPIOD, 0X0000);
	GPIO_Write(GPIOE, 0X0000);
	
}


void SleepPortUninit(u32 *confReg,u16 *datReg)
{
	confReg[0] = GPIOA->CRL;
	confReg[1] = GPIOA->CRH;
	confReg[2] = GPIOB->CRL;
	confReg[3] = GPIOB->CRH;
	confReg[4] = GPIOC->CRL;
	confReg[5] = GPIOC->CRH;

	confReg[6] = GPIOD->CRL;
	confReg[7] = GPIOD->CRH;

	confReg[8] = GPIOE->CRL;
	confReg[9] = GPIOE->CRH;

	datReg[0] = GPIOA->ODR;
	datReg[1] = GPIOB->ODR; 
	datReg[2] = GPIOC->ODR;

	datReg[3] = GPIOD->ODR;
	datReg[4] = GPIOE->ODR;


	PortUninit();			//sleep2

}

void SleepPortInit(u32 *confReg,u16 *datReg)
{

	GPIOA->CRL = confReg[0];
	GPIOA->CRH = confReg[1];

	GPIOB->CRL = confReg[2];
	GPIOB->CRH = confReg[3];

	GPIOC->CRL = confReg[4];
	GPIOC->CRH = confReg[5];

	GPIOD->CRL = confReg[6];
	GPIOD->CRH = confReg[7];
	GPIOE->CRL = confReg[8];
	GPIOE->CRH = confReg[9];	


	GPIOA->ODR = datReg[0];
	GPIOB->ODR = datReg[1];
	GPIOC->ODR = datReg[2];

	GPIOD->ODR = datReg[3];
	GPIOE->ODR = datReg[4];
	

}


void SleepWakeIntSet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//       EXTI_InitTypeDef EXTI_InitStructure;
//       NVIC_InitTypeDef NVIC_InitStructure;
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
       GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA1
//       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
       //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//       GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//PC9
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //pull down
       GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA15
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //pull down

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	Ex_NVIC_Config(GPIO_A,0,FTIR);		//PA0
	//Ex_NVIC_Config(GPIO_A,1,FTIR);		//
	//Ex_NVIC_Config(GPIO_C,9,FTIR);		//PC9
	Ex_NVIC_Config(GPIO_A,15,RTIR);         //PA15
	//EXTIx_Init(GPIO_A,RTIR);
	//Ex_NVIC_Config(GPIO_A,5,RTIR);

	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);
	MY_NVIC_Init(2,1,EXTI0_IRQn,1);
	//MY_NVIC_Init(2,2,EXTI1_IRQChannel,1);
	//MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,1);
	//SetPwrOnExti();

}



#if(_HARD_WARE_==HARD_WARE_RFONLY)

void LowPow_Init(void)
{
	u32 confRegRsv[10]={0};
	u16 datRegRsv[5] = {0};
	
	//BkLiteAutoOff();					//SleepPortUninit
	//BK_TURNOFF(0);
	//KEY_BK_OFF();
	
	
	//DISABLE_CHARGE();				//SleepPortUninit
	//LCD_Clear(0);
	//LCD_WRITE_CMD(0XAF);
	//DisplayOff();

	//PortUninit();	

	//PortUninit();
	BkLiteAutoOff();
	DISABLE_KEY_INT();
	DISABLE_FLASH_INT();
	DISABLE_SYN_INT();
	//Ex_NVIC_Disable(5);
	//Ex_NVIC_Disable(4);
	g_SettingIndex = 0;
	g_chargeOk = 0;						//2015/08/10  to avoid twinkle when waked up
	memset(radioStatue.devCnt,0x00,5);
	
	//CHARGE_LED1_OFF;
	//CHARGE_LED2_OFF;
	CHARGE_LED_OFF;
	//FLAOK_OFF();
	
	//BOOST_EN = 1;
	//D3V3_EN = 0;

	//g_comStatue = COM_STATUE_SLEEP;
	//LCD_RST = 0;					//SleepPortUninit

//for test
//	BOOST_EN = 0;
//	D3V3_EN = 1;
/**************************************
for test enable RF module to sleep
***************************************/
//	DISABLE_RF_MODULE();			//SleepPortUninit
//	PERIPHERAL_PWR_OFF();
/**************************************
end
***************************************/
//	ADC1->CR2 &= ~(1<<0);			//turn off adc
//	ADC_DeInit(ADC1);
	ADC1->CR2 &= ~(1<<0);			//turn off adc
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
	//SleepPortUninit();
	//SPI_I2S_DeInit(SPI2);
//	SPIx_Disable();
//	PERIPHERAL_PWR_OFF();					//before the SleepPortUninit, so when wake up, the power is off, after pilot is released, the power up, in order to avoiding cc2530 jump to boot
	SleepPortUninit(confRegRsv,datRegRsv);
//#ifdef SLEEP_PWR_OFF
	//D3V3_EN = 1;
	//BOOST_EN = 0;
//this is where should pay special attention, after ADC_DeInit,  OSTimeDlyHMSM will cause task switched, than 
//the keyprocess and voltage check will excuted, but the ADC is closed, so the task will cause wait perminately
	//delay_ms(10);		
	//OSTimeDlyHMSM(0,0,0,10);
	//OSTimeDlyHMSM(0,0,0,10);
//#endif	
       PERIPHERAL_PWR_OFF();
/**************************************************/

	SleepWakeIntSet();				//sleep1
	//GPIOA->CRL&=0XFF0FFF00;//PA0设置成输入	  
	//GPIOA->CRL|=0X00800088; 

	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;

	//Ex_NVIC_Config(GPIO_A,0,FTIR);		//PA0
	//Ex_NVIC_Config(GPIO_A,1,FTIR);		//
	//Ex_NVIC_Config(GPIO_C,9,FTIR);		//PC9
	//Ex_NVIC_Config(GPIO_A,15,RTIR);         //PA15
	//EXTIx_Init(GPIO_A,RTIR);
	//Ex_NVIC_Config(GPIO_A,5,RTIR);

	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);
	//MY_NVIC_Init(2,2,EXTI1_IRQChannel,1);
	//MY_NVIC_Init(2,2,EXTI9_5_IRQChannel,1);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);

	//SetPwrOnExti();
	//SleepWakeIntSet();
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
	delay_ms(10);	
	
	g_comStatue = COM_STATUE_SLEEP;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	RCC_Configuration();

	//while(!PILOT_IN);				//avoid unexpected flash

	//HaltSec(100);

	Ex_NVIC_Disable(0);
	//Ex_NVIC_Disable(1);
	//Ex_NVIC_Disable(9);
	//Ex_NVIC_Disable(13);
	Ex_NVIC_Disable(15);
	ENABLE_FLASH_INT();
	//ENABLE_SYN_INT();
	//Timer2_Init(40,72);
	//g_tmHandleFlag = TM_HANDLE_SHUT;

	while(!PILOT_IN);	

//	Adc_Init();

	//if(KEY_POW_OFF>0)
	if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
		//SleepPortUninit();
		//SleepPortUninit(confRegRsv,datRegRsv);
//#ifdef SLEEP_PWR_OFF
//		D3V3_EN = 0;
//		BOOST_EN = 1;
//#endif	
/**************************************************/
	
		ENABLE_KEY_INT();

	/*******************************************************/
	//	port initialize
	/*******************************************************/
	//	PortInit();				//2015/08/21
		SleepPortInit(confRegRsv,datRegRsv);

		PERIPHERAL_PWR_ON();

		//OSTimeDlyHMSM(0,0,0,1);
		delay_ms(1);

		//Com_Stop();

		//if(COM_STATUE_START==g_comStatue)
		//if(CLK_IN>0)
		if(PAin(15)>0)
		{
			EXTIx_Init(FTIR);
			//OSTimeDlyHMSM(0,0,0,1);
			delay_ms(1);
			SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
			g_cmrLinked = 1;
			g_comStatue = COM_STATUE_START;
		}else
		{
			EXTIx_Init(RTIR);
			SPI_MISO_AUX  = 0;
			COM_AUX = 0;
			g_cmrLinked = 0;
			g_comStatue = COM_STATUE_IDLE;
		}
	/*******************************************************/

	//for test
	//	BOOST_EN = 1;
	//	D3V3_EN = 0;

/**********************************************************/
	//	ADC1->CR2|=(1<<0);
	//	Adc_Init();
		ADC1->CR2|=(1<<0);

/**********************************************************/

		LCD_Init();

		LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		//Mode(mainPara.workMode);

#if 0
#ifdef RF_LOWPWR_SLEEP
		CC2530_RESET = 0;
		delay_ms(400);
		CC2530_RESET = 1;
#else
/**************************************
for test enable RF module to sleep
***************************************/
		DISABLE_RF_MODULE();			//SleepPortUninit
/**************************************
end
***************************************/
		delay_ms(400);
#endif
#endif

		while(!PILOT_IN);	

		//if(KEY_POW_OFF>0)
		//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
		{
			RfMoudleSet();
			g_cc2530Wake = 1;
		}

		OSTimeDlyHMSM(0,0,0,20);

		//SET_UPDATE_EVT(DSP_UPDATE_RFCHAN);
		//SET_UPDATE_EVT(DSP_UPDATE_RFID);
		//SET_UPDATE_EVT(DSP_UPDATE_RFSW);
		BkLiteOn();
		//SET_UPDATE_EVT(DSP_UPDATE_PWON);
		SET_UPDATE_EVT(DSP_UPDATE_RFINFO);
	}
	//OSTaskSuspend(CMDSEND_TASK_Prio);

	//BOOST_EN = 1;
	//BOOST_EN = 0;
	//D3V3_EN = 0;
//	g_expAccHotWarnValue = 0;
	

	//PILOT_IN    PA1 PA0 PA5
	//EXTIx_Init(GPIO_A,RTIR);				//PA5 SPI_CLK
	//Ex_NVIC_Config(GPIO_A,0,RTIR); //上升沿触发	
	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //抢占2，子优先级2，组2
}
#else
void LowPow_Init(void)
{
	//set wakeup pin
	//g_sleepFlag = 1;
/*
	unsigned long regRsvAL;
	unsigned long regRsvAH;
	unsigned long regRsvBL;
	unsigned long regRsvBH;
	unsigned long regRsvCL;
	unsigned long regRsvCH;
	unsigned long regRsvDL;
*/
	
	
	//ADC1->CR2 &= ~1;			//turn off adc
	//BK_LIGHT = 0;
	//ADC1->CR2&=~(1<<0);	//disable ADC
#if 1
	//RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	//RCC->APB2ENR|=1<<5;		 //使能PORTD时钟

	//JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_SLEEP,0);
	//while(uart_tran_flag);

	//CHARGE_LED_OFF;
	//FLAOK_OFF;

	if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
		||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		DISABLE_RF_MODULE();
	}
	
	DISABLE_CHARGE;
	//ADC1->CR2&=~(1<<0);	//disable ADC

	BkLiteAutoOff();	
	LCD_StandBy();

	UART2_LOCK();

	POW_ON = 0;		//turn off the power supply

	//GPIOA->CRL&=0XFFFFFFF0;//PA0设置成输入	  
	//GPIOA->CRL|=0X00000008;   
	//GPIOA->CRH&=0X0F0FFFFF;//PA13,15设置成输入	  
	//GPIOA->CRH|=0X80800000; 				   
	//GPIOA->ODR|=1<<13;	   //PA13上拉,PA0默认下拉
	//GPIOA->ODR|=1<<15;	   //PA15上拉

	//GPIOA->CRL&=0XFF0FFFFF;//PA0设置成输入	  
	//GPIOA->CRL|=0X00800000;
	//GPIOA->ODR|=1<<5;

/*
	regRsvAL = GPIOA->CRL;
	regRsvAH = GPIOA->CRH;

	regRsvBL = GPIOB->CRL;
	regRsvBH = GPIOB->CRH;

	regRsvCL = GPIOC->CRL;
	regRsvCH = GPIOC->CRH;

	regRsvDL = GPIOD->CRL;

	GPIOA->CRL = 0x88888888;
	GPIOA->CRH = 0x88888888;

	GPIOB->CRL = 0x88888888;
	GPIOB->CRH = 0x88888888;

	GPIOC->CRL = 0x88888888;
	GPIOC->CRH = 0x88888888;

	GPIOD->CRL = 0x88888888;
*/


	Ex_NVIC_Config(GPIO_A,0,FTIR); //上升沿触发
	DISABLE_DIRINT();
	//Ex_NVIC_Config(GPIO_A,13,FTIR);//下降沿触发
	//Ex_NVIC_Config(GPIO_A,15,FTIR);//下降沿触发
	//Ex_NVIC_Config(GPIO_A,5,FTIR);
	
	//MY_NVIC_Init(2,1,EXTI9_5_IRQChannel,2); 
	MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //抢占2，子优先级2，组2
	//MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//抢占2，子优先级1，组2	   
	OSTimeDlyHMSM(0,0,0,50);
	
	CHARGE_LED_OFF;				//in light mode, if it is set before, it will turn on the LED
	FLAOK_OFF();			
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//Sys_Standby();

	RCC_Configuration();

/*	GPIOA->CRL = regRsvAL;
	GPIOA->CRH = regRsvAH;

	GPIOB->CRL = regRsvBL;
	GPIOB->CRH = regRsvBH;

	 GPIOC->CRL = regRsvCL;
	GPIOC->CRH = regRsvCH;

	GPIOD->CRL = regRsvDL;
*/

	Ex_NVIC_Disable(0);
	ResetKeyInt();

/******************************************************************************************************
//	send rf parameter
******************************************************************************************************/
	//RfParaSend();
	//PortInit();
	while(!PILOT_IN);
	//iap_load_app(FLASH_APP1_ADDR);

	//OSTimeDlyHMSM(0,0,0,50);
	//GPIOC->CRL&=0xFF0FFFFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, COM_AUX
	//GPIOC->CRL|=0x00300000;

	//OSTimeDlyHMSM(0,0,0,100);

	POW_ON = 1;	
	
	//ADC1->CR2|=(1<<0);		//enable ADC
	
	if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
		||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		ENABLE_RF_MODULE();
		memset(radioStatue.devCnt,0x00,5);
		g_cc2530Wake = 1;
		//DISABLE_CHARGE;
	}else
	{
		ENABLE_CHARGE;
	}

	UART2_UNLOCK();
	LCD_StandByExit();
	LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);
	//CmdModDisplay();
	BkLiteOn();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	//ADC1->CR2|=(1<<0);
	//Adc_Init();
	

	
#endif


	//Sys_Standby();
	//Sys_StopMode();
	//WFI_SET();

	
}
#endif

void EXTI0_IRQHandler(void)
{
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
}

#if 0
void EXTI15_10_IRQHandler()
{
/*
	WFI_SET();
	EXTI->PR=1<<14;
	Ex_NVIC_Disable(14);
	ADC1->CR2 |= 0x01;
*/
	//CHARGE_LED = !CHARGE_LED;
	//Ex_NVIC_Disable(14);
	//BK_LIGHT = !BK_LIGHT;
#if(_HARD_WARE_==HARD_WARE_VER3)
	g_keyDir = 1;
	Ex_NVIC_Disable(10);
	EXTI->PR=1<<10;  //清除LINE0上的中断标志位 
#else
	EXTI->PR=1<<14;
#endif
	//Sys_Soft_Reset();
	//iap_load_app(FLASH_APP1_ADDR);
	
	
}
#endif


////check ok
////091202
//void BKP_Write(u8 reg,u16 dat)
//{
//  RCC->APB1ENR|=1<<28;     //使能电源时钟	    
//	RCC->APB1ENR|=1<<27;     //使能备份时钟	    
//	PWR->CR|=1<<8;           //取消备份区写保护 
//	switch(reg)
//	{
//		case 1:
//			BKP->DR1=dat;
//			break;
//		case 2:
//			BKP->DR2=dat;
//			break;
//		case 3:
//			BKP->DR3=dat;
//			break; 
//		case 4:
//			BKP->DR4=dat;
//			break;
//		case 5:
//			BKP->DR5=dat;
//			break;
//		case 6:
//			BKP->DR6=dat;
//			break;
//		case 7:
//			BKP->DR7=dat;
//			break;
//		case 8:
//			BKP->DR8=dat;
//			break;
//		case 9:
//			BKP->DR9=dat;
//			break;
//		case 10:
//			BKP->DR10=dat;
//			break;
//	} 
//}	    
//系统软复位
//CHECK OK
//091209
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 

//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;
//CHECK OK	
//100818		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
} 
//系统时钟初始化函数
//pll:选择的倍频数，从2开始，最大值为16	
//CHECK OK
//091209
void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //复位并配置向量表
	RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	while(!(RCC->CR>>17));//等待外部时钟就绪
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//抵消2个单位
	RCC->CFGR|=PLL<<18;   //设置PLL值 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2个延时周期

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟	 
	while(temp!=0x02)     //等待PLL作为系统时钟设置成功
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}    
}

__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

iapfun jump2app; 
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}else
	{
		printf("start flash error\r\n");
	}
}	




























