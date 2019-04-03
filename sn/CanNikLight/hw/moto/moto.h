#ifndef _MOTO_H_

#define MOTO_ENABLE			0X01

#define MOTO_DIR_FORWARD 0x00
#define MOTO_DIR_BACKWARD 0X01

#define MOTO_READY       0X01
#define MOTO_MODE_AUTO      0x00
#define MOTO_MODE_MANU      0x01

#define MOTO_CHARGE_ENABLE_BIT		0x01//0
#define MOTO_CHARGE_DISABLE_BIT		0x02///1/
#define MOTO_STEPS_TIMEOUT			100//0xFF
#define   _MOTO_CHK_4STEP_
#define MOTO_MAX_INDEX			9

#define MOTOIN1   PA_ODR_ODR7
#define MOTOIN2   PG_ODR_ODR0

#define MOTOENA  PG_ODR_ODR1

#define MOTOLOCA PA_IDR_IDR6
#define REF_BOARD_DETECT PC_IDR_IDR7

void MotoTickDriver(void);
void MotoReset(void);


#endif



