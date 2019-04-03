#include "include.h"

#define EEPROM_HEAD_ADDR    0x004000

//��дEEPROMʱ�������ֽڵ�����д�뵽EEPROM���׵�ַ����ʾ������EEPROM��
//�ϵ��EEPROMʱ������⵽EEPROM���׵�ַ���ֽ�Ϊ���ֽ�������ڲ����ݡ�
#define FLAG_VALIDDATA      0x55
u16 g_paraSaveTimer = 0;
u8 g_paraReadBuf[4] = {0}; 

//------------------------------------------------------------------------------------------------------------
// ����: Read_EEPROM ��
// ����: ��EEPROM����������Ч��������� ��
// ���ã��� ��
// ����: �� ��
// ˵��: �ޡ�
// �β�: �� ��
// �β�ȡֵ��Χ: �� ��
//------------------------------------------------------------------------------------------------------------
void Read_EEPROM(u8 *buf, u8 len)
{
	u8 i;	
	  //EEPROM���������
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
		 
	//��EEPROM���׵�ַ����һ���ֽ��Ǳ�־�ֽڣ���֪֮ǰ��EEPROM��д������ݣ�Ӧ������ 
    if(FLASH_ReadByte(EEPROM_HEAD_ADDR) == FLAG_VALIDDATA)
    {
			  //��10�鹲60���ֽڵ����ݴ�EEPROM�ж��� ��
				for(i=0; i<len; i++)
				{
						buf[i] = FLASH_ReadByte(EEPROM_HEAD_ADDR+i+1);		
				}	 
	}
		
	//EEPROM���ϱ�����
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}


//------------------------------------------------------------------------------------------------------------
// ����: Write_EEPROM ��
// ����: ������д�뵽EEPROM�� ��
// ���ã��� ��
// ����: �� ��
// ˵��: �ޡ�
// �β�: �� ��
// �β�ȡֵ��Χ: �� ��
//------------------------------------------------------------------------------------------------------------
void Write_EEPROM(u8 *dat, u8 len)
{
	u8 i;
	 //EEPROM���������
	FLASH_Unlock(FLASH_MEMTYPE_DATA); 
		
	//STM8S003F3һ����128�ֽڵ�EEPROM�ռ䡣
	//д��ǰ��ȫ��������
	for(i=0; i<len; i++)
	{
			 FLASH_EraseByte(EEPROM_HEAD_ADDR + i);
	}
		
	//��EEPROM���׵�ַд���־�ֽڣ���ʾEEPROM��ʹ�ù���������Ч���ݡ�
	FLASH_ProgramByte(EEPROM_HEAD_ADDR, FLAG_VALIDDATA);
	 
	//��10�鹲60���ֽڵ�����д��EEPROM�С�
        for(i=0; i<len; i++)
       {
       		FLASH_ProgramByte(EEPROM_HEAD_ADDR+i+1, dat[i]);			
       }
	
       //EEPROM���ϱ�����
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
