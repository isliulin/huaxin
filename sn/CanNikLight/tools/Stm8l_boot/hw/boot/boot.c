#include "..\uart\uart.h"
#include "iostm8l152k4.h"
#include "type.h"
#include "boot.h"
#include "..\..\aes\aes.h"

typedef struct U_STCOMMANDER
{
	u8 head;
	u8 cmd;
	u32 addr;		
	u16 data_len;
 	u8 buf[RCV_CMD_LEN];
	u8 sum;
}STCOMMANDER;

STCOMMANDER ucLDR_Message = {0};

u8 start_flag=0;

extern void delay_ms(unsigned int time);

static unsigned char uartRcvData(void)
{
	unsigned int count1=0;
	unsigned int count2=0;

	while(!(USART1_SR&(1<<5)))
	{
     		count1++;
     		if(count1>=5000)
     		{
			count1=0;
			count2++;
		}
		if((count2>=5000)&&(start_flag==1))
		{
			start_flag=0;
			count2 = 0;
			return 0;
		}
	}

	return (USART1_DR);
}

static u16 Message_Rcv(u8 *errCod)
{
	u8 ucdata;
	u8 rcvdat;
	u16 uci;
	
	start_flag=1;
	*errCod = 0;
	
        // 0x7E
	ucdata=uartRcvData();
	if(ucdata!=0x7E)
	{
		
		*errCod = ucdata;		
		return 1;
	}

	start_flag=1;
	
        // 0x41
      ucdata=uartRcvData();
	if(start_flag==0)
	{
		return 2;
	}
	ucLDR_Message.cmd =  ucdata;
	ucLDR_Message.sum =ucdata;	

	//printf("sum, %x\r\n",ucLDR_Message.sum);
	
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 3;
	}
	ucLDR_Message.addr =  (((u32)ucdata)<<16);
	ucLDR_Message.sum+=ucdata;	
	
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 3;
	}
	ucLDR_Message.addr |=  ((u32)ucdata<<8);
	ucLDR_Message.sum+=ucdata;	

	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 3;
	}
	ucLDR_Message.addr |=  ((u32)ucdata);
	ucLDR_Message.sum+=ucdata;	

	//printf("sum, %x\r\n",ucLDR_Message.sum);
	
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 4;
	}
	ucLDR_Message.data_len=  ucdata*256;
	ucLDR_Message.sum+=ucdata;

	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 4;
	}
	ucLDR_Message.data_len +=  ucdata;
	ucLDR_Message.sum+=ucdata;
	

	//printf("sum, %x\r\n",ucLDR_Message.sum);

	//printf("len %d\r\n",ucLDR_Message.data_len);
	
	for( uci=0 ; uci<ucLDR_Message.data_len ; uci++ )
	{
		ucdata = uartRcvData();
		if(start_flag==0)
		{
			//printf("data no rcv\r\n");
			 return (4+uci);
		}
		ucLDR_Message.buf[uci] = ucdata;
		ucLDR_Message.sum+=ucdata;

		//printf("data %x, sum %x\r\n",ucdata,ucLDR_Message.sum);
	}
	ucdata = uartRcvData();
	rcvdat = ucLDR_Message.sum;

	//printf("sum, %x\r\n",rcvdat);
	
	
	if(start_flag==0)
	{
		//printf("sum no rcv\r\n");
		 return (4+ucLDR_Message.data_len);
	}
	//if(ucLDR_Message.msg_cks==ucdata)
	if(rcvdat==ucdata)
	{
      		return 0;
	}
	else
	{
		//*errCod = ucdata;
		//printf("sum error, %x %x\r\n",rcvdat,ucdata);
		*errCod = rcvdat;
		return (4+ucLDR_Message.data_len+1);	
	}

}

unsigned char CalcSum(unsigned char *buf, unsigned char len)
{
	unsigned char sum=0, i;
	for(i=0;i<len;i++)
	{
		sum+=buf[i];
	}
	return sum;
}

static void SendData(char dat)
{
	putchar(dat);
}

static void Message_Send(void)
{
	unsigned char  uci;

	SendData(0x7E);

	//ucLDR_Message.sum=ucLDR_Message.head;
	//SendData(ucLDR_Message.head);
	ucLDR_Message.sum = 0;

	ucLDR_Message.sum+=ucLDR_Message.cmd;
	SendData(ucLDR_Message.cmd); 	

	ucLDR_Message.sum+=((ucLDR_Message.addr>>16)&0xff);
	SendData((ucLDR_Message.addr>>16)&0xff); 

	ucLDR_Message.sum+=((ucLDR_Message.addr>>8)&0xff);
	SendData((ucLDR_Message.addr>>8)&0xff); 

	ucLDR_Message.sum+=(ucLDR_Message.addr&0xff);
	SendData(ucLDR_Message.addr&0xff); 

	ucLDR_Message.sum+=((ucLDR_Message.data_len>>8)&0xff);
	SendData((ucLDR_Message.data_len>>8)&0xff); 

	ucLDR_Message.sum+=(ucLDR_Message.data_len&0xff);
	SendData(ucLDR_Message.data_len&0xff); 

	for(uci =0;uci<ucLDR_Message.data_len;uci++)
	{
		ucLDR_Message.sum+=ucLDR_Message.buf[uci];  
		SendData(ucLDR_Message.buf[uci]); 	
	}
	ucLDR_Message.sum+=0;
	SendData(ucLDR_Message.sum); 		
}

static void STMFLASH_Write_NoCheck(u32 WriteAddr,u8* pBuffer,u16 NumToWrite)
//static void STMFLASH_Write_NoCheck(u32 WriteAddr,u16* pBuffer,u16 NumToWrite)
{
	u16 i;
        //u32 pFlash;
	//@far INT16U * pflash  = (@far INT16U *)(0x9000+WriteAddr);
	for(i=0;i<NumToWrite;i++)
	{
               // pFlash = 65536l + WriteAddr;
                //*(unsigned char*)pFlash = pBuffer[i];
		*(unsigned char*)(APP_START_ADDR + WriteAddr) = pBuffer[i];
                //*(u16*)(APP_START_ADDR + WriteAddr) = pBuffer[i];
                //*(u16 *)(APP_START_ADDR + WriteAddr) = pBuffer[i];
		//pflash[i] = pBuffer[i];
		WriteAddr++;//µØÖ·Ôö¼Ó2.
	}  
	delay_ms(10);
}

//u8 *g_decKey = "MakeTomorrowBest";
u8 *g_decKey = "KeyForSndebug@17";
//u8 g_src[16] = {0xF0,0x02,0x2D,0x76,0xAA,0x9C,0xC7,0xD7,0x2A,0xB3,0x24,0x53,0x98,0xB9,0x62,0x93};

void BootProc( void )
{
  u16 ret = 0;
  u8 err;
  
  while (1)
  {
      ret = Message_Rcv(&err);
      if(!ret)
      {
          switch(ucLDR_Message.cmd)
          {
            case AM1_IAP_WRITE_PAGE:
                if(ucLDR_Message.addr < FLASH_AVAILABLE)
                {
                  Boot_Decrypt(ucLDR_Message.buf,g_decKey,ucLDR_Message.buf,RCV_CMD_LEN);
                  //if (0x6000 == ucLDR_Message.addr)
                  //{
                  //  err = 1;
                  //}
                  STMFLASH_Write_NoCheck(ucLDR_Message.addr,ucLDR_Message.buf,RCV_CMD_LEN);
                  //flash_write_block(ucLDR_Message.buf,ucLDR_Message.addr,RCV_CMD_LEN);
                  //ucLDR_Message.addr += 16;
                  ucLDR_Message.addr += RCV_CMD_LEN;
                  //delay_ms(1);
                  ucLDR_Message.buf[0]=0x00;
                  ucLDR_Message.data_len=2;
                  Message_Send();	
                }else
                {
                  ucLDR_Message.buf[0]=1;
                  ucLDR_Message.data_len=2;
                  Message_Send();
                }
            break;
          }
      }else
      {
          putchar(ret);
          putchar(err);
      }
  }
}

