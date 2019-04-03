//#include <stm32f10x_lib.h>		 
#include "stm32f10x.h"
#include "sys.h"
#include "../../user/golbal.h" 
#include "../../user/Display.h" 
#include "../../HARDWARE/STMFLASH/stmflash.h"
#include "../../HARDWARE/lcd/Lcd.h"
#include "usart.h"
#include "../../user/canon.h"



//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����
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
  	//�������Ϸ���
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 
//CHECK OK
//091209
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����
//CHECK OK
//100329
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u32 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	u8 IPRADDR=NVIC_Channel/4;  //ÿ��ֻ�ܴ�4��,�õ����ַ 
	u8 IPROFFSET=NVIC_Channel%4;//�����ڵ�ƫ��
	IPROFFSET=IPROFFSET*8+4;    //�õ�ƫ�Ƶ�ȷ��λ��
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//ȡ����λ

	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK)
	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);    
	//NVIC->IPR[IPRADDR]|=temp<<IPROFFSET;//������Ӧ���ȼ����������ȼ�   	    	  				   
	NVIC->IP[IPRADDR]|=temp<<IPROFFSET;//������Ӧ���ȼ����������ȼ�   	    	  				   
}

//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��USB����������
//����:GPIOx:0~6,����GPIOA~G;BITx:��Ҫʹ�ܵ�λ;TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   
//������...
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
	EXTOFFSET=(BITx%4)*4;

	RCC->APB2ENR|=0x01;//ʹ��io����ʱ��

	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx
	
	//�Զ�����
	//EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
	//EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
 	//if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
	//if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
	//not active rasing eadge and falling eadge at the same time
	if(DU_TIR==(TRIM&0x03))
	{
		EXTI->FTSR|=1<<BITx;
		EXTI->RTSR|=1<<BITx;
	}else if(TRIM&0x01)
	{
		EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
		EXTI->RTSR&=~(1<<BITx);
 	}else if(TRIM&0x02)
	{
		EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
		EXTI->FTSR&=~(1<<BITx);
	}

	EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�

} 

void Ex_NVIC_Enable(u8 BITx)
{
	EXTI->IMR |= 1<<BITx;
}

void Ex_NVIC_Disable(u8 BITx)
{
	EXTI->IMR &= ~(1<<BITx);
}


//����������ִ���������踴λ!�����������𴮿ڲ�����.		    
//������ʱ�ӼĴ�����λ
//CHECK OK
//091209
void MYRCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000;//��λ����			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR = 0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
  	RCC->APB2ENR = 0x00000000; //����ʱ�ӹر�.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //��λHSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //��λHSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //�ر������ж�
	//����������				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else   
	//MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x5000);	 
#endif
}
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI
//CHECK OK
//091209
__asm void WFI_SET(void)
{
	WFI;    
}
//�������ģʽ	 
//check ok 
//091202
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//ʹ��SLEEPDEEPλ (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
 	PWR->CSR|=1<<8;          //����WKUP���ڻ���
	PWR->CR|=1<<2;           //���Wake-up ��־
	PWR->CR|=1<<1;           //PDDS��λ		  
	WFI_SET();				 //ִ��WFIָ��		 
}	 

void Sys_StopMode()
{
	//g_sleepFlag = 1;
	SCB->SCR|=1<<2;
	RCC->APB1ENR|=1<<28;
	PWR->CR |= 1<<2;
	PWR->CR &= 0xFD;
	PWR->CR |= 0x01;		//��ͣ��ģʽ�µ�ѹ��ѹ�����ڵ͹���ģʽ
	WFI_SET();
} 



void RCC_Configuration(void)
{
	/* RCC system reset(for debug purpose) */
	/* ��λ���е�RCC��Χ�豸�Ĵ��������ı��ڲ�������������λ��HSITRIM[4..0]���Լ�
	��������ƼĴ�����RCC_BDCR��,����״̬�Ĵ���RCC_CSR */
	RCC_DeInit();

	/* Enable HSE */
	/* ����HSE���� */
	/* ��������
	RCC_HSE_ON ����
	RCC_HSE_OFF �ر�
	RCC_HSE_BYPASS ʹ���ⲿʱ������
	*/
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	/* HSEStartUpStatusΪö�����ͱ�����2��ȡֵ��0ΪERROR����0ΪSUCCESS
	�ȴ�HSE׼���ã�����ʱʱ�䵽���˳�*/
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();

	/* ��HSE׼����������ȶ��� */
	if(RCC_WaitForHSEStartUp()== SUCCESS)
	{
	/* HCLK = SYSCLK */
	/* ����AHBʱ�ӣ����ʱ�Ӵ�SYSCLK��Ƶ����
	��Ƶϵ����1��2��4��8��16��64��128��256��512
	*/
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	/* ���õ���APB2ʱ�ӣ����ʱ�Ӵ�AHBʱ�ӷ�Ƶ����
	��Ƶϵ��Ϊ1��2��4��8��16 */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK/2 */
	/* ���õ���APB1ʱ�ӣ����ʱ�Ӵ�AHBʱ�ӷ�Ƶ����
	��Ƶϵ��Ϊ1��2��4��8��16 */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	/* PLLCLK = 8MHz * 9 = 72 MHz */
	/* ����PLL��ʱ��Դ�ͳ˷�����
	��һ����ڲ���Ϊʱ��Դ������3��
	RCC_PLLSource_HSI_Div2
	RCC_PLLSource_HSE_Div1
	RCC_PLLSource_HSE_Div2
	�˷�����RCC_PLLMul_2~RCC_PLLMul_16��֮���������
	*/
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	/* Enable PLL */
	/* �������ΪENABLE��DISABLE */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source */
	/*ѡ��ϵͳʱ��Դ����������
	RCC_SYSCLKSource_HSI �ڲ���������
	RCC_SYSCLKSource_HSE �ⲿ��������
	RCC_SYSCLKSource_PLLCLK PLLʱ��
	*/
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	/* ����ϵͳ��ǰ��ʱ��Դ
	����ֵ��3��
	0x00 HSI�ǵ�ǰʱ��Դ
	0x04 HSE�ǵ�ǰʱ��Դ
	0x08 PLL�ǵ�ǰʱ��Դ
	*/
	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
	}

	/* Enable GPIOB, GPIOC and AFIO clocks */
	/* ʹ�ܵ�������APB2�ϵ���Χ�豸
	�������IO�˿�B��ʱ��
	AFIO����ӳ�书�ܵ�ʱ�ӣ�һ��δʹ��REMAP����ʱ����ʱ�Ӳ��ؿ���
	���������UM0472 PAGE 426
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
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA1
//       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
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
	//GPIOA->CRL&=0XFF0FFF00;//PA0���ó�����	  
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
	//Ex_NVIC_Config(GPIO_A,0,RTIR); //�����ش���	
	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //��ռ2�������ȼ�2����2
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
	//RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	//RCC->APB2ENR|=1<<5;		 //ʹ��PORTDʱ��

	//JTAG_Set(JTAG_SWD_DISABLE);//�ر�JTAG��SWD   

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

	//GPIOA->CRL&=0XFFFFFFF0;//PA0���ó�����	  
	//GPIOA->CRL|=0X00000008;   
	//GPIOA->CRH&=0X0F0FFFFF;//PA13,15���ó�����	  
	//GPIOA->CRH|=0X80800000; 				   
	//GPIOA->ODR|=1<<13;	   //PA13����,PA0Ĭ������
	//GPIOA->ODR|=1<<15;	   //PA15����

	//GPIOA->CRL&=0XFF0FFFFF;//PA0���ó�����	  
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


	Ex_NVIC_Config(GPIO_A,0,FTIR); //�����ش���
	DISABLE_DIRINT();
	//Ex_NVIC_Config(GPIO_A,13,FTIR);//�½��ش���
	//Ex_NVIC_Config(GPIO_A,15,FTIR);//�½��ش���
	//Ex_NVIC_Config(GPIO_A,5,FTIR);
	
	//MY_NVIC_Init(2,1,EXTI9_5_IRQChannel,2); 
	MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //��ռ2�������ȼ�2����2
	//MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//��ռ2�������ȼ�1����2	   
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
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ  
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
	EXTI->PR=1<<10;  //���LINE0�ϵ��жϱ�־λ 
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
//  RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
//	RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
//	PWR->CR|=1<<8;           //ȡ��������д���� 
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
//ϵͳ��λ
//CHECK OK
//091209
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 

//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;
//CHECK OK	
//100818		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
} 
//ϵͳʱ�ӳ�ʼ������
//pll:ѡ��ı�Ƶ������2��ʼ�����ֵΪ16	
//CHECK OK
//091209
void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //��λ������������
	RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//����2����λ
	RCC->CFGR|=PLL<<18;   //����PLLֵ 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��	 
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
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
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}else
	{
		printf("start flash error\r\n");
	}
}	




























