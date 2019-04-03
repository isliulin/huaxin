#include "includes.h"


_Motor_Manger_TypeDef   Motor_Manger_TypeDef     = {0};
_Motor_Contorl_TypeDef  Motor_Contorl_TypeDef    = {0};

uint8_t  Camera_Zoom_offset;

#define MOTOR_DATA_LENTH                                 35

const _Motor_Value_TypeDef Motor_Value_Data[MOTOR_DATA_LENTH]= {
    Transfer_Motor_24,Transfer_Motor_25,Transfer_Motor_26, \
    Transfer_Motor_28,Transfer_Motor_30,Transfer_Motor_32, \
    Transfer_Motor_34,Transfer_Motor_35,Transfer_Motor_38, \
    Transfer_Motor_40,Transfer_Motor_42,Transfer_Motor_45, \
    Transfer_Motor_48,Transfer_Motor_50,Transfer_Motor_52, \
    Transfer_Motor_55,Transfer_Motor_58,Transfer_Motor_62, \
    Transfer_Motor_66,Transfer_Motor_70,Transfer_Motor_75, \
    Transfer_Motor_80,Transfer_Motor_85,Transfer_Motor_90, \
    Transfer_Motor_92,Transfer_Motor_95,Transfer_Motor_98, \
    Transfer_Motor_105,Transfer_Motor_112,Transfer_Motor_120, \
    Transfer_Motor_130,Transfer_Motor_135,Transfer_Motor_140, \
    Transfer_Motor_180,Transfer_Motor_200
};

const _Motor_Step_TypeDef Motor_Step_Data[MOTOR_DATA_LENTH] = {
    Motor_Step_24,Motor_Step_25,Motor_Step_26, \
    Motor_Step_28,Motor_Step_30,Motor_Step_32, \
    Motor_Step_33,Motor_Step_34,Motor_Step_38, \
    Motor_Step_40,Motor_Step_42,Motor_Step_45, \
    Motor_Step_48,Motor_Step_50,Motor_Step_52, \
    Motor_Step_55,Motor_Step_58,Motor_Step_62, \
    Motor_Step_66,Motor_Step_70,Motor_Step_75, \
    Motor_Step_80,Motor_Step_85,Motor_Step_90, \
    Motor_Step_92,Motor_Step_95,Motor_Step_98, \
    Motor_Step_105,Motor_Step_112,Motor_Step_120, \
    Motor_Step_130,Motor_Step_135,Motor_Step_140, \
    Motor_Step_180,Motor_Step_200
};

const  _Canon_Data_Zoom_TypeDef  Camera_Zoom_Data[11] ={
    Camera_Zoom_18,
    Camera_Zoom_24,  
    Camera_Zoom_28,       
    Camera_Zoom_35,          
    Camera_Zoom_50,         
    Camera_Zoom_70,
    Camera_Zoom_80,
    Camera_Zoom_105,           
    Camera_Zoom_135,           
    Camera_Zoom_180,
    Camera_Zoom_200
};

const  _Moto_Zoom_TypeDef  Moto_Zoom_Data[] ={
    Moto_Zoom_18,
    Moto_Zoom_24,  
    Moto_Zoom_28,       
    Moto_Zoom_35,          
    Moto_Zoom_50,         
    Moto_Zoom_70,           
    Moto_Zoom_105,           
    Moto_Zoom_135,           
    Moto_Zoom_180,  
    Moto_Zoom_200
};

/*************************************************************************************
* FunctionName   : Motor_Contorl()
* Description    : 马达控制
* EntryParameter : 
* ReturnValue    : Motor_Manger_TypeDef.Motor_Step
**************************************************************************************/
void Motor_Contorl(void)
{
    unsigned char Motor_Count;
    if(Motor_Manger_TypeDef.Motor_Step == 0)// &&\
        (fDevice_TypeDef.fTemp_Protection == RESET) &&\
            (Device_Time_Manager.Battery_Charge_Time < BATTERY_CHARGE_TIME_LIMIT))//马达停止,没有过温并且电池不是低电量
    {      
        switch(Motor_Manger_TypeDef.Motor_State)
        {                                         
          case Motor_Normal_Run:
            {
                if(Motor_Contorl_TypeDef.Motor_Value < Transfer_Motor_24)
                {
                    Motor_Count = 0;
                }
                else if(Motor_Contorl_TypeDef.Motor_Value <= Transfer_Motor_180)
                {
                    for(Motor_Count = 0; Motor_Count < MOTOR_DATA_LENTH; Motor_Count++)
                    {
                        if(Motor_Contorl_TypeDef.Motor_Value <= Motor_Value_Data[Motor_Count])
                        {
                            break;
                        }
                    }
                }
                else
                {
                    Motor_Count = MOTOR_DATA_LENTH - 1;
                }
                
                if(Motor_Manger_TypeDef.Motor_Mem != Motor_Count)
                {                       
                    if(Motor_Manger_TypeDef.fMotor_State.fMotor_Reset == SET)
                    {
//                        Motor_Manger_TypeDef.fMotor_State.fMotor_Reset = RESET;
//                        Motor_Manger_TypeDef.Motor_Step = Motor_Step_Data[Motor_Count];
//                        Motor_Manger_TypeDef.Motor_Mem  = Motor_Count;
//                        Motor_Manger_TypeDef.fMotor_State.fMotor_Revise = SET;
//                        CHANGE_CAP_DIS;
//                        PWM3_DISABLE();                         
                        
                        if(Motor_Manger_TypeDef.Motor_Mem == 0xFF)
                        {
                            Motor_Manger_TypeDef.Motor_Step = Motor_Step_Data[Motor_Count];
                            Motor_Manger_TypeDef.Motor_Mem  = Motor_Count;
                        }
                        else
                        {                             
                            Motor_Manger_TypeDef.Motor_Step = \
                                Motor_Step_Data[Motor_Count] - Motor_Step_Data[Motor_Manger_TypeDef.Motor_Mem];                            
                            Motor_Manger_TypeDef.Motor_Mem  = Motor_Count;
                        }
                    }
                    else
                    {
                        MOTOR_RESET_EN;
                    }
                }              
            }break;
            
          case Motor_Reset_Check:
            {
                if(!MOTOR_MOVE_STATE)
                {
                    Motor_Manger_TypeDef.Motor_State = Motor_Reset_Forward;
                }
                else
                {
                    Motor_Manger_TypeDef.Motor_State = Motor_Reset_Reverse;
                }
                Motor_Manger_TypeDef.fMotor_State.fMotor_Reset = SET;
                Motor_Manger_TypeDef.Motor_tickstart = uwTick;
            }break;
            
          case Motor_Reset_Forward:
            {
                Motor_Manger_TypeDef.Motor_Step = MOTOR_RESET_STEP;
//                CHANGE_CAP_DIS;
//                PWM3_DISABLE();                  
            }break;
            
          case Motor_Reset_Reverse:
            {
                Motor_Manger_TypeDef.Motor_Step = 0 - MOTOR_RESET_STEP;
//                CHANGE_CAP_DIS;
//                PWM3_DISABLE();                  
            }break; 
          case Motor_Reset_Error:
            {
                MOTO_ENA_OFF;
            }
        }       
    }
    if(Motor_Manger_TypeDef.Motor_State != Motor_Normal_Run)
    {  
        if(Motor_Manger_TypeDef.Motor_State != Motor_Reset_Error)
        {
            if(uwTick - Motor_Manger_TypeDef.Motor_tickstart > Motor_Res_TimeOut)
            {
                Motor_Manger_TypeDef.Motor_State = Motor_Reset_Error;
                Motor_Manger_TypeDef.Motor_Step  = 0;
                MOTO_ENA_OFF;
            }
        }
    }
}
/*************************************************************************************
* FunctionName   : Moto_Operation()
* Description    : 马达步进，在系统TICK定时器回调函数中调用
* EntryParameter : 
* ReturnValue    : 
**************************************************************************************/
void Moto_Operation(void)
{
    if(Motor_Manger_TypeDef.Motor_Step > 0)
    {
        MOTO_ENA_ON;
        Motor_Manger_TypeDef.Motor_Move_State = Motor_Move_In;
        Motor_IN12_OFF;
        switch(Motor_Manger_TypeDef.Motor_Period)
        {          
          case Motor_Forward_1s4: Motor_Forward_1;break;
          case Motor_Forward_2s4: Motor_Forward_2;break; 
          case Motor_Forward_3s4: Motor_Forward_3;break;
          case Motor_Forward_4s4: Motor_Forward_4;break;        
        }
        if(++Motor_Manger_TypeDef.Motor_Period > Motor_Forward_4s4)
        {
            Motor_Manger_TypeDef.Motor_Period = Motor_Forward_1s4;
            Motor_Manger_TypeDef.Motor_Step--;             
            if(Motor_Manger_TypeDef.Motor_State == Motor_Reset_Forward)
            {
                if(MOTOR_MOVE_STATE)
                {
                    MOTOR_RESET_DIS;
                }
            }                                        
        }
    }
    else if(Motor_Manger_TypeDef.Motor_Step < 0)
    {
        MOTO_ENA_ON;
        Motor_Manger_TypeDef.Motor_Move_State = Motor_Move_In;
        Motor_IN12_OFF;
        switch(Motor_Manger_TypeDef.Motor_Period)
        {
          case Motor_Reverse_1s4: Motor_Reverse_1;break;
          case Motor_Reverse_2s4: Motor_Reverse_2;break;
          case Motor_Reverse_3s4: Motor_Reverse_3;break;
          case Motor_Reverse_4s4: Motor_Reverse_4;break;
        }
        if(++Motor_Manger_TypeDef.Motor_Period > Motor_Reverse_4s4)
        {
            Motor_Manger_TypeDef.Motor_Period = Motor_Reverse_1s4;
            Motor_Manger_TypeDef.Motor_Step++;            
            if(Motor_Manger_TypeDef.Motor_State == Motor_Reset_Reverse)
            {
                if(!MOTOR_MOVE_STATE)
                {
                    MOTOR_RESET_DIS;
                }   
            }          
        }         
    }
    else
    {
        MOTO_ENA_OFF;
        Motor_Manger_TypeDef.Motor_Move_State = Motor_Move_End;
    }
}
/*************************************************************************************
* FunctionName   : Cannon_Camera_Zoom_Handler()
* Description    : 佳能对应的焦距值
* EntryParameter : 
* ReturnValue    : None
**************************************************************************************/
void Cannon_Camera_Zoom_Handler(uint8_t Zoom_Value)
{
    if(M_Zoom_TypeDef == M_Zoom_AUTO)
    {
        uint8_t count,offset;   
        
        count = sizeof(Camera_Zoom_Data);
        
        while(--count)
        {
            if(Zoom_Value >= Camera_Zoom_Data[count])  
                break;
        } 
        
        offset = count;
        
        count = sizeof(Motor_Value_Data);
        
        Zoom_Value = Moto_Zoom_Data[offset];
        
        while(--count)
        {
            if(Zoom_Value >= Motor_Value_Data[count])  
                break;
        }
        
        offset = count;
        
        Motor_Contorl_TypeDef.Motor_Value = Motor_Value_Data[offset]; 
    }
}
/*************************************************************************************
* FunctionName   : Motor_Oper()
* Description    : 
* EntryParameter : 
* ReturnValue    : None
**************************************************************************************/
void Motor_Oper(void)
{
    switch(M_Zoom_TypeDef)
    {
      //case M_Zoom_AUTO:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_35;break;
      case M_Zoom_24mm:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_24;break;
      case M_Zoom_28mm:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_28;break;
      case M_Zoom_35mm:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_35;break;
      case M_Zoom_50mm:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_50;break;
      case M_Zoom_70mm:  Motor_Contorl_TypeDef.Motor_Value = Key_Motor_70;break;
      case M_Zoom_105mm: Motor_Contorl_TypeDef.Motor_Value = Key_Motor_105;break;
      case M_Zoom_135mm: Motor_Contorl_TypeDef.Motor_Value = Key_Motor_135;break;
      case M_Zoom_180mm: Motor_Contorl_TypeDef.Motor_Value = Key_Motor_180;break;
    }
    
    if(comm_state.link)   //有通信状态才更新ZOOM，否则回到35
    {
//        if((menuPara.motoCod & 0xf0) ==0x00)
//        {
//            Cannon_Camera_Zoom_Handler();
//        }
        if(*motoSet.mode == 0)
        {
            Cannon_Camera_Zoom_Handler(menuPara.motoLoc);
        }
    }
    
    Motor_Contorl();
}