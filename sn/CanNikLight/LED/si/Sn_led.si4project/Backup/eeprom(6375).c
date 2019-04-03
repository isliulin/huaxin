#include "include.h"

#define EEPROM_HEAD_ADDR    0x004000

//当写EEPROM时，将此字节的数据写入到EEPROM的首地址，表示操作过EEPROM。
//上电读EEPROM时，若检测到EEPROM的首地址的字节为此字节则读出内部数据。
#define FLAG_VALIDDATA      0x55
u16 g_paraSaveTimer = 0;
u8 g_paraReadBuf[4] = {0}; 

//------------------------------------------------------------------------------------------------------------
// 名称: Read_EEPROM 。
// 功能: 读EEPROM，若内有有效数据则读出 。
// 调用：无 。
// 返回: 无 。
// 说明: 无。
// 形参: 无 。
// 形参取值范围: 无 。
//------------------------------------------------------------------------------------------------------------
void Read_EEPROM(u8 *buf, u8 len)
{
	u8 i;	
	  //EEPROM解除保护。
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
		 
	//若EEPROM的首地址即第一个字节是标志字节，可知之前向EEPROM中写入过数据，应读出。 
    if(FLASH_ReadByte(EEPROM_HEAD_ADDR) == FLAG_VALIDDATA)
    {
			  //将10组共60个字节的数据从EEPROM中读出 。
				for(i=0; i<len; i++)
				{
						buf[i] = FLASH_ReadByte(EEPROM_HEAD_ADDR+i+1);		
				}	 
	}
		
	//EEPROM加上保护。
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}


//------------------------------------------------------------------------------------------------------------
// 名称: Write_EEPROM 。
// 功能: 将数据写入到EEPROM中 。
// 调用：无 。
// 返回: 无 。
// 说明: 无。
// 形参: 无 。
// 形参取值范围: 无 。
//------------------------------------------------------------------------------------------------------------
void Write_EEPROM(u8 *dat, u8 len)
{
	u8 i;
	 //EEPROM解除保护。
	FLASH_Unlock(FLASH_MEMTYPE_DATA); 
		
	//STM8S003F3一共有128字节的EEPROM空间。
	//写入前，全部擦除。
	for(i=0; i<len; i++)
	{
			 FLASH_EraseByte(EEPROM_HEAD_ADDR + i);
	}
		
	//在EEPROM的首地址写入标志字节，表示EEPROM被使用过，内有有效数据。
	FLASH_ProgramByte(EEPROM_HEAD_ADDR, FLAG_VALIDDATA);
	 
	//将10组共60个字节的数据写入EEPROM中。
        for(i=0; i<len; i++)
       {
       		FLASH_ProgramByte(EEPROM_HEAD_ADDR+i+1, dat[i]);			
       }
	
       //EEPROM加上保护。
       FLASH_Lock(FLASH_MEMTYPE_DATA);
}

u8 IS_PARA_NEED_SAVE()
{
	//u8 tmp;
	if (g_rfChan != g_paraReadBuf[0])
	{
		return 1;
	}

	if (g_isLedON != g_paraReadBuf[1])
	{
		return 1;
	}

	if (g_pwm_chan1_duty!=g_paraReadBuf[2])
	{
		return 1;
	}

	if (g_pwm_chan2_duty!=g_paraReadBuf[3])
	{
		return 1;
	}

/*
	tmp = g_pwm_chan1_duty/PWM_PULSE_WIDTH_STEP;
	if (tmp!=g_paraReadBuf[2])
	{
		return 1;
	}

	tmp = g_pwm_chan2_duty/PWM_PULSE_WIDTH_STEP;
	if (tmp!=g_paraReadBuf[3])
	{
		return 1;
	}*/
    return 0;

}

void CopyParam()
{
	g_paraReadBuf[0] = g_rfChan;
	g_paraReadBuf[1] = g_isLedON;
	//g_paraReadBuf[2] = (g_pwm_chan1_duty/PWM_PULSE_WIDTH_STEP);
	//g_paraReadBuf[3] = (g_pwm_chan2_duty/PWM_PULSE_WIDTH_STEP);
	g_paraReadBuf[2] = (g_pwm_chan1_duty);
	g_paraReadBuf[3] = (g_pwm_chan2_duty);
	
}

void ParaSaveLoop()
{
	g_paraSaveTimer ++;
	if (g_paraSaveTimer > 1000)
	{
		g_paraSaveTimer = 0;
		if (IS_PARA_NEED_SAVE())
		{
			CopyParam();
			Write_EEPROM(g_paraReadBuf,4);
		}
	}
}

void ParaReadProc()
{
	Read_EEPROM(g_paraReadBuf,4);
	if (g_paraReadBuf[0] > 16)
	{
		g_paraReadBuf[0] = 0;
	}

	if (g_paraReadBuf[1] > 1)
	{
		g_paraReadBuf[1] = 0;
	}

	if (g_paraReadBuf[2] > 100)
	{
		g_paraReadBuf[2] = 0;
	}

	if (g_paraReadBuf[3] > 100)
	{
		g_paraReadBuf[3] = 0;
	}

	g_rfChan = g_paraReadBuf[0];
	g_isLedON = g_paraReadBuf[1];
	//g_pwm_chan1_duty = g_paraReadBuf[2]*PWM_PULSE_WIDTH_STEP;
	//g_pwm_chan2_duty = g_paraReadBuf[3]*PWM_PULSE_WIDTH_STEP;
	g_pwm_chan1_duty = g_paraReadBuf[2];
	g_pwm_chan2_duty = g_paraReadBuf[3];
	
}
