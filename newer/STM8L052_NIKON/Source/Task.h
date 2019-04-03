#ifndef __TASK_H
#define __TASK_H

#include "stm8l15x.h"

void ComMonitor(void);

void Task_1(void);
void Task_2(void);
void Task_3(void);
void Task_4(void);
void Task_5(void);

void TaskProcess(void);
void HAL_SYSTICK_Callback(void);

extern __IO uint32_t uwTick;

#endif


