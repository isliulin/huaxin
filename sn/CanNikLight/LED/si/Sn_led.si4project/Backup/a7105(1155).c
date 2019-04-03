#include "a7105.h"
#include "sys.h"
#include "include.h"

//const u8 g_a7105_id1[4] = {0x35,0x99,0x9a,0x5a};
//u8 g_a7105_id1[4] = {0x35,0x99,0x9a,0x5a};
const u8 g_a7105_id1[4] = {0xA4,0x11,0x66,0xf4};
const u8 g_a7105_id2[4] = {0xC2,0x4D,0xDE,0xE4};
const u8 g_a7105_id3[4] = {0xCD,0xB3,0xAD,0x9B};            // PreFLASH

extern u8 g_rfTtl;
u8 g_rfStatue = 0;
u8 g_rfLinkTag = 0;
u16 g_rfLinkTmr = 0;
u8 g_rfRcvSetTag = 0;
extern u8 ttl_pre_dbg;
u8 g_rfChan;

void RfTest(void);

void Spi_Send_byte(u8 dat)
{
	u8 i = 0;
	//A7105_CS = 0;
	for (i=0;i<8;i++)
	{	
		if (dat & 0x80)
		{
			A7105_ODR(SET);
		}else
		{
			A7105_ODR(RESET);
		}
		dat <<= 1;
		//A7105_SCK = 1;
		//A7105_SCK = 0;
		A7105_SCK(SET);
		A7105_SCK(RESET);
	}
	//A7105_CS = 1;
}

u8 Spi_Rcv_byte(void)
{
	u8 i = 0;
	u8 rcvDat = 0;
	for (i=8;i>0;i--)
	{
		/*if (A7105_IDR)
		{
			rcvDat |= (1 << i);
		}*/
		rcvDat |= (A7105_IDR() << (i-1));
		//A7105_SCK = 1;
		//A7105_SCK = 0;
		A7105_SCK(SET);
		A7105_SCK(RESET);
	}
	return rcvDat;
}

void A7105_strobe_cmd(u8 cmd)
{
	//A7105_SCK = 0;
	//A7105_CS = 0;
	A7105_SCK(RESET);
	A7105_CS(RESET);
	Spi_Send_byte(cmd);
	//A7105_CS = 1;
	A7105_CS(SET);
}

void A7105_Write_Reg(u8 addr, u8 val)
{
	//A7105_SCK = 0;
	//A7105_CS = 0;
	A7105_SCK(RESET);
	A7105_CS(RESET);
	Spi_Send_byte(addr);
	Spi_Send_byte(val);
	//A7105_CS = 1;
	A7105_CS(SET);
}

u8 A7105_Read_Reg(u8 addr)
{
	u8 val;
	A7105_SCK(RESET);//A7105_SCK = 0;
	A7105_CS(RESET);//A7105_CS = 0;
	//PD_CR2_C23 = 0;
	A7105_DDR(RESET);//A7105_DDR = 0;
	Spi_Send_byte(addr|0x40);
	val = Spi_Rcv_byte();
	A7105_CS(SET);//A7105_CS = 1;
	return val;
}

void A7105_Write_Ram(u8 addr, const u8 * ram, u8 len)
{
	u8 i = 0;
	A7105_SCK(RESET);//A7105_SCK = 0;
	A7105_CS(RESET);//A7105_CS = 0;
	Spi_Send_byte(addr);
	for (i=0;i<len;i++)
	{
		Spi_Send_byte(ram[i]);
	}
	//A7105_CS = 1;
	A7105_CS(SET);
}

void A7105_Read_Ram(u8 addr, u8 * ram, u8 len)
{
	u8 i = 0;
	//PD_CR2_C23 = 0;
	A7105_DDR(RESET);//A7105_DDR = 0;
	A7105_SCK(RESET);//A7105_SCK = 0;
	A7105_CS(RESET);//A7105_CS = 0;
	Spi_Send_byte(0x40|addr);
	for (i=0;i<len;i++)
	{
		ram[i] = Spi_Rcv_byte();
	}
	A7105_CS(SET);//A7105_CS = 1;
}

void A7105_Probe(void)
{
	

}

u8 memcmp(u8* src, u8 *dest, u8 len)
{
	u8 i = 0;
	for (i=0;i<len;i++)
	{
		if (src[i]!=dest[i])
		{
			return (i+1);
		}
	}

	return 0;
}

void A7105_Init(void)
{
	//u8 rdBuf[32];
	u8 idBuf[4];
	u8 tm = 0;
	//u8 i;

	//A7105_RX_AF = 1;
	A7105_RX_AF(SET);

	A7105_Write_Reg(0x00, 0x00);
	A7105_Write_Reg(0x01, 0x62);
	A7105_Write_Reg(0x02, 0x00);
	A7105_Write_Reg(0x03, 0x0b);
	A7105_Write_Reg(0x04, 0x00);
	A7105_Write_Reg(0x07, 0x00);
	A7105_Write_Reg(0x08, 0x00);
	A7105_Write_Reg(0x09, 0x00);
	A7105_Write_Reg(0x0a, 0x00);
	A7105_Write_Reg(0x0b, 0x01);
	A7105_Write_Reg(0x0c, 0x01);
	A7105_Write_Reg(0x0d, 0x05);
	A7105_Write_Reg(0x0e, 0x00);
	A7105_Write_Reg(0x0f, 0x50);
	A7105_Write_Reg(0x10, 0x9e);
	A7105_Write_Reg(0x11, 0x4b);
	A7105_Write_Reg(0x12, 0x00);
	A7105_Write_Reg(0x13, 0x02);
	A7105_Write_Reg(0x14, 0x16);

	A7105_Write_Reg(0x15, 0x2b);
	A7105_Write_Reg(0x16, 0x12);
	A7105_Write_Reg(0x17, 0x00);
	A7105_Write_Reg(0x18, 0x62);
	A7105_Write_Reg(0x19, 0x80);
	A7105_Write_Reg(0x1a, 0x80);
	A7105_Write_Reg(0x1b, 0x00);
	A7105_Write_Reg(0x1c, 0x0a);
	A7105_Write_Reg(0x1d, 0x32);
	A7105_Write_Reg(0x1e, 0xc3);
	A7105_Write_Reg(0x1f, 0x05);
	A7105_Write_Reg(0x20, 0x16);
	A7105_Write_Reg(0x21, 0x00);
	A7105_Write_Reg(0x22, 0x00);
	A7105_Write_Reg(0x24, 0x00);
	A7105_Write_Reg(0x25, 0x00);
	A7105_Write_Reg(0x26, 0x3b);
	A7105_Write_Reg(0x27, 0x00);
	A7105_Write_Reg(0x28, 0x1f);

	A7105_Write_Reg(0x29, 0x47);
	A7105_Write_Reg(0x2a, 0x80);
	A7105_Write_Reg(0x2b, 0x03);
	A7105_Write_Reg(0x2c, 0x01);
	A7105_Write_Reg(0x2d, 0x45);
	A7105_Write_Reg(0x2e, 0x18);
	A7105_Write_Reg(0x2f, 0x00);
	A7105_Write_Reg(0x30, 0x01);
	A7105_Write_Reg(0x31, 0x0f);

	A7105_Write_Reg(0x0b, 0x19);

	A7105_strobe_cmd(0xa0);	

	A7105_Write_Reg(0x02,0x01);	

	for (tm=0;tm<10;tm++)
	{
		if (0 == (0x01 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}

	A7105_Read_Reg(0x22);
	A7105_Write_Reg(0x24,0x13);
	A7105_Write_Reg(0x26,0x3b);
	A7105_Write_Reg(0x0f,0x00);
	A7105_Write_Reg(0x02,0x02);

#if 0
	while(0x02 & A7105_Read_Reg(0x02))
	{
		delay_ms(10);
	}
#else
	for (tm=0;tm<100;tm++)
	{
		if (0 == (0x02 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}
#endif

	A7105_Read_Reg(0x25);
	A7105_Write_Reg(0x0f,0xa0);
	A7105_Write_Reg(0x02,0x02);
/*
	while(0x02 & A7105_Read_Reg(0x02))
	{
		delay_ms(10)
	}
*/
	for (tm=0;tm<10;tm++)
	{
		if (0 == (0x02 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}
	
	A7105_Read_Reg(0x25);
	
	
	A7105_Write_Ram(0x06, g_a7105_id1,4);
	A7105_Read_Ram(0x06, idBuf,4);

	//A7105_strobe_cmd(0xC0);
	//A7105_Write_Reg(0x0f,0x1e);

	//A7105_Write_Ram(0x06, g_a7105_id2,4);
	//A7105_Write_Reg(0x03,0x0B);

	RfSetChan(g_rfChan);

	A7105_strobe_cmd(0xF0);
	A7105_strobe_cmd(0xC0);

	//A7105_Write_Reg(0x0f,0x1e);
	//RfSetChan(mainPara.s_chan);
	//A7105_Write_Reg(0x0f,0x20);

	A7105_Read_Ram(0x06, idBuf,4);

#ifdef _A7105_DBG_
	if(!memcmp((u8*)g_a7105_id2,idBuf,4))
	{
		RF_LED_READY();// A7105 ready
		printf("a105 ok\r\n");
		//g_rfTtl = 1;
	}
#endif
	

}

void A7105Halt(void)
{
	A7105_Write_Reg(0x00, 0x00);
	A7105_strobe_cmd(0x80);
}

void RfHandleProc(void)
{

}

void RfSetId(u8 ch, u8* id)
{
	//u8 i = 0;
	/*u8 tmpBuf[4];
	tmpBuf[0] = ch*2 + id[0];
	tmpBuf[1] = ch*4 + id[1];
	tmpBuf[2] = ch*8 + id[2];
	tmpBuf[3] = ch*16 + id[3];*/
	
	A7105_Write_Ram(0x06,id,4);
}

void RfSetChan(u8 chan)
{
	A7105_Write_Reg(0x0f, chan*2+0x1E);
	//RfSetId(chan,(u8*)g_a7105_id2);
}

void ParseRfData(u8 *dat)
{
#if 0
	if (0x01 == dat[0]) // 频道
	{
		g_rfChan = dat[1];
		LedDspRfChan(g_rfChan);
		
	} else if (0x02 == dat[0])   // 色温
	{
		if (0 == dat[1])
		{
			g_pwm_chan1_duty = dat[2] * PWM_PULSE_WIDTH_STEP;
			CHAN_1_Set_Duty(PWN_PERIOD - g_pwm_chan1_duty);
			g_ledChar = (g_pwm_chan1_duty / PWM_PULSE_WIDTH_STEP);
			
		}else (1 == dat[1])
		{
			g_pwm_chan2_duty = dat[2] * PWM_PULSE_WIDTH_STEP;
			CHAN_2_Set_Duty(PWN_PERIOD - g_pwm_chan2_duty);
			g_ledChar = (g_pwm_chan2_duty / PWM_PULSE_WIDTH_STEP);
		}
	} else if (0x03 == dat[0])       //  on/off
	{
#endif
		if ( 0 == dat[1] )
		{
			g_pwm_chan1_duty = 0;
			g_pwm_chan2_duty = 0;
			CHAN_1_Set_Duty(PWN_PERIOD - g_pwm_chan1_duty);
			CHAN_2_Set_Duty(PWN_PERIOD - g_pwm_chan2_duty);
			g_ledChar = 0;
		} else if ( 1 == dat[1] )
		{
			g_pwm_chan1_duty = dat[2] * PWM_PULSE_WIDTH_STEP;
			g_pwm_chan2_duty = dat[3] * PWM_PULSE_WIDTH_STEP;
			CHAN_1_Set_Duty(PWN_PERIOD - g_pwm_chan1_duty);
			CHAN_2_Set_Duty(PWN_PERIOD - g_pwm_chan2_duty);
		}
	
#if 0			
	} 
#endif

}


void A7105_Slave_Proc()
{
	u8 rfBuf[12];      // 命令格式，[55][AA][LEN][D1][D2][D..][DN][CRC]
	if (GPIO_ReadInputPin(GPIOC,GPIO_PIN_3) == RESET)    // 数据
	{
		A7105_Read_Ram(0x05,rfBuf,12);
		if ((rfBuf[0] == 0x55) && (rfBuf[1] == 0xAA))
		{
			ParseRfData(rfBuf+3);
		}
	}
}

#define RF_PACKET_LEN 49
void RC_ControlData()
{
	u8 buf[RF_PACKET_LEN] = {0};
	u8 i = 0;

	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = RF_PACKET_LEN - 2;
	buf[3] = g_isLedON;       //
	buf[4] = g_pwm_chan1_duty;
	buf[5] = g_pwm_chan2_duty;
	buf[6] = 0;
	buf[7] = 0;
	for (i=2;i<7;i++)
	{
		buf[7] += buf[i]; 
	}
	buf[47] = 0x55;
	buf[48] = 0xAA;

	A7105_Write_Ram(0x05,buf,RF_PACKET_LEN);
	
	
}


