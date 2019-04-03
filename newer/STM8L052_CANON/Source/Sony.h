#ifndef __SONY_H
#define __SONY_H

#include "stm8l15x.h"

#define  SCK_IN                 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
#define  FLASH_IN               GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)

#define  IGBT_DRIVER_Start      GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define  IGBT_DRIVER_Stop       GPIO_ResetBits(GPIOE, GPIO_Pin_7)

#define AF_LED_ON               GPIO_SetBits(GPIOC, GPIO_Pin_5)
#define AF_LED_OFF              GPIO_ResetBits(GPIOC, GPIO_Pin_5)

typedef struct  COMM_STATE
{
    unsigned char send_flag              :1;
    unsigned char receive_flag           :1;
    unsigned char preflash1_flag         :1;       //有预闪标志
    unsigned char preflash2_flag         :1;
    unsigned char cmd_ctrl_flash_flag    :1;
    unsigned char cmd_ctrl_preflash_flag :1;
    unsigned char link                   :1;       //和相机通信成功
    unsigned char Timeout_Hander_Flag    :1;       //通信超时标志
    unsigned char Timeout_Send_Flag      :1;
    unsigned char Timeout_receive_Flag   :1;
    unsigned char Timeout_cnt;
    unsigned char link_cnt;
    unsigned char Send_packet_cnt;
    unsigned char receive_packet_cnt;
}_Comm_State;

extern uint8_t SONY_Send_DATA_F60M[26];
extern uint8_t SONY_Receive_DATA[14];
extern uint8_t SONY_Receive2_DATA[26];
extern uint8_t SONY_Receive_DATA[14];
extern uint8_t SONY_Receive_Pre_Data[14];
extern uint8_t SONY_Receive_Fla_Data[14];
//uint8_t SONY_Receive_Cmd_Pre_Data[14]={0};
//uint8_t SONY_Receive_Cmd_Fla_Data[14]={0};

extern _Comm_State              comm_state;
extern _Flash_Time_TypeDef      Flash_Time_Group[];

uint8_t Flash_Time_out(uint8_t value);
void TTL_Prefrash(uint8_t data);
void Formalfrash(uint16_t time);
void Enable_SCK_Init(void);
void Disable_SCK_Init(void);
void Disable_SPI(void);
void TimeOut_Start(uint16_t time);
void GPIO_Pin_1_Callback(void);
void GPIO_Pin_4_Callback(void);
void GPIO_Pin_5_Callback(void);
void TIM2_Callback(void);
void SPI1_IT_Callback(void);
void Formalfrash(uint16_t time);
void Multiflash(void);
void Multifrash_Handle(void);
void Txbuf_Change_Mode(void);

#endif