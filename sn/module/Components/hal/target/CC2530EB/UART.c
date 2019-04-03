//-----------����ͨѶ ���յ��Է����ݲ����ص���---------//
/*                  Creat By ES                       */
/*           http://shop101811732.taobao.com          */
/******************************************************
ʹ�ô���0�� �������趨Ϊ115200
ʵ�����ݣ�
ģ��һ�ϵ�����Է��� Hello��Welcome to ES
�����յ����Ե����ݣ�abc#��ʽ���ͣ�#Ϊ�����������ѽ��յ���
���ݷ��ص���

ע�⣺ʹ�ô��ڵ��������뽫�������趨Ϊ115200����ѡ�����
���ںţ���Ҫ��ѡHEX��ʾ�����Ա�����������Ҫ�ڵ��԰�װ
PL2302 USBת������������ʹ��USB������ģ��͵���
******************************************************/

#include <ioCC2530.h>
#include <string.h>

#define uint unsigned int
#define uchar unsigned char

//�������LED�ƵĶ˿�

#define Blue_LED     P1_4    //P14������ɫLED
#define Yellow_LED1  P1_5    //P15���ӻ�ɫLED
#define Yellow_LED2  P1_6    //P16���ӻ�ɫLED

#define Bell P2_0	//BellΪP2.0�ڿ���

//��������
void Delayms(uint xms);		//��ʱ����
void Initial_IO(void);		//��ʼ���˿�
void InitUart(void);              //��ʼ������
void Uart_Send_String(char *Data,int len);

char Rxdata[50];
uchar RXTXflag = 1; 
char temp; 
uchar  datanumber = 0;

/****************************
          ��ʱ����
*****************************/
void Delayms(uint xms)   //i=xms ����ʱi���� (16M����ʱ���Լ����32M��Ҫ�޸ģ�ϵͳ���޸�Ĭ��ʹ���ڲ�16M)
{
 uint i,j;
 for(i=xms;i>0;i--)
   for(j=587;j>0;j--);
} 


/****************************
//��ʼ������
*****************************/
void Initial_IO(void)
{
        P1DIR |= 0xff; //P1����Ϊ���
        P2DIR |= 0x01;     //P2_0 Ϊ���
        P1=0x00;       //P1��Ϊ�ͣ���ֹ����LED
        Bell=0;    //��ֹ������  
}
/**************************************************************** 
   ���ڳ�ʼ������     
***********************************************************/
void InitUart(void)
{
    CLKCONCMD &= ~0x40; // ����ϵͳʱ��ԴΪ 32MHZ����
    while(CLKCONSTA & 0x40);                     // �ȴ������ȶ� 
    CLKCONCMD &= ~0x47;                          // ����ϵͳ��ʱ��Ƶ��Ϊ 32MHZ

    PERCFG = 0x00;        //λ��1 P0�� 
    P0SEL = 0x3c;        //P0_2,P0_3,P0_4,P0_5��������,�ڶ����� 
    P2DIR &= ~0XC0;      //P0 ������ΪUART0 �����ȼ�
 
    U0CSR |= 0x80;       //UART ��ʽ 
    U0GCR |= 11;         //U0GCR��U0BAUD���     
    U0BAUD |= 216;       // ��������Ϊ115200 
    UTX0IF = 0;          //UART0 TX �жϱ�־��ʼ��λ1  ���շ�ʱ��
    U0CSR |= 0X40;       //������� 
    IEN0 |= 0x84;        // �����жϣ������ж�    
}

/**************************************************************** 
���ڷ����ַ�������    
****************************************************************/ 
void Uart_Send_String(char *Data,int len) 
{
 { 
  int j; 
  for(j=0;j<len;j++) 
  { 
    U0DBUF = *Data++; 
    while(UTX0IF == 0); //������ɱ�־λ
    UTX0IF = 0; 
  } 
 }
}
/***************************
//������
***************************/

void main(void)
{
  Initial_IO();		//���ó�ʼ������ 
  InitUart();
  Uart_Send_String("Hello, Welcome to ES  ",22);
  while(1)
  {
     if(RXTXflag == 1)     //����״̬ 
     { 
        Blue_LED=1;       //����״ָ̬ʾ 
        if( temp != 0) 
        { 
           if((temp!='#')&&(datanumber<50)) //'��'������Ϊ�����ַ�������ܽ���50���ַ�           
           Rxdata[datanumber++] = temp; 
           else 
           { 
             RXTXflag = 3;                     //���뷢��״̬ 
             Blue_LED=0;   //��ָʾ��
            } 
            temp  = 0;
         }
      }
      if(RXTXflag == 3)     //����״̬ 
      { 
       Yellow_LED1= 1;                           
       U0CSR &= ~0x40;      //��ֹ���� 
       Uart_Send_String(Rxdata,datanumber); //�����Ѽ�¼���ַ�����
       U0CSR |= 0x40;      //������� 
       RXTXflag = 1;       // �ָ�������״̬ 
       datanumber = 0;     //ָ���0 
       Yellow_LED1 = 0;       //�ط���ָʾ 
      }    
    }
}


/**************************************************************** 
���ڽ���һ���ַ�: һ�������ݴӴ��ڴ���CC2530, ������жϣ������յ������ݸ�ֵ������temp. 
****************************************************************/ 

#pragma vector = URX0_VECTOR 
  __interrupt void UART0_ISR(void) 
 { 
  URX0IF = 0;    // ���жϱ�־ 
  temp = U0DBUF;                           
 }
