#include "sys.h"
//#include"global.h"

#pragma optimize=none
void Delayus(void)   
{    
    asm("nop"); //һ��asm("nop")��������ʾ�������Դ���100ns   
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
  
//---- ���뼶��ʱ����-----------------------   
void delay_ms(u32 time)   
{   
    u32 i;   
    while(time--)     
    for(i=900;i>0;i--)   
    Delayus();    
}

u16 get_counter(void)
{
	u16 tim2;
	tim2 = (u16)TIM5->CNTRH << 8;
	tim2 |= TIM5->CNTRL;
	return tim2;
}



