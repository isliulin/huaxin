#include "timer.h"
#include"global.h"
#include <stdarg.h>
#include "..\spi\spi.h"

u8 g_tmHandleFlag = 0;

void Timer2Init(void)
{
	CLK_PCKENR1 |= 0x01; 
	//TIM2_CR1 = 0x00;
	TIM2_CR2 = 0x00;
	//TIM2_IER |= 0x02;                     // Capture/compare 1 interrupt enable
	TIM2_PSCR = 0x04;                   
	//TIM2_ARRH = 0xFF;                    // use default
	//TIM2_ARRH = 0xFF;                    // use default
	TIM2_CR1 |= 0x01;
}

void Timer1Init(void)
{
	// set ch1 as pwm output
	
	  CLK_PCKENR2 |= 0x02;          // timer clock

	  //TIM1_CCMR2 |= 0x70;
	  TIM1_CCMR1 |= 0x70;
	  
	  //TIM1_CCER1 |= 0x30;
	  TIM1_CCER1 |= 0x06;
	  //TIM1_CCER1 |= 0x03;

	  // auto reload
	  TIM1_ARRH = 0x00;
	  TIM1_ARRL = 25;
	              
	  TIM1_CCR1H = 0x00;
	  TIM1_CCR1L = 13;

          //	Prescaler value (MSB)		
	  TIM1_PSCRH = 0x00;
	  TIM1_PSCRL = 15;

	  //
	  //TIM1_OISR |= 0x03;
		
	  TIM1_CR1 |= 0x01;

	  //TIM1_BKR |= 0x8C;
	  TIM1_BKR |= 0x80;

}

void Timer4Init(void)
{
    CLK_PCKENR1 |= 0x04; 
    TIM4_EGR = 0x01;    
    TIM4_PSCR = 0x07;                           // 8m
    //TIM4_CNTR = arr;
    //TIM4_ARR = 255;
    //TIM4_CR1 = 0x01;
    TIM4_CNTR = 0x01;
    TIM4_IER = 0x01;
    TIM4_CR1 = 0x09;                              // generate a interrupt in case the initialized cnt is not 0
}

void CanResetMod(void)
{
	if (!g_canCmrLink)
	{
		g_canCmrLink = 1;
		SwRfMode();
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}

#pragma vector=TIM4_UIF_vector
__interrupt void TIM4_UIF_IRQHandler(void)
{

        // TIM4_SR1 = 0x00; //清中断标志
#ifdef SLEEP_DBG
        //printf("tm4");
#endif

	 if (TIM4_SR1 & 0x01)
	 {
/*
	        if (TM_HANDLE_REAR == g_tmHandleFlag)
	        {
	        	SET_SPIO_ON ( );
			SPI_MISO_AUX ( 1 );	
	        }else if(TM_HANDLE_SHUT == g_tmHandleFlag)
	        {
			UnlinkCmrHandle();
		}
		Timer4Stop();	
		g_tmHandleFlag = TM_HANDLE_CLR;
*/
		if ( g_tmHandleFlag & TM_HANDLE_REAR )
		{
			SetSpioOn();
			CanResetMod();
			SPI_MISO_AUX ( 1 );	
			g_tmHandleFlag^=TM_HANDLE_REAR;
		}

		if ( g_tmHandleFlag & TM_HANDLE_SHUT )
		{
			//TEST_PIN ^= 1;
			//TEST_PIN ^= 1;
			WaitCmrHandle();
			g_tmHandleFlag^=TM_HANDLE_SHUT;
		}

		if ( g_tmHandleFlag & TM_nHANDLE_COMRST ) 
		{
			//TEST_PIN ^= 1;
			NikTmComRst();
			g_tmHandleFlag^=TM_nHANDLE_COMRST;
		}
/*
		if (g_tmHandleFlag & TM_nHANDLE_COMTIMEOUT)
		{
			NikTmComTimeout();
			g_tmHandleFlag^=TM_nHANDLE_COMTIMEOUT;
		}
*/
		
	 }

	 TIM4_SR1 = 0x00; //清中断标志
	 //Timer4Stop();                          //why delete? one shot mode

/*
       TIM4_SR1 = 0x00; 
       PF_ODR_ODR1 = 1;
	Timer4Stop();
	g_tmCtrl = 0;
       //Timer4Start(10);
       */
}



