/*******************************************************************************
*******************************CANON_NIKON_IN_ONE*******************************
********************************************************************************
*******************************************************************************/
#include"iostm8l152r8.h"
#include "aes\aes.h"
#include "hw\uart\uart.h"
#include "hw\eeprom\eeprom.h"
#include "hw\boot\boot.h"

#define PILOT_PIN  PE_IDR_IDR0

__root const long reintvec[]@".intvec"=
{   
   0x82008080,0x8200A004,0x8200A008,0x8200A00c, //当应用程序地址不是0x9000时则要相应改掉除第一个0x82008080以外的数值
   0x8200A010,0x8200A014,0x8200A018,0x8200A01c,
   0x8200A020,0x8200A024,0x8200A028,0x8200A02c,
   0x8200A030,0x8200A034,0x8200A038,0x8200A03c,
   0x8200A040,0x8200A044,0x8200A048,0x8200A04c,
   0x8200A050,0x8200A054,0x8200A058,0x8200A05c,
   0x8200A060,0x8200A064,0x8200A068,0x8200A06c,
   0x8200A070,0x8200A074,0x8200A078,0x8200A07c,
};

void PrintBuffer(u8 * buf, u8 len)
{
  u8 i = 0;
  for (i = 0; i < len; i++)
  {
    printf("%x ", buf[i]);
  }
  printf("\r\n");
}

void JumpToApp( void )
{
  asm("LDW X,  SP ");
  asm("LD  A,  $FF");
  asm("LD  XL, A ");
  asm("LDW SP, X ");
  asm("JPF $A000");
}

void Delayus(void)   
{    
    asm("nop"); //一个asm("nop")函数经过示波器测试代表100ns   
    asm("nop");   
    asm("nop");   
    asm("nop");    
}   
  
//---- 毫秒级延时程序-----------------------   
void delay_ms(unsigned int time)   
{   
    unsigned int i;   
    while(time--)     
    for(i=900;i>0;i--)   
    Delayus();    
}

void FlashUnlock(void)
{
	//_asm("sim");
	
	//FLASH_CR2 = 1<<0;//打开块编程
	//FLASH_NCR2 = ~(1<<0);//打开块编程

        FLASH_CR2 = (1<<6);
  
	FLASH_PUKR = 0x56; 
	FLASH_PUKR = 0xAE; 
	FLASH_PUKR = 0x56; 
	FLASH_PUKR = 0xAE; 	
	//FLASH_CR2 = 1<<0;//打开块编程
	//FLASH_NCR2 = ~(1<<0);//打开块编程
}

static void Init_OSC(void)
{
    
    CLK_HSITRIMR|=0x04;
    while(!(CLK_ICKCR & 0x02));
    CLK_CKDIVR = 0;
    ITC_SPR2 = 0;	
}

static void Init_Gpio(void)
{
     PE_CR1 |= 0x01;     //
}

void main( void )
{
   //在此编写您的代码  
    //u8 readBuf[16];
    asm("sim");
    Init_OSC();
    Init_USART1();
    Init_Gpio();
    //u8 i = 0;
    //u8 iqTms = 0;
    
    //printf("boot start\r\n");
    
    /*
    printf("aes boot start\r\n");
    Boot_Decrypt(g_src,g_decKey,g_src,16);
    PrintBuffer(g_src,16);
    Boot_Encrypt(g_src,g_decKey,g_src,16);
    PrintBuffer(g_src,16);
    printf("aes boot end\r\n");
    
    printf("Eeprom test start:");
    EEPROM_Read(0,readBuf,16);
    PrintBuffer(readBuf,16);
    printf("Eeprom write\r\n");
    EEPROM_Write(0,g_src,16);
    */
/*    
    while(1)
    {
        putchar(0xA8);
        
        delay_ms(10);
        
        if(USART1_SR & (1 << 5))
        {
            if( 0x57 == USART1_DR )
            {
                break;
            }
        }
        
        iqTms ++;
        
        if (iqTms > 8)
        {
            JumpToApp();
        }
    }*/
#if 1  
    // 2017/07/19
    if ( PILOT_PIN )
    {
         printf("Jump\r\n");
         JumpToApp();
    }
#endif
    
    printf("boot start\r\n");
    
    FlashUnlock();
    
    // no return
    BootProc();
}

/*******************************************************************************
*******************************蓝旗嵌入式系统***********************************
*************************http://lenchimcu.taobao.com****************************
*******************************************************************************/