#include "sys.h"
//#include"global.h"

#pragma optimize=none
void Delayus(void)   
{    
    asm("nop"); //一个asm("nop")函数经过示波器测试代表100ns   
    asm("nop");   
    asm("nop");   
    asm("nop");    
}   

void delay_us(u16 us)
{
    while (us--)
    {
        Delayus();
    }
}
  
//---- 毫秒级延时程序-----------------------   
void delay_ms(u32 time)   
{   
    u32 i;   
    while(time--)     
    for(i=900;i>0;i--)   
    Delayus();    
}

u16 get_counter(void)
{
	//u16 tim2;
	//tim2 = (u16)TIM5->CNTRH << 8;
	//tim2 |= TIM5->CNTRL;
	return TIM5_GetCounter();
}

u8 memcmp(u8* src, u8 *dest, u8 len)
{
	u8 i = 0;
	for (i=0;i<len;i++)
	{
		if (src[i]!=dest[i])
		{
			return (i+1);
		}
	}

	return 0;
}




