#ifndef __KEY_PROCESS_H
#define __KEY_PROCESS_H

#include "stm8l15x.h"

#define KEY_S2_Value    4054   //up
#define KEY_S3_Value    3412   //left
#define KEY_S4_Value    2786   //down
#define KEY_S5_Value    2250   //right
#define KEY_S6_Value    1644   //MODE
#define KEY_S7_Value    1118   //SET

#define KEY_MAX_Offset  100
#define KEY_MIN_Offset  100

#define KEY_S2_MAX_Value       (KEY_S2_Value + KEY_MAX_Offset)
#define KEY_S2_MIN_Value       (KEY_S2_Value - KEY_MIN_Offset)
#define KEY_S3_MAX_Value       (KEY_S3_Value + KEY_MAX_Offset)
#define KEY_S3_MIN_Value       (KEY_S3_Value - KEY_MIN_Offset)
#define KEY_S4_MAX_Value       (KEY_S4_Value + KEY_MAX_Offset)
#define KEY_S4_MIN_Value       (KEY_S4_Value - KEY_MIN_Offset)
#define KEY_S5_MAX_Value       (KEY_S5_Value + KEY_MAX_Offset)
#define KEY_S5_MIN_Value       (KEY_S5_Value - KEY_MIN_Offset)
#define KEY_S6_MAX_Value       (KEY_S6_Value + KEY_MAX_Offset)
#define KEY_S6_MIN_Value       (KEY_S6_Value - KEY_MIN_Offset)
#define KEY_S7_MAX_Value       (KEY_S7_Value + KEY_MAX_Offset)
#define KEY_S7_MIN_Value       (KEY_S7_Value - KEY_MIN_Offset)

#define SYS_POWER_CON_SET       GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define SYS_POWER_CON_RESET     GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define BK_LIGHT_SET            GPIO_SetBits(GPIOD,GPIO_Pin_3)
#define BK_LIGHT_RESET          GPIO_ResetBits(GPIOD,GPIO_Pin_3)

#define _KEY_NONE               0 
#define _REENTER                1 
#define _NO_REENTER             2 

typedef enum                   //�������
{   
    KEY_None,
    KEY_S2,
    KEY_S3,
    KEY_S4,
    KEY_S5, 
    KEY_S6,
    KEY_S7,
    KEY_PILOT,
    KEY_POWER,
    KEY_RESET   
}_Key_Input;

typedef enum _KEY_STATE        //����״̬
{
    _HAS_NO_KEY,                //0:δ����             
    _HAS_KEY_DOWN,              //1:��⵽һ�ΰ���,           
    _HAS_KEY_SURE,              //2:�ּ�⵽һ�ΰ���, ���ζ���⵽����, ����Ϊȷʵ������, �ﵽ������Ŀ��, \
                                    ����������ӿɿ���, ��������״̬���߸�ÿ���������ø�������.             
    _HAS_KEY_WAITUP,            //3:�ȴ���̧��       
}_Key_State;

typedef struct   
{
    uint8_t Key_Short           :1;
    uint8_t Key_Long            :1;
    uint8_t Key_Short_Set       :1; 
}_Key_Flag_Struct;

typedef struct   
{
    uint8_t  Long_Key_t;                                //��������
    uint8_t  LastKey;                                   //���ΰ��µļ�
    uint8_t  PreDownKey;                                //�ϴμ�⵽�ļ� 
    uint8_t  KeyState;                                  //״̬ 
    uint16_t SameKeyCntr;                               //ͬһ����⵽���µĴ��� 
    uint8_t  CurKey;                                    //��ǰ��⵽�ļ�, ���ڴ���������� 
    uint8_t  (*KeyDownCallBack)(uint16_t, uint16_t);    //��ȷ�ϰ��µĻص�����ָ�� 
    void    (*KeyUpCallBack)(uint16_t);                 //��̧��Ļص�����ָ�� 
}struct_KeyInfo;


uint8_t KeyDownCallBack(uint16_t Key, uint16_t Times);  //���������ص����� 
void KeyUpCallBack(uint16_t Key);                       //���������ص����� 
void DitherlessKey(struct_KeyInfo* pInfo);              //�����Ĵ�����
uint8_t Key_Scan(void);
void Key_Stroke(void);
void Key_Proc(void);
void Key_S2_Str(void);
void Key_S2_Long_Str(void);
void Key_S3_Str(void);
void Key_S3_Long_Str(void);
void Key_S4_Str(void);
void Key_S4_Long_Str(void);
void Key_S5_Str(void);
void Key_S5_Long_Str(void);
void Key_S6_Str(void);
void Key_S6_Long_Str(void);
void Key_S7_Str(void);
void Key_S7_Long_Str(void);
void Key_Pilot_Str(void);
void Key_Power_Str(void);
void Key_Power_Long_Str(void);

extern _Key_Flag_Struct        Key_Flag;

#endif

