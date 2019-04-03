#include "includes.h"


_Comm_State     comm_state;

uint8_t offset;

uint8_t SONY_Send_DATA_F60M[26]=
{
    0x8F,0xF9,    
    0xEC,0xEB,0xCF,0xFD,0xB2,0xF7,0x5C,0x0A,
    0x7F,0xC4,0xCF,0x00,0xA0,0x89,0x89,0x89,
    0x89,0x97,0x97,0x97,0x97,0xFF,0x40,0xFF,
};
uint8_t SONY_Receive2_DATA[26]={0};
uint8_t SONY_Receive_DATA[14]={0};
uint8_t SONY_Receive_Pre_Data[14]={0};
uint8_t SONY_Receive_Fla_Data[14]={0};
uint8_t SONY_Receive_Cmd_Pre_Data[14]={0};
uint8_t SONY_Receive_Cmd_Fla_Data[14]={0};

_Light_Level_Data_TypeDef  M_SCALE_Group[] = {
    
    Light_Low_Level_Data_512,   Light_Low_Level_Data_512s3,     Light_Low_Level_Data_512s7,        
    Light_Low_Level_Data_256,   Light_Low_Level_Data_256s3,     Light_Low_Level_Data_256s7,    
    Light_Low_Level_Data_128,   Light_Low_Level_Data_128s3,     Light_Low_Level_Data_128s7,        
    Light_Low_Level_Data_64,    Light_Low_Level_Data_64s3,      Light_Low_Level_Data_64s7,         
    Light_Low_Level_Data_32,    Light_Low_Level_Data_32s3,      Light_Low_Level_Data_32s7,         
    Light_Low_Level_Data_16,    Light_Low_Level_Data_16s3,      Light_Low_Level_Data_16s7,         
    Light_Low_Level_Data_8,     Light_Low_Level_Data_8s3,       Light_Low_Level_Data_8s7,          
    Light_Low_Level_Data_4,     Light_Low_Level_Data_4s3,       Light_Low_Level_Data_4s7,          
    Light_Low_Level_Data_2,     Light_Low_Level_Data_2s3,       Light_Low_Level_Data_2s7,          
    Light_Low_Level_Data_1,    
};

_Flash_Time_TypeDef  Flash_Time_Group[] ={
    
    Flash_Time_512,     Flash_Time_512s3,       Flash_Time_512s7,           
    Flash_Time_256,     Flash_Time_256s3,       Flash_Time_256s7,   
    Flash_Time_128,     Flash_Time_128s3,       Flash_Time_128s7,           
    Flash_Time_64,      Flash_Time_64s3,        Flash_Time_64s7,            
    Flash_Time_32,      Flash_Time_32s3,        Flash_Time_32s7,            
    Flash_Time_16,      Flash_Time_16s3,        Flash_Time_16s7,            
    Flash_Time_8,       Flash_Time_8s3,         Flash_Time_8s7,             
    Flash_Time_4,       Flash_Time_4s3,         Flash_Time_4s7,             
    Flash_Time_2,       Flash_Time_2s3,         Flash_Time_2s7,             
    Flash_Time_1,    
};

void Enable_SCK_Init(void)
{
    disableInterrupts();
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising);
    ITC_SetSoftwarePriority(EXTI5_IRQn, ITC_PriorityLevel_2);
    enableInterrupts();
}
void Disable_SCK_Init(void)
{
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_In_FL_No_IT);
}
void Disable_SPI(void)
{
    SPI_DeInit(SPI1);
}
void Serial_send(void)
{
    comm_state.Send_packet_cnt = 0;
    SPI_DeInit(SPI1);
    SPI_Init(SPI1, SPI_FirstBit_LSB, 
             SPI_BaudRatePrescaler_2, 
             SPI_Mode_Slave, 
             SPI_CPOL_Low, 
             SPI_CPHA_2Edge, 
             SPI_Direction_1Line_Tx, 
             SPI_NSS_Soft, 
             0x07);
    SPI_ITConfig(SPI1,SPI_IT_TXE, ENABLE);
    SPI_NSSInternalSoftwareCmd(SPI1,DISABLE);

    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);
    
    SPI1->DR = SONY_Send_DATA_F60M[0];    
    SPI_ClearITPendingBit(SPI1,SPI_IT_TXE);    
    SPI_Cmd(SPI1,ENABLE);
    comm_state.Timeout_Hander_Flag = 1;
    comm_state.Timeout_Send_Flag   = 1;
    comm_state.Timeout_receive_Flag= 0;
    TimeOut_Start(8500);
}
void Serial_receive(void)
{
    comm_state.receive_packet_cnt = 0;
    SPI_DeInit(SPI1);
    SPI_Init(SPI1,SPI_FirstBit_LSB, 
             SPI_BaudRatePrescaler_2, 
             SPI_Mode_Slave, 
             SPI_CPOL_Low, 
             SPI_CPHA_2Edge, 
             SPI_Direction_1Line_Rx, 
             SPI_NSS_Soft, 
             0x07);
    SPI_ClearITPendingBit(SPI1,SPI_IT_RXNE); 
    SPI_ITConfig(SPI1,SPI_IT_RXNE, ENABLE);
    SPI_ClearITPendingBit(SPI1,SPI_IT_RXNE); 
    SPI_NSSInternalSoftwareCmd(SPI1,DISABLE);    
    
    SPI_Cmd(SPI1,ENABLE);
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);
    comm_state.Timeout_Hander_Flag = 1;
    comm_state.Timeout_Send_Flag   = 0;
    comm_state.Timeout_receive_Flag= 1;
    TimeOut_Start(4500);
}
    
   
void SPI_Data_Send_Receive(void)
{
    uint8_t timeout_spi = 100;
    uint16_t delay_time;
    
//    GPIO_ToggleBits(GPIOG, GPIO_Pin_5); 
        
    if(comm_state.send_flag)//send mode
    {
        if(comm_state.Send_packet_cnt >= sizeof(SONY_Send_DATA_F60M))
        {
            comm_state.send_flag = 0;
            delay_time = 100;
            while(delay_time--);
            Disable_SPI();
            Enable_SCK_Init();
        }
        else
        {
            SPI1->DR = SONY_Send_DATA_F60M[++comm_state.Send_packet_cnt];
            while((SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)==RESET)&&(timeout_spi--));
        }
    }
    else if(comm_state.receive_flag)//receive mode
    {
        SONY_Receive_DATA[comm_state.receive_packet_cnt++] = SPI1->DR;
        if( comm_state.receive_packet_cnt >= 16 )
        {
            comm_state.receive_flag = 0;
//            comm_state.link = 1;
            delay_time = 100;
            while(delay_time--);
            Disable_SPI();
            Enable_SCK_Init();
        }
    }    
}

void Sony_Communication(void)
{
    uint16_t TimeOut_Sck = 1000; //TIMEOUT_SCK;
    uint32_t time_clk = 0;    
    TIM2_SetCounter(0);
    TIM2_Cmd(ENABLE);    
    while((SCK_IN)&&(TimeOut_Sck--)); 
    TIM2_Cmd(DISABLE);
    time_clk = TIM2_GetCounter(); 
    
    if((time_clk > 60)&(time_clk < 90))                           //88us
    {  
        if( 0 == comm_state.preflash2_flag)
        {  
            if(0 == comm_state.cmd_ctrl_flash_flag)
            { 
                Disable_SCK_Init();
                comm_state.send_flag = 1; 
                Serial_send();
            }
            else
            {
                Enable_SCK_Init();
            }
        }
        else
        {
            comm_state.preflash2_flag = 0;                           
            comm_state.preflash1_flag = 1;                          //有预闪标志
            memcpy(SONY_Receive_Pre_Data,SONY_Receive_DATA,14);     //取预闪数据  
            
            switch(Sys_Mode_Typedef)
            {
              case Sys_Mode_TTL:
                {                            
                    if(SONY_Receive_Pre_Data[8] == 0xEF)
                    {
                        //HSS_Prefrash();//高速预闪方式
                        TTL_Prefrash(SONY_Receive_Pre_Data[9]);
                    }
                    else
                    {
                        TTL_Prefrash(SONY_Receive_Pre_Data[9]);   //预闪一束的方式
                    }
                }break;
              case Sys_Mode_M:
              case Sys_Mode_RPT: break;
              default:break;
            }            
            Enable_SCK_Init();
        }
    }
    else if((time_clk > 130)&(time_clk < 170))          //162us
    {
        Disable_SCK_Init();
        comm_state.link_cnt++;
        comm_state.receive_flag = 1; 
        comm_state.receive_packet_cnt = 0;
        Serial_receive();
    }
    else if((time_clk > 230)&(time_clk < 270))         //270us
    {
        if(1 == comm_state.preflash2_flag)
        {
            comm_state.preflash2_flag = 0;     
            memcpy(SONY_Receive_Fla_Data,SONY_Receive_DATA,14);       //低速正闪数据             
            offset = Flash_Time_out(SONY_Receive_Fla_Data[9]);        //计算闪光档位//取出正闪值,数据范围：0xEE->OxFF & 0xC0->0xD6 
        }
        else
        {
            comm_state.preflash2_flag = 1;
        } 
    }
    else
    {        
        time_clk = 0;  
    }
}
             
void SPI1_IT_Callback(void)     //SPI1中断
{
    SPI_Data_Send_Receive();
}
void GPIO_Pin_4_Callback(void)  //大触点中断
{
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
        {                            
            Formalfrash(Flash_Time_Group[offset]); 
//            Enable_SCK_Init();
        }break;
      case Sys_Mode_M:
        {                    
            Formalfrash(Flash_Time_Group[(Flash_Output_Typedef+9)+6]);
//            Enable_SCK_Init();
        }break;
      case Sys_Mode_RPT:
        {
            Multiflash();
        }break;
    }
}
void GPIO_Pin_5_Callback(void) //SCK中断
{
    Sony_Communication();
}
void GPIO_Pin_1_Callback(void) //光敏中断
{
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_S1:
        {
            uint16_t time;
            GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);
            Formalfrash(Flash_Time_Group[(Slave_Output_Typedef+9)+6]);
            time = 0x1000;
            while(time--);
            GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_PU_IT);
        }break;
      case Sys_Mode_S2:
        {
            
        }break;
      default:break;
    }
}
//void TIM4_Callback(void)
//{
//    if(comm_state.Timeout_Hander_Flag)
//    {        
//        comm_state.Timeout_cnt++;
//    }
//    else
//    {
//        comm_state.Timeout_cnt = 0;
//    }
//    if(comm_state.Timeout_Send_Flag)
//    {
//        if(comm_state.Timeout_cnt>8)
//        {
//            comm_state.Timeout_cnt = 0;
//            comm_state.Timeout_Hander_Flag = 0;
//            comm_state.Send_packet_cnt = 0;
//            comm_state.receive_packet_cnt = 0;
//            Disable_SPI();
//            Enable_SCK_Init();
//        }        
//    }
//    else if(comm_state.Timeout_receive_Flag)
//    {
//        if(comm_state.Timeout_cnt>4)
//        {
//            comm_state.Timeout_cnt = 0;
//            comm_state.Timeout_Hander_Flag = 0;
//            comm_state.Send_packet_cnt = 0;
//            comm_state.receive_packet_cnt = 0;
//            Disable_SPI();
//            Enable_SCK_Init();
//        }        
//    }
//}
void TIM2_Callback(void)
{
    TIM2_Cmd(DISABLE);
    GPIO_ToggleBits(GPIOA,GPIO_Pin_3);
    comm_state.Timeout_cnt = 0;
    comm_state.Timeout_Hander_Flag = 0;
    comm_state.Send_packet_cnt = 0;
    comm_state.receive_packet_cnt = 0;
    comm_state.send_flag = 0;
    comm_state.receive_flag = 0;
    Disable_SPI();
    Enable_SCK_Init();
}
void TimeOut_Start(uint16_t time)
{
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up,time);
    TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    TIM2_Cmd(ENABLE);
}

/*******************************************************************************
* FunctionName   : Flash_Time_out()
* Description    : 计算正闪档位
* EntryParameter : value                闪光码值     
* ReturnValue    : count                偏移量  
                   Flash_Output_TypeDef 档位
*******************************************************************************/
uint8_t Flash_Time_out(uint8_t value)
{
    int8_t count = 0;
//    uint8_t Ratio_set = 0;
    
    if(value<= 0x3F)        //兼容A55，第二组预闪值 //A55相机：20-3F | 00-1F
    {
        value = value + 0xC0;
    }
                            
    if(value >= 0xE0)       //范围：   E0-FF | C0-DF   普通闪光：EE-FF | C0-D6  指令机顶：E4-FF | C0-DF
    {
        value = value - 0xE0;
    }
    else if(value < 0xE0)  // 让取值在0-63范围，且为从小到大顺序取值
    {
        value = value - 0xA0; 
    }  
    
    if(value <= 0x3F)      // 0-63 方便取值
    {
        count = sizeof(M_SCALE_Group);
        
        while(--count)
        {
            if(value >= M_SCALE_Group[count])  
                break;
        }        
    } 
    
//    if(Mode_TypeDef == Sys_Mode_Cmd_CTRL)
//    {
//        uint16_t CMD_Rmt1_Ratio;        
//        
//        CMD_Rmt1_Ratio = (uint16_t)(((float)CMD_CTRL_TypeDef /(float)((float)CMD_CTRL_TypeDef + \
//                                    (float)CMD_RMT_TypeDef + (float)CMD_RMT2_TypeDef))*1000);        
//        Ratio_set = sizeof(CTRL_Ratio_Group)/2;
//        
//        while(--Ratio_set)
//        {
//            if(CMD_Rmt1_Ratio >= CTRL_Ratio_Group[Ratio_set])  
//                break;
//        }
//        
//        Ratio_set = (sizeof(CTRL_Ratio_Group)/2) - Ratio_set - 1;  
//        
//        count = count + (Sony_Ev_TypeDef) - Ratio_set; 
//    }
//    else
//    {            
//        count = count + (Sony_Ev_TypeDef);
//    }
    count = count + (TTL_Ev_TypeDef);
    
    if(count<0)
    {
        count = 0;
    }
    else if(count>54)
    {
        count = 54;
    }
    
    return count;
}
/*************************************************************************************
* FunctionName   : Start_Flash()
* Description    : 闪光定时器启动
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Start_Flash(uint16_t time)
{ 
    uint16_t Time_Master = 0;

//    SysInfo.Cap_Value_Count = 0;
    
//    AF_LED_OFF;
    
//    SysInfo.AF_Led_Count = 0;
    
//    IGBT_30V_EN;
    
//    CHANGE_CAP_OFF;
    
//    H_CHANGDE_L;
    
    SysInfo.frash_Start_Flag = 1;
    
//    SysInfo.Cap_Change_Delay_flag = 1;
    
//    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
//    TIM1_DeInit();
//    TIM1_TimeBaseInit(15, TIM1_CounterMode_Up,8000,0);
//    TIM1_ClearFlag(TIM1_FLAG_Update);
//    TIM1_ITConfig(TIM1_IT_Update, DISABLE);
//    TIM1_SetCounter(0x0000);
//    TIM1_Cmd(ENABLE);    
//    __disable_interrupt();    
//    IGBT_DRIVER_Start;           //闪光开始         
//    while(Time_Master < time)
//    {
//        Time_Master = (uint16_t)TIM1_GetCounter();
//    }    
//    IGBT_DRIVER_Stop;      
//    TIM1_Cmd(DISABLE);
    
    CHANGE_CAP_STOP;
    Disable_SCK_Init();
    
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up,8000);
    TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_ITConfig(TIM2_IT_Update, DISABLE);
    TIM2_SetCounter(0);
    TIM2_Cmd(ENABLE);    
    __disable_interrupt();    
    IGBT_DRIVER_Start;           //闪光开始    
    while(Time_Master < time)
    {
        Time_Master = (uint16_t)TIM2_GetCounter();
    }
    IGBT_DRIVER_Stop;      
    TIM2_Cmd(DISABLE);
    
    EXTI_ClearITPendingBit(EXTI_IT_Pin5);
    EXTI_ClearITPendingBit(EXTI_IT_Pin4);
    SPI_ClearITPendingBit(SPI1,SPI_IT_TXE);
    SPI_ClearITPendingBit(SPI1,SPI_IT_RXNE);
    comm_state.Timeout_cnt = 0;
    comm_state.Timeout_Hander_Flag = 0;
    comm_state.Send_packet_cnt = 0;
    comm_state.receive_packet_cnt = 0;
    comm_state.send_flag = 0;
    comm_state.receive_flag = 0;
    Disable_SPI();
    
//IGBT_30V_OFF;
//SysInfo.frash_Start_Flag = 0;
    __enable_interrupt(); 
    
    Enable_SCK_Init();
}
void TTL_Prefrash(uint8_t data)
{
    uint32_t delay_time = 0;
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
    switch(data)
    {
      case 0xF4: Start_Flash(Flash_Time_256s3);break;//F1.4
      case 0xF5: Start_Flash(Flash_Time_256s7);break;//F1.6
      case 0xF6: Start_Flash(Flash_Time_128  );break;//F1.7
      case 0xF7: Start_Flash(Flash_Time_128  );break;
      case 0xF8: Start_Flash(Flash_Time_128s3);break;//F2.0
      case 0xF9: Start_Flash(Flash_Time_128s7);break;//F2.2
      case 0xFA: Start_Flash(Flash_Time_64   );break;//F2.5
      case 0xFB: Start_Flash(Flash_Time_64   );break;
      case 0xFC: Start_Flash(Flash_Time_64s3 );break;//F2.8
      case 0xFD: Start_Flash(Flash_Time_64s7 );break;//F3.2
      case 0xFE: Start_Flash(Flash_Time_32   );break;//F3.5
      case 0xFF: Start_Flash(Flash_Time_32   );break;
      case 0xC0: Start_Flash(Flash_Time_32s3 );break;//F4.0
      case 0xC1: Start_Flash(Flash_Time_32s7 );break;//F4.5
      case 0xC2: Start_Flash(Flash_Time_16   );break;//F5.0
      case 0xC3: Start_Flash(Flash_Time_16   );break;
      case 0xC4: Start_Flash(Flash_Time_16s3 );break;//F5.6
      case 0xC5: Start_Flash(Flash_Time_16s7 );break;//F6.3
      case 0xC6: Start_Flash(Flash_Time_8    );break;//F7.1
      
      default :
        {            
            data = data & 0x0F;                        //兼容老相机
            switch(data)
            {
              case 0x00:Start_Flash(Flash_Time_32);break;
              case 0x04:Start_Flash(Flash_Time_16);break;
              case 0x06:Start_Flash(Flash_Time_8);break; 
              default:Start_Flash(Flash_Time_8);break;
            }            
        }break;
    }
    
    delay_time = 3200;
    while(delay_time--);    
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);  
}

void Formalfrash(uint16_t time)//正闪
{
    if((time > 2) & (time <= Flash_Time_1))
    {
        Start_Flash(time-1);
    }
    if(SysInfo.Cap_Full_Flag)
    {
        BeepFlag.Beep_TrueFlash_Flag = 1;
    }
    else
    {
        BeepFlag.Beep_FailFlash_Flag = 1;
    }
}

void Multifrash(uint16_t time)//正闪
{
    if((time > 2) & (time <= Flash_Time_1))
    {
        Start_Flash(time-1);
    }
}

void Multifrash_Handle(void)
{
    static uint16_t Multi_Cnt; 

    if((SysInfo.frash_Multi_Flag) & ((!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4))|(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))))
    {
        uint16_t Freq_time;   
        
        Freq_time = (1000/Multi_Freq_TypeDef);
        
        if(SysInfo.Multi_Times-1)
        {
            Multi_Cnt++;
            if(Multi_Cnt >= Freq_time)
            {
                Multi_Cnt = 0;
                SysInfo.Multi_Times--;
                Multifrash(Flash_Time_Group[(Multi_Output_Typedef+9)+6]);
            }
        }
        else
        {
            Multi_Cnt = 0;
            SysInfo.frash_Multi_Flag = 0;
        }  
    }
    else
    {
        Multi_Cnt = 0;
        SysInfo.frash_Multi_Flag = 0;
    }
}

void Multiflash(void)
{    
    Multifrash(Flash_Time_Group[(Multi_Output_Typedef+9)+6]);
    
    SysInfo.frash_Multi_Flag = 1;
    
    SysInfo.Multi_Times = Multi_Times_TypeDef;
    
    if(SysInfo.Cap_Full_Flag)
    {
        BeepFlag.Beep_TrueFlash_Flag = 1;
    }
    else
    {
        BeepFlag.Beep_FailFlash_Flag = 1;
    }
}

/*******************************************************************************
* FunctionName   : Txbuf_Change_Mode()
* Description    : 灯模式改变发送的数据
* EntryParameter : None                     
* ReturnValue    : None                  
                   None 
*******************************************************************************/
void Txbuf_Change_Mode(void)
{
    switch(Sys_Mode_Typedef)
    {        
      case Sys_Mode_TTL:
        SONY_Send_DATA_F60M[7] = 0xF7;
        SONY_Send_DATA_F60M[11]= 0x84;
        SONY_Send_DATA_F60M[10]= 0x7F;
        SONY_Send_DATA_F60M[15]= 0x98;
        SONY_Send_DATA_F60M[16]= 0x98;
        SONY_Send_DATA_F60M[17]= 0x98;
        SONY_Send_DATA_F60M[18]= 0x98;
        SONY_Send_DATA_F60M[19]= 0xA7;
        SONY_Send_DATA_F60M[20]= 0xA7;
        SONY_Send_DATA_F60M[21]= 0xA7;
        SONY_Send_DATA_F60M[22]= 0xA7;
        break;                
      case Sys_Mode_M: //break;
        {
            switch(Flash_Output_Typedef)
            {
              case Flash_Output_1:  SONY_Send_DATA_F60M[10]= 0x0F;break;
              case Flash_Output_2:  SONY_Send_DATA_F60M[10]= 0x1F;break;
              case Flash_Output_4:  SONY_Send_DATA_F60M[10]= 0x2F;break;
              case Flash_Output_8:  SONY_Send_DATA_F60M[10]= 0x3F;break;
              case Flash_Output_16: SONY_Send_DATA_F60M[10]= 0x4F;break;
              case Flash_Output_32: SONY_Send_DATA_F60M[10]= 0x5F;break;
              case Flash_Output_64: SONY_Send_DATA_F60M[10]= 0x6F;break;
              case Flash_Output_128:SONY_Send_DATA_F60M[10]= 0x7F;break;
            }
        }                
      case Sys_Mode_RPT:
        SONY_Send_DATA_F60M[7] = 0xF6;
        SONY_Send_DATA_F60M[15]= 0x88;
        SONY_Send_DATA_F60M[16]= 0x88;
        SONY_Send_DATA_F60M[17]= 0x88;
        SONY_Send_DATA_F60M[18]= 0x88;
        SONY_Send_DATA_F60M[19]= 0x88;
        SONY_Send_DATA_F60M[20]= 0x88;
        SONY_Send_DATA_F60M[21]= 0x88;
        SONY_Send_DATA_F60M[22]= 0x88;
        break;
    }

//      case Sys_Mode_Cmd_CTRL:
//        {                        
//            switch(CMD_CH_TypeDef)
//            {
//              case CMD_CH_1:SONY_Send_DATA_F60M[8] = 0x50;break;
//              case CMD_CH_2:SONY_Send_DATA_F60M[8] = 0x54;break;
//              case CMD_CH_3:SONY_Send_DATA_F60M[8] = 0x58;break;
//              case CMD_CH_4:SONY_Send_DATA_F60M[8] = 0x5C;break;
//            }
//            
//            switch(CMD_Ratio_TypeDef)
//            {
//              case CMD_Ratio_OFF:
//                {
//                    SONY_Send_DATA_F60M[2] = 0xEA;
//                    SONY_Send_DATA_F60M[3] = 0xE9;
//                    SONY_Send_DATA_F60M[4] = 0xCD;
//                    
//                    SONY_Send_DATA_F60M[13] = 0x08;
//                    SONY_Send_DATA_F60M[14] = 0xA0;
//                    
//                }
//              case CMD_Ratio_TTL:
//                {
//                    uint8_t data;
//                    
//                    SONY_Send_DATA_F60M[2]= 0xEC;
//                    SONY_Send_DATA_F60M[3]= 0xEB;
//                    SONY_Send_DATA_F60M[4]= 0xCF;
//                    
//                    switch(CMD_CTRL_TypeDef)
//                    {
//                      case CMD_CTRL_None: SONY_Send_DATA_F60M[14] = 0xA0;break;
//                      case CMD_CTRL_1:  SONY_Send_DATA_F60M[14] = 0xA8;break;
//                      case CMD_CTRL_2:  SONY_Send_DATA_F60M[14] = 0xA9;break;
//                      case CMD_CTRL_4:  SONY_Send_DATA_F60M[14] = 0xAA;break;
//                      case CMD_CTRL_8:  SONY_Send_DATA_F60M[14] = 0xAB;break;
//                      case CMD_CTRL_16: SONY_Send_DATA_F60M[14] = 0xAC;break;
//                    }
//                    data = 0;
//                    switch(CMD_RMT_TypeDef)
//                    {
//                      case CMD_RMT_None: data |= 0x00;break;
//                      case CMD_RMT_1:    data |= 0x08;break;
//                      case CMD_RMT_2:    data |= 0x09;break;
//                      case CMD_RMT_4:    data |= 0x0A;break;
//                      case CMD_RMT_8:    data |= 0x0B;break;
//                      case CMD_RMT_16:   data |= 0x0C;break;
//                    }
//                    switch(CMD_RMT2_TypeDef)
//                    {
//                      case CMD_RMT2_None:data |= 0x00;break;
//                      case CMD_RMT2_1:   data |= 0x80;break;
//                      case CMD_RMT2_2:   data |= 0x90;break;
//                      case CMD_RMT2_4:   data |= 0xA0;break;
//                      case CMD_RMT2_8:   data |= 0xB0;break;
//                      case CMD_RMT2_16:  data |= 0xC0;break;
//                    }
//                    SONY_Send_DATA_F60M[13] = data;
//                }
//              case CMD_Ratio_M:
//                {
//                    SONY_Send_DATA_F60M[3]= 0xFF;
//                    
//                }
//            }
//            
//            SONY_Send_DATA_F60M[11]= 0xC4;
//            SONY_Send_DATA_F60M[15]= 0x88;
//            SONY_Send_DATA_F60M[16]= 0x88;
//            SONY_Send_DATA_F60M[17]= 0x88;
//            SONY_Send_DATA_F60M[18]= 0x88;
//            SONY_Send_DATA_F60M[19]= 0x88;
//            SONY_Send_DATA_F60M[20]= 0x88;
//            SONY_Send_DATA_F60M[21]= 0x88;
//            SONY_Send_DATA_F60M[22]= 0x88;
//
//        }break;
//      case Sys_Mode_Cmd_Remote:
//        {
//            switch(CMD_CH_TypeDef)
//            {
//              case CMD_CH_1:SONY_Send_DATA_F60M[8] = 0x50;break;
//              case CMD_CH_2:SONY_Send_DATA_F60M[8] = 0x54;break;
//              case CMD_CH_3:SONY_Send_DATA_F60M[8] = 0x58;break;
//              case CMD_CH_4:SONY_Send_DATA_F60M[8] = 0x5C;break;
//            }            
//        }break;
//    }
}

