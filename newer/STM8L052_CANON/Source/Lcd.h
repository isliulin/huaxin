#ifndef __LCD_H
#define __LCD_H

#include "stm8l15x.h"

//////////////////////////////////////////////////////////////////
//                        HT1621B宏定义                         //
//////////////////////////////////////////////////////////////////
#define SYSDIS          0x00    // 关振系统荡器和LCD偏压发生器
#define SYSEN           0x01    // 打开系统振荡器
#define LCDOFF          0x02    // 关LCD偏压
#define LCDON           0x03    // 打开LCD偏压
#define TIMEDIS         0x04    // 禁止时基输出
#define WDTDIS          0x05    // 禁止WDT超时标志输出
#define TIMEREN         0x08    // 启用时基输出
#define WDTEN           0x0D    // 启用WDT超时标志输出
#define TONEOFF         0x08    // 关闭声音输出
#define CLRTIMER        0x05    // 清除时基发生器的内容
#define CLRWDT          0x0F    // 清除WDT的内容
#define XTAL32K         0x14    // 系统时钟源，片上RC振荡器
#define EXT32K          0x1C    // 系统时钟源，外部时钟源
#define RC256K          0x18    //
#define EXT256K         0x1C
#define BIAS1_2         0x28    // 1/2 BIAS 4commons
#define BIAS1_3         0x29    // 1/3 BIAS 4commons
#define TONE4K          0x40    // 音的输出频率：4kHz的
#define TONE2K          0x60    // 音的输出频率：2kHz的
#define IRQDIS          0x80    // 禁止IRQ输出
#define IRQEN           0x88    // 启用IRQ输出
#define F1              0xA0    // 时基时钟输出：2Hz WDT的超时标志后：4s
#define F2              0xA1    // 时基时钟输出：2Hz WDT的超时标志后：2s
#define F4              0xA2    // 时基时钟输出：4Hz WDT的超时标志后：1s
#define F8              0xA3    // 时基时钟输出：8Hz WDT的超时标志后：1/2s
#define F16             0xA4    // 时基时钟输出：16Hz WDT的超时标志后：1/4s
#define F32             0xA5    // 时基时钟输出：32Hz WDT的超时标志后：1/8s
#define F64             0xA6    // 时基时钟输出：64Hz WDT的超时标志后：1/16s
#define F128            0xA7    // 时基时钟输出：128Hz WDT的超时标志后：1/32s
#define TEST            0xE0    // 测试模式下，用户不使用。
#define NORMAL          0xE3    // 普通模式

#define _Nop()                 {nop();nop();nop();}

#define TM1621_DAT_HIGH         GPIOE->ODR |= GPIO_Pin_3;
#define TM1621_DAT_LOW          GPIOE->ODR &= (uint8_t)(~GPIO_Pin_3);
#define TM1621_WR_HIGH          GPIOE->ODR |= GPIO_Pin_4;
#define TM1621_WR_LOW           GPIOE->ODR &= (uint8_t)(~GPIO_Pin_4);
#define TM1621_CS_HIGH          GPIOE->ODR |= GPIO_Pin_5;
#define TM1621_CS_LOW           GPIOE->ODR &= (uint8_t)(~GPIO_Pin_5);

#define NC              0
#define Minus           1
#define Positive        2

//////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_Dis_S01_ON          {LCD_Data[10] |= 0x08;}
#define LCD_Dis_S01_OFF         {LCD_Data[10] &= 0xF7;}
#define LCD_Dis_S02_ON          {LCD_Data[10] |= 0x04;}
#define LCD_Dis_S02_OFF         {LCD_Data[10] &= 0xFB;}
#define LCD_Dis_S03_ON          {LCD_Data[10] |= 0x02;}
#define LCD_Dis_S03_OFF         {LCD_Data[10] &= 0xFD;}
#define LCD_Dis_S04_ON          {LCD_Data[10] |= 0x01;}
#define LCD_Dis_S04_OFF         {LCD_Data[10] &= 0xFE;}
#define LCD_Dis_S05_ON          {LCD_Data[6]  |= 0x10;}
#define LCD_Dis_S05_OFF         {LCD_Data[6]  &= 0xEF;}
#define LCD_Dis_S06_ON          {LCD_Data[6]  |= 0x20;}
#define LCD_Dis_S06_OFF         {LCD_Data[6]  &= 0xDF;}
#define LCD_Dis_S07_ON          {LCD_Data[0]  |= 0x40;}
#define LCD_Dis_S07_OFF         {LCD_Data[0]  &= 0xBF;}
#define LCD_Dis_S08_ON          {LCD_Data[0]  |= 0x20;}
#define LCD_Dis_S08_OFF         {LCD_Data[0]  &= 0xDF;}
#define LCD_Dis_S09_ON          {LCD_Data[0]  |= 0x10;}
#define LCD_Dis_S09_OFF         {LCD_Data[0]  &= 0xEF;}
#define LCD_Dis_S10_ON          {LCD_Data[7]  |= 0x10;}
#define LCD_Dis_S10_OFF         {LCD_Data[7]  &= 0xEF;}
#define LCD_Dis_S11_ON          {LCD_Data[8]  |= 0x10;}
#define LCD_Dis_S11_OFF         {LCD_Data[8]  &= 0xEF;}
#define LCD_Dis_S12_ON          {LCD_Data[9]  |= 0x10;}
#define LCD_Dis_S12_OFF         {LCD_Data[9]  &= 0xEF;}
#define LCD_Dis_S13_ON          {LCD_Data[10] |= 0x10;}
#define LCD_Dis_S13_OFF         {LCD_Data[10] &= 0xEF;}
#define LCD_Dis_S14_ON          {LCD_Data[6]  |= 0x40;}
#define LCD_Dis_S14_OFF         {LCD_Data[6]  &= 0xBF;}
#define LCD_Dis_S15_ON          {LCD_Data[0]  |= 0x80;}
#define LCD_Dis_S15_OFF         {LCD_Data[0]  &= 0x7F;}
#define LCD_Dis_S16_ON          {LCD_Data[0]  |= 0x04;}
#define LCD_Dis_S16_OFF         {LCD_Data[0]  &= 0xFB;}
#define LCD_Dis_S17_ON          {LCD_Data[0]  |= 0x02;}
#define LCD_Dis_S17_OFF         {LCD_Data[0]  &= 0xFD;}
#define LCD_Dis_S18_ON          {LCD_Data[0]  |= 0x01;}
#define LCD_Dis_S18_OFF         {LCD_Data[0]  &= 0xFE;}
#define LCD_Dis_S19_ON          {LCD_Data[4]  |= 0x80;}
#define LCD_Dis_S19_OFF         {LCD_Data[4]  &= 0x7F;}
#define LCD_Dis_S20_ON          {LCD_Data[5]  |= 0x80;}
#define LCD_Dis_S20_OFF         {LCD_Data[5]  &= 0x7F;}
#define LCD_Dis_S21_ON          {LCD_Data[0]  |= 0x08;}
#define LCD_Dis_S21_OFF         {LCD_Data[0]  &= 0xF7;}
#define LCD_Dis_S22_ON          {LCD_Data[1]  |= 0x80;}
#define LCD_Dis_S22_OFF         {LCD_Data[1]  &= 0x7F;}
#define LCD_Dis_S23_ON          {LCD_Data[3]  |= 0x08;}
#define LCD_Dis_S23_OFF         {LCD_Data[3]  &= 0xF7;}
#define LCD_Dis_S24_ON          {LCD_Data[2]  |= 0x08;}
#define LCD_Dis_S24_OFF         {LCD_Data[2]  &= 0xF7;}
#define LCD_Dis_S25_ON          {LCD_Data[1]  |= 0x80;}
#define LCD_Dis_S25_OFF         {LCD_Data[1]  &= 0x7F;}
#define LCD_Dis_S26_ON          {LCD_Data[1]  |= 0x40;}
#define LCD_Dis_S26_OFF         {LCD_Data[1]  &= 0xBF;}
#define LCD_Dis_S27_ON          {LCD_Data[1]  |= 0x20;}
#define LCD_Dis_S27_OFF         {LCD_Data[1]  &= 0xDF;}
#define LCD_Dis_S28_ON          {LCD_Data[1]  |= 0x10;}
#define LCD_Dis_S28_OFF         {LCD_Data[1]  &= 0x7F;}
#define LCD_Dis_S29_ON          {LCD_Data[1]  |= 0x01;}
#define LCD_Dis_S29_OFF         {LCD_Data[1]  &= 0xF7;}
#define LCD_Dis_S30_ON          {LCD_Data[1]  |= 0x02;}
#define LCD_Dis_S30_OFF         {LCD_Data[1]  &= 0xFD;}
#define LCD_Dis_S31_ON          {LCD_Data[1]  |= 0x04;}
#define LCD_Dis_S31_OFF         {LCD_Data[1]  &= 0xFB;}
#define LCD_Dis_S32_ON          {LCD_Data[6]  |= 0x80;}
#define LCD_Dis_S32_OFF         {LCD_Data[6]  &= 0x7F;}
//--------------------------------------------NUM1---------------------------------------------//1
#define LCD_Digital1_OFF        {LCD_Data[6] &= 0xF0;LCD_Data[7] &= 0x1F;}
#define LCD_Digital1_0          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0F;LCD_Data[7] |= 0xA0;} 
#define LCD_Digital1_1          {LCD_Digital1_OFF;LCD_Data[6] |= 0x00;LCD_Data[7] |= 0xA0;}
#define LCD_Digital1_2          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0B;LCD_Data[7] |= 0xC0;}
#define LCD_Digital1_3          {LCD_Digital1_OFF;LCD_Data[6] |= 0x09;LCD_Data[7] |= 0xE0;}
#define LCD_Digital1_4          {LCD_Digital1_OFF;LCD_Data[6] |= 0x04;LCD_Data[7] |= 0xE0;}
#define LCD_Digital1_5          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0D;LCD_Data[7] |= 0x60;}
#define LCD_Digital1_6          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0F;LCD_Data[7] |= 0x60;}
#define LCD_Digital1_7          {LCD_Digital1_OFF;LCD_Data[6] |= 0x08;LCD_Data[7] |= 0xA0;}
#define LCD_Digital1_8          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0F;LCD_Data[7] |= 0xE0;}
#define LCD_Digital1_9          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0D;LCD_Data[7] |= 0xE0;}
#define LCD_Digital1_P          {LCD_Digital1_OFF;LCD_Data[6] |= 0x0E;LCD_Data[7] |= 0xC0;}
//--------------------------------------------NUM2---------------------------------------------//2
#define LCD_Digital2_OFF        {LCD_Data[7] &= 0xF0;LCD_Data[8] &= 0x1F;}
#define LCD_Digital2_0          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0F;LCD_Data[8] |= 0xA0;}
#define LCD_Digital2_1          {LCD_Digital2_OFF;LCD_Data[7] |= 0x00;LCD_Data[8] |= 0xA0;}
#define LCD_Digital2_2          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0B;LCD_Data[8] |= 0xC0;}
#define LCD_Digital2_3          {LCD_Digital2_OFF;LCD_Data[7] |= 0x09;LCD_Data[8] |= 0xE0;}
#define LCD_Digital2_4          {LCD_Digital2_OFF;LCD_Data[7] |= 0x04;LCD_Data[8] |= 0xE0;}
#define LCD_Digital2_5          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0D;LCD_Data[8] |= 0x60;}
#define LCD_Digital2_6          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0F;LCD_Data[8] |= 0x60;}
#define LCD_Digital2_7          {LCD_Digital2_OFF;LCD_Data[7] |= 0x08;LCD_Data[8] |= 0xA0;}
#define LCD_Digital2_8          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0F;LCD_Data[8] |= 0xE0;}
#define LCD_Digital2_9          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0D;LCD_Data[8] |= 0xE0;}
#define LCD_Digital2_O          {LCD_Digital2_OFF;LCD_Data[7] |= 0x0F;LCD_Data[8] |= 0xA0;}
//--------------------------------------------NUM3---------------------------------------------//3
#define LCD_Digital3_OFF        {LCD_Data[8] &= 0xF0;LCD_Data[9] &= 0x1F;}
#define LCD_Digital3_0          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0F;LCD_Data[9] |= 0xA0;}
#define LCD_Digital3_1          {LCD_Digital3_OFF;LCD_Data[8] |= 0x00;LCD_Data[9] |= 0xA0;}
#define LCD_Digital3_2          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0B;LCD_Data[9] |= 0xC0;}
#define LCD_Digital3_3          {LCD_Digital3_OFF;LCD_Data[8] |= 0x09;LCD_Data[9] |= 0xE0;}
#define LCD_Digital3_4          {LCD_Digital3_OFF;LCD_Data[8] |= 0x04;LCD_Data[9] |= 0xE0;}
#define LCD_Digital3_5          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0D;LCD_Data[9] |= 0x60;}
#define LCD_Digital3_6          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0F;LCD_Data[9] |= 0x60;}
#define LCD_Digital3_7          {LCD_Digital3_OFF;LCD_Data[8] |= 0x08;LCD_Data[9] |= 0xA0;}
#define LCD_Digital3_8          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0F;LCD_Data[9] |= 0xE0;}
#define LCD_Digital3_9          {LCD_Digital3_OFF;LCD_Data[8] |= 0x0D;LCD_Data[9] |= 0xE0;}
#define LCD_Digital3__          {LCD_Digital3_OFF;LCD_Data[8] |= 0x00;LCD_Data[9] |= 0x40;}
//--------------------------------------------NUM4---------------------------------------------//4
#define LCD_Digital4_OFF        {LCD_Data[9] &= 0xF0;LCD_Data[10] &= 0x1F;}
#define LCD_Digital4_0          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0F;LCD_Data[10] |= 0xA0;}
#define LCD_Digital4_1          {LCD_Digital4_OFF;LCD_Data[9] |= 0x00;LCD_Data[10] |= 0xA0;}
#define LCD_Digital4_2          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0B;LCD_Data[10] |= 0xC0;}
#define LCD_Digital4_3          {LCD_Digital4_OFF;LCD_Data[9] |= 0x09;LCD_Data[10] |= 0xE0;}
#define LCD_Digital4_4          {LCD_Digital4_OFF;LCD_Data[9] |= 0x04;LCD_Data[10] |= 0xE0;}
#define LCD_Digital4_5          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0D;LCD_Data[10] |= 0x60;}
#define LCD_Digital4_6          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0F;LCD_Data[10] |= 0x60;}
#define LCD_Digital4_7          {LCD_Digital4_OFF;LCD_Data[9] |= 0x08;LCD_Data[10] |= 0xA0;}
#define LCD_Digital4_8          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0F;LCD_Data[10] |= 0xE0;}
#define LCD_Digital4_9          {LCD_Digital4_OFF;LCD_Data[9] |= 0x0D;LCD_Data[10] |= 0xE0;}
#define LCD_Digital4__          {LCD_Digital4_OFF;LCD_Data[9] |= 0x00;LCD_Data[10] |= 0x40;}
//--------------------------------------------NUM5---------------------------------------------//5
#define LCD_Digital5_OFF        {LCD_Data[5] &= 0x80;}                 
#define LCD_Digital5_0          {LCD_Digital5_OFF;LCD_Data[5] |= 0x5F;}
#define LCD_Digital5_1          {LCD_Digital5_OFF;LCD_Data[5] |= 0x50;}
#define LCD_Digital5_2          {LCD_Digital5_OFF;LCD_Data[5] |= 0x3D;}
#define LCD_Digital5_3          {LCD_Digital5_OFF;LCD_Data[5] |= 0x79;}
#define LCD_Digital5_4          {LCD_Digital5_OFF;LCD_Data[5] |= 0x72;}
#define LCD_Digital5_5          {LCD_Digital5_OFF;LCD_Data[5] |= 0x6B;}
#define LCD_Digital5_6          {LCD_Digital5_OFF;LCD_Data[5] |= 0x6F;}
#define LCD_Digital5_7          {LCD_Digital5_OFF;LCD_Data[5] |= 0x51;}
#define LCD_Digital5_8          {LCD_Digital5_OFF;LCD_Data[5] |= 0x7F;}
#define LCD_Digital5_9          {LCD_Digital5_OFF;LCD_Data[5] |= 0x7B;}
//--------------------------------------------NUM6---------------------------------------------//6
#define LCD_Digital6_OFF        {LCD_Data[4] &= 0x80;}                 
#define LCD_Digital6_0          {LCD_Digital6_OFF;LCD_Data[4] |= 0x5F;}
#define LCD_Digital6_1          {LCD_Digital6_OFF;LCD_Data[4] |= 0x50;}
#define LCD_Digital6_2          {LCD_Digital6_OFF;LCD_Data[4] |= 0x3D;}
#define LCD_Digital6_3          {LCD_Digital6_OFF;LCD_Data[4] |= 0x79;}
#define LCD_Digital6_4          {LCD_Digital6_OFF;LCD_Data[4] |= 0x72;}
#define LCD_Digital6_5          {LCD_Digital6_OFF;LCD_Data[4] |= 0x6B;}
#define LCD_Digital6_6          {LCD_Digital6_OFF;LCD_Data[4] |= 0x6F;}
#define LCD_Digital6_7          {LCD_Digital6_OFF;LCD_Data[4] |= 0x51;}
#define LCD_Digital6_8          {LCD_Digital6_OFF;LCD_Data[4] |= 0x7F;}
#define LCD_Digital6_9          {LCD_Digital6_OFF;LCD_Data[4] |= 0x7B;}
//--------------------------------------------NUM7---------------------------------------------//7
#define LCD_Digital7_OFF        {LCD_Data[2] &= 0x08;}                 
#define LCD_Digital7_0          {LCD_Digital7_OFF;LCD_Data[2] |= 0xF5;}
#define LCD_Digital7_1          {LCD_Digital7_OFF;LCD_Data[2] |= 0x05;}
#define LCD_Digital7_2          {LCD_Digital7_OFF;LCD_Data[2] |= 0xB6;}
#define LCD_Digital7_3          {LCD_Digital7_OFF;LCD_Data[2] |= 0x97;}
#define LCD_Digital7_4          {LCD_Digital7_OFF;LCD_Data[2] |= 0x47;}
#define LCD_Digital7_5          {LCD_Digital7_OFF;LCD_Data[2] |= 0xD3;}
#define LCD_Digital7_6          {LCD_Digital7_OFF;LCD_Data[2] |= 0xF3;}
#define LCD_Digital7_7          {LCD_Digital7_OFF;LCD_Data[2] |= 0x85;}
#define LCD_Digital7_8          {LCD_Digital7_OFF;LCD_Data[2] |= 0xF7;}
#define LCD_Digital7_9          {LCD_Digital7_OFF;LCD_Data[2] |= 0xD7;}
#define LCD_Digital7__          {LCD_Digital7_OFF;LCD_Data[2] |= 0x02;}
//--------------------------------------------NUM8---------------------------------------------//8
#define LCD_Digital8_OFF        {LCD_Data[3] &= 0x08;}                 
#define LCD_Digital8_0          {LCD_Digital8_OFF;LCD_Data[3] |= 0xF5;}
#define LCD_Digital8_1          {LCD_Digital8_OFF;LCD_Data[3] |= 0x05;}
#define LCD_Digital8_2          {LCD_Digital8_OFF;LCD_Data[3] |= 0xB6;}
#define LCD_Digital8_3          {LCD_Digital8_OFF;LCD_Data[3] |= 0x97;}
#define LCD_Digital8_4          {LCD_Digital8_OFF;LCD_Data[3] |= 0x47;}
#define LCD_Digital8_5          {LCD_Digital8_OFF;LCD_Data[3] |= 0xD3;}
#define LCD_Digital8_6          {LCD_Digital8_OFF;LCD_Data[3] |= 0xF3;}
#define LCD_Digital8_7          {LCD_Digital8_OFF;LCD_Data[3] |= 0x85;}
#define LCD_Digital8_8          {LCD_Digital8_OFF;LCD_Data[3] |= 0xF7;}
#define LCD_Digital8_9          {LCD_Digital8_OFF;LCD_Data[3] |= 0xD7;}
#define LCD_Digital8__          {LCD_Digital8_OFF;LCD_Data[3] |= 0x02;}

extern unsigned char LCD_Data[16];

void TM1621_Init(void);
void TM1621_Beep(uint8_t Beep, uint8_t Hz);
void LCD_Dis_All_Clear(void);
void LCD_Display_SLEEP(void);
void LCD_Display(void);
void TM1621WrCmd(unsigned char Cmd);
void TM1621WrAllData(unsigned char Addr,unsigned char *p,unsigned char cnt);

static void DisPlay_M_Gear(int8_t M_Gear, uint8_t Large_num, int8_t Ev_num);
static void BuzLock_Hndler(void);
static void Mode_Handler(void);
static void LCD_Handler(void);
static void Display_CH(uint8_t Num);
static void Display_Multi_Hz(uint8_t Num);
static void Display_Multi_Times(uint8_t Num);
static void Display_Zoom(uint8_t Num);
static void Display_Ctrl(uint8_t Num);
static void Display_Rmt(uint8_t Num);
static void Display_Rmt2(uint8_t Num);
static void Display_Aper(uint16_t Num);
void Dis_Frash_Times(uint16_t num);

#endif