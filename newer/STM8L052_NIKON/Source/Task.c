#include "includes.h"

volatile uint32_t uwTick;   //1ms������
extern u8 g_canComStatue;
extern u16 g_dspUpdateFlag;
extern u8 g_cmrLinked;

uint32_t GetTick(void)
{
  return (uint32_t)uwTick;  //����ֵ���󣬲�ʹ��
}

typedef enum _TASK_LIST
{
//	TASK_COM,
    TASK_1,                             
    TASK_2,                            
    TASK_3,
    TASK_4, 
    TASK_5, 
    TASKS_MAX,                          // �ܵĿɹ�����Ķ�ʱ������Ŀ
} TASK_LIST;

typedef struct _TASK_COMPONENTS
{
    uint8_t  Run;                       // �������б�ǣ�0-�����У�1����
    uint16_t Timer;                     // ��ʱ��
    uint16_t ItvTime;                   // �������м��ʱ��
    void (*TaskHook)(void);            // Ҫ���е�������
} TASK_COMPONENTS;

//����״̬
static TASK_COMPONENTS TaskComps[] = 
{
//    {0,  1,   2,       ComMonitor},
    {0,  10,  10,      Task_1 /*charge*/},           //  10MS
    {0,  20,  20,      Task_2 /*key, beep*/},        //  20MS
    {0,  60,  60,      Task_3 /*lcd*/},              //  60MS
    {0,  100, 100,     Task_4 /*moto, low_bat*/},    //  100MS
    {0,  500, 500,     Task_5 /*bk light*/},         //  500MS
};
/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : �����־���� ��ʱ���ж���ִ��
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskRemarks(void)
{
    uint8_t i;
    for (i=0; i<TASKS_MAX; i++)                                 // �������ʱ�䴦��
    {
        if (TaskComps[i].Timer)                                 // 
        {
            TaskComps[i].Timer--;                               // 
            if (TaskComps[i].Timer == 0)                        // 
            {
                TaskComps[i].Timer = TaskComps[i].ItvTime;      // 
                TaskComps[i].Run = 1;                           // �����������
            }
        }
    }
}
/**************************************************************************************
* FunctionName   : HAL_SYSTICK_Callback()
* Description    : SYSTICK�ص�����  1ms��ʱ���ж�
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void HAL_SYSTICK_Callback(void)
{
    Moto_Operation();
    TaskRemarks();
    Multifrash_Handle();
    Slave_S2_TimeOut();
}
/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : ������
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskProcess(void)
{
    uint8_t i;
    for (i=0; i<TASKS_MAX; i++)                 // �������ʱ�䴦��
    {
        if (TaskComps[i].Run)                   // ʱ�䲻Ϊ0
        {
            TaskComps[i].TaskHook();            // ��������
            TaskComps[i].Run = 0;               // ��־��0
        }
    }   
}
/**************************************************************************************
* FunctionName   : TASK()
* Description    : ����
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Task_1(void)
{
    static uint8_t  IGBT_30V_EN_Flag;    
    if(SysInfo.Sys_Power_Flag)
    {
        Change_Cap();
    }
    else
    {
        CHANGE_CAP_STOP;
    }
    
    if(SysInfo.frash_Start_Flag == 0)
    {
        IGBT_30V_EN_Flag++;
        
        switch(IGBT_30V_EN_Flag)
        {
          case 4:IGBT_30V_ON ;break;
          case 8:IGBT_30V_OFF;IGBT_30V_EN_Flag = 0;break;
          default:break;
        }
    }
}
void Task_2(void)
{   
    Key_Proc();
    
    Beep_Mode_Handle();
    
    if(SysInfo.Sys_Power_Flag_On)                                //��Դ����
    {
        SysInfo.Sys_Power_Flag_On = 0;  
        SYS_POWER_CON_SET;
    }
    if(SysInfo.Sys_Power_Flag_Off)
    {
        SysInfo.Sys_Power_Flag_Off = 0; 
        SYS_POWER_CON_RESET;
    }
    
    if(SysInfo.Sys_Sleep_Count > 3600)  //30���ӹػ�
    {
        Save_EepromData();        
        SysInfo.Sys_Power_Flag_Off = 1;
    }
    
    if((SysInfo.BK_Light_Flag)&(SysInfo.BK_Light_Count<40))     //��Ļ����0.5s * 40 = 20s
    {
        BK_LIGHT_SET;
    }
    else
    {
        SysInfo.BK_Light_Flag = 0;
        SysInfo.BK_Light_Count = 0;
        BK_LIGHT_RESET;
    }
    
    SysInfo.Dis_Glitter_cnt++;
    if(SysInfo.Dis_Glitter_cnt >15)
    {
        SysInfo.Dis_Glitter_cnt = 0;
        SysInfo.Dis_Glitter_flag = !SysInfo.Dis_Glitter_flag;
    }
}
void Task_3(void)
{
    LCD_Display();
}
void Task_4(void)
{
//    static uint8_t Dis_Glitter_cnt;
    
    if(SysInfo.Sys_Power_Flag)
    {
        Motor_Oper();
        
        Nikon_To_Camera();
        
        //    if(SysInfo.Dis_Glitter_flag_en)
        //    {
        //        SysInfo.Dis_Glitter_flag = 0;        
        //        Dis_Glitter_cnt = 0;
        //        SysInfo.Dis_Glitter_flag_en = 0;
        //        
        //    }
        //    else
        //    {
        //        Dis_Glitter_cnt++;
        //        if(Dis_Glitter_cnt >= 4)
        //        {
        //            Dis_Glitter_cnt = 0;
        //            SysInfo.Dis_Glitter_flag = !SysInfo.Dis_Glitter_flag; 
        //        }       
        //    }
        
        Check_Lamp_Temp();
        Low_Batt_Check();
    }
    else
    {
        CAP_LED_OFF;
    }
}
void Task_5(void)
{
//    if(comm_state.link_cnt != 0)
//    {
//        comm_state.link_cnt = 0;
//        comm_state.link = 1;
//        SysInfo.Sys_Sleep_Count = 0;
//    }
//    else
//    {
//        comm_state.link = 0;
//        //memset(SONY_Receive_DATA,0,sizeof(SONY_Receive_DATA));//�������
//        Enable_SCK_Init();
//    }
    if(SysInfo.Cap_Value_Beep_Count < 9)
    {
        SysInfo.Cap_Value_Beep_Count++;
    }
    else
    {
        SysInfo.Cap_Value_Beep_Count = 9;
    }
    
    if(cmrPara.cls)
    {
        comm_state.link = 1;
        SysInfo.Sys_Sleep_Count = 0;
    }
    else
    {
        comm_state.link = 0;
    }
    
    if(SysInfo.BK_Light_Flag)
    {        
        SysInfo.BK_Light_Count++;
    }
    else
    {
        SysInfo.BK_Light_Count = 0;
    }
    
    SysInfo.Sys_Sleep_Count++;
    
    SysInfo.KEY_AUTO_SET_Count++;
    
    if(SysInfo.KEY_AUTO_SET_Count>20)
    {
        SysInfo.KEY_AUTO_SET_Count = 0;
        Key_SET_Function_Typedef = Key_SET_None;
    } 
}
