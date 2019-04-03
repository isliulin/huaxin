#include "includes.h"

_ADC_VALUE  ADC_VALUE;
uint16_t g_volRef = 0;

void TIM5_Config(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE);
    TIM5_TimeBaseInit(TIM5_Prescaler_8, TIM5_CounterMode_Up,63);
    TIM5_ClearFlag(TIM5_FLAG_Update);
    //  TIM5_ITConfig(TIM5_IT_Update, ENABLE);
    TIM5_OC1Init(TIM5_OCMode_PWM1,
                 TIM5_OutputState_Enable,
                 32,
                 TIM5_OCPolarity_High,
                 TIM5_OCIdleState_Reset);
    TIM5_Cmd(ENABLE); 
}
/*************************************************************************************
* FunctionName   : filter()
* Description    : ADC滤波
* EntryParameter : 冒泡法
* ReturnValue    : None
**************************************************************************************/
#define N  11 
uint16_t filter(uint8_t channel)  
{  
    uint16_t value_buf1[N] = {0}; 
    uint8_t count,i,j;
    uint16_t temp;   
    
    for ( count=0;count<N;count++)  
    { 
        value_buf1[count] = ADC_Buffer[channel];  
    }
    
    for (j=0;j<N-1;j++)  
    {  
        for (i=0;i<N-j-1;i++)  
        {  
            if ( value_buf1[i]>value_buf1[i+1] )  
            {  
                temp = value_buf1[i];  
                value_buf1[i] = value_buf1[i+1];   
                value_buf1[i+1] = temp;  
            } 
        }  
    }     
    return (value_buf1[(N-1)/2]);
}
/*************************************************************************************
* FunctionName   : Change_Cap()
* Description    : 充电
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Change_Cap(void)           //10ms调用一次
{
    ADC_VALUE.Channel_CAP_VALUE = filter(4);
    g_volRef = ADC_VALUE.Channel_CAP_VALUE;
    
    if((Motor_Manger_TypeDef.Motor_Move_State == Motor_Move_End) &\
        (SysInfo.Change_Cap_En_flag == 1)&\
        (SysInfo.Low_Battery_flag == 0))
    {
        if(ADC_VALUE.Channel_CAP_VALUE > CAP_330V_VALUE)
        {
            CHANGE_CAP_STOP;
        }
        else if(ADC_VALUE.Channel_CAP_VALUE < CAP_330V_VALUE-100)
        {
            if(ADC_VALUE.Channel_CAP_VALUE < CAP_170V_VALUE)
            {
                GPIO_H_CHANGE_DISABLE;
            }
            else if(ADC_VALUE.Channel_CAP_VALUE > CAP_170V_VALUE+20)
            {
                GPIO_H_CHANGE_ENABLE;
            }
            CHANGE_CAP_START;
        }        
    }
    else
    {
        CHANGE_CAP_STOP;
    }
}

/*************************************************************************************
* FunctionName   : Low_Batt_Check()
* Description    : 电池电量检测
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
uint8_t BEEP_CAP_ONCE =0;
void Low_Batt_Check(void)
{
    static uint16_t Cap_Value_Rem1,Cap_Value_Rem2;
    
    uint16_t Cap_Value; 
    
    Cap_Value = ADC_VALUE.Channel_CAP_VALUE;
    
    if(Cap_Value > CAP_275V_VALUE)                          //回电指示
    {
        CAP_LED_RED;
        SysInfo.Cap_Full_Flag = 1;                          //电容满电标志
        if(BEEP_CAP_ONCE)
        {
            BEEP_CAP_ONCE = 0;
            BeepFlag.Beep_CapFull_Flag = 1;                  //回电音
        }
    }
    else if(Cap_Value < (CAP_275V_VALUE-100))
    {        
        CAP_LED_GREEN;
        SysInfo.Cap_Full_Flag = 0;
        BEEP_CAP_ONCE = 1;
    }        
    
    if((Cap_Value < CAP_275V_VALUE) & \
        (SysInfo.Change_Cap_En_flag)& \
        (Motor_Manger_TypeDef.Motor_Move_State == Motor_Move_End))
    {
        SysInfo.Cap_Value_Count++;  
    
        switch(SysInfo.Cap_Value_Count)
        {
          case  10: 
            {
                Cap_Value_Rem1 = Cap_Value;
            }break;
          case  20: 
            {
                SysInfo.Cap_Value_Count = 0;
                
                Cap_Value_Rem2 = Cap_Value;
                
                if(Cap_Value_Rem2 > Cap_Value_Rem1)
                {
                    if((Cap_Value_Rem2 - Cap_Value_Rem1) < 5)
                    {
                        SysInfo.Low_Battery_flag = 1;
                    }
                    else
                    {
                        SysInfo.Low_Battery_flag = 0;
                    }                        
                }
                else
                {
                    if((Cap_Value_Rem1 - Cap_Value_Rem2) < 5)
                    {
                        SysInfo.Low_Battery_flag = 1;
                    }
                    else
                    {
                        SysInfo.Low_Battery_flag = 0;
                    } 
                }
            }break;
        }
    }
}

uint8_t getVolLevel()
{
	if (g_volRef < 2800)
	{
		return 5;               // <280
	}else if (g_volRef < 2900)
	{
		return 4;		// <290
	}else if (g_volRef < 3000)
	{
		return 3;		// <300
	}else if (g_volRef < 3100)
	{
		return 2;		// <310
	}else if (g_volRef < 3200)
	{
		return 1;		// <320
	}else 
	{
		return 0;		// >=320
	}
	
}


/*************************************************************************************
* FunctionName   : Check_Lamp_Temp()
* Description    : 灯头温度检测
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
uint8_t BEEP_TEMP_ONCE = 0;
uint8_t BEEP_CAP_LOW_ONCE =0;
void Check_Lamp_Temp(void)
{
    uint16_t Lamp_Value;
    
    static uint8_t TEMP_Tick;
    
    ADC_VALUE.Channel_LAMP_VALUE = filter(1);
    
    Lamp_Value = ADC_VALUE.Channel_LAMP_VALUE;
    
    if(Lamp_Value > Lamp_Temp_3)
    {
        Lamp_Temp_TypeDef = Lamp_Temp_level_2;
        SysInfo.Lamp_Glitter_flag = 1;
        if(BEEP_TEMP_ONCE)
        {
            BEEP_TEMP_ONCE = 0;
            BeepFlag.Beep_LampTemp_Flag  = 1;  //触发蜂鸣器
        }        
    }
    else if(Lamp_Value > Lamp_Temp_2)
    {
        Lamp_Temp_TypeDef = Lamp_Temp_level_1;
        SysInfo.Lamp_Glitter_flag = 0;
        BEEP_TEMP_ONCE = 1;

    }
    else if(Lamp_Value > Lamp_Temp_1)
    {
        Lamp_Temp_TypeDef = Lamp_Temp_level_0;
        SysInfo.Lamp_Glitter_flag = 0;
        BEEP_TEMP_ONCE = 1;
    }
    
    switch(Lamp_Temp_TypeDef)
    {
      case Lamp_Temp_level_0:
        {
            SysInfo.Change_Cap_En_flag = 1;
        }break;
      case Lamp_Temp_level_1:
        {
            ++TEMP_Tick;
            if(TEMP_Tick <= 3)
            {
                SysInfo.Change_Cap_En_flag = 0;
                SysInfo.Cap_Value_Count = 0;
            }
            else if(TEMP_Tick <= 4)
            {
                SysInfo.Change_Cap_En_flag = 1;
                TEMP_Tick = 0;
            }            
        }break;
      case Lamp_Temp_level_2:
        {
            SysInfo.Change_Cap_En_flag = 0;
        }break;
    } 
    
    if(SysInfo.Low_Battery_flag)
    {        
        if(BEEP_CAP_LOW_ONCE)
        {
            BEEP_CAP_LOW_ONCE = 0;
            BeepFlag.Beep_BattLow_Flag = 1;
        }
        SysInfo.Change_Cap_En_flag = 0;
    }
    else
    {
        BEEP_CAP_LOW_ONCE = 1;
    }
} 
