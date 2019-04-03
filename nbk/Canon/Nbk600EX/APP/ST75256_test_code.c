/********************************************************;
;*		 IC		: 		ST75256							*;
;*		 Output: 256-segment/162-common+1-icon-common   *;
;*       MCU type       : 	STC89LE85	              	*;
;*       Data		: 2014.07.25      8080,6800,4SPI    *;
;*       	===========lina==========			 		*;
;********************************************************/
#include<reg51.h>
#include<intrins.h>
#include"ASCH.H"
#define uint unsigned int
#define uchar unsigned char

//======================================
#define OFF 0
#define ON 1 
#define HIGH   1
#define LOW    0
#define REVERSE 1
#define NORMAL  0
#define P6800  1
#define P8080  2
#define SPI4   3 
//#define DATA_IO P1
//==========================
#define MODE 	    SPI4
#define UP_START    0  	 //0-7 page	  
#define UP_END      6	  
#define DOWN_START  7	 //0-7 page
#define DOWN_END    12
#define WIDTH       172
#define SEG_START	84 //
#define LINE1		0	//adjust vop screen	(EV)
#define LINE2		2	//adjust vop screen	(VOP=**.**V)
#define EV        0x55//360//323  //VOP=3.6+EV*0.04 (5-335) 
//#define EV_LSB	 	0x20 		//0x00-0x3f D5-D0
//#define EV_MSB	    0x02		//0x00-0x07 D2-D0
#define BOOSTER_EFFICIENCY 1  //0: 3KHz   1: 6KHz
							  //2: 12KHz  3: 24KHz 
#define BIAS 	    2	  //    BIAS        BIAS  
                          //0:  1/14	3:  1/11
						  //1:  1/13    4:  1/10
						  //2:  1/12    5:  1/9	
#define CLD 	0	      //0: NOT DIVIDE 1:  1:2 DIVISION
#define DUTY 	103	  //real=DUTY+1						 					     
#define BOOSTER  	  1   // 0:X8  1:X10
#define TA		  0 //temp=TA-40	 10
#define TB		  20 //temp=TB-40	 20
#define TC		  40 //temp=TC-40	 30
#define FREQUENCYA     0X1f	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 -30 TO TA
#define FREQUENCYB     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TA TO TB
#define FREQUENCYC     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TB TO TC
#define FREQUENCYD     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TC TO 90
#define VB  ON		 //internal booster
#define VR  ON		 //internal regulator
#define VF  ON		 //internal follower
#define HPM  HIGH //HIGH:HIGH POWEWR MODE  LOW:NORMAL MODE
#define CL   0 		  //0:in the column direction 1:in the page direction  *9.2.9*
#define MX  REVERSE   // SEG DIRECTION: NORMAL:0->131  REVERSE: 131->0
#define MY  NORMAL	  // COM_DIRECTIONN: ORMAL:0->63   REVERSE: 63->0
#define DISP_MODE  	 0 	//0: MONO   1: 4GRAY SCALE MODE
#define ALL_PIXEL  OFF//ALL PIXEL ON/OFF
#define DISP           NORMAL	 //����or����  
#define VOP_COMPENSATE  -0.03   //VOP����	  
//=======================================================
#define DISP_ON           0XAE|ON
#define DISP_OFF          0XAE|OFF
#define EV_LSB		 EV&0X3F
#define EV_MSB		 ((EV>>1)&0XE0)>>5


//ģ�鲢�нӿڶ���(�ӿڰ�ʵ�ʲ�ܸ���)
#define dataport  P1			   //����ͨѶ���ݽ�
//sbit CS    = P3^4;		          //����Ƭѡ��ΪP3.0
//sbit RST   = P3^0;		          //���帴λ��ΪP3.1
//sbit RS    = P3^3;		          //��������/ָ����ƽ�ΪP3.2
sbit RW_WR = P3^1;		          //�����д����(68)��д(80)��ΪP3.3
sbit E_RD  = P3^2;		          //�����дʹ��(68)���(80)��ΪP3.4
//---------------------------------------------------------


//ģ�鴮�нӿڶ���(�ӿڰ�ʵ�ʲ�ܸ���)
sbit SDA = P1^4;  //SDA			  //���崮�����ݽ�ΪP1.4
sbit SCK = P1^3;				  //���崮��ʱ�ӽ�ΪP1.3
sbit RS  = P1^1;  //A0			 //��������/ָ����ƽ�ΪP1.2
sbit RST = P1^2;				 //���帴λ��ΪP1.1
sbit CS  = P1^0;			   //����Ƭѡ��ΪP10
//---------------------------------------------------------	

 
//�����ӿڶ���(����԰�һ��)
sbit SW	=P2^5;
sbit DN	=P2^6;
sbit UP	=P2^7;
sbit key=P2^4;
//========================================================
void init();
void clearddram();	  
void showpic(uchar *p);
void disp_fram(unsigned char x,unsigned char y,unsigned  char a,unsigned char b);
void write_com(uchar para);
void write_data(uchar para);
void delay_ms(uint t);
void SetContrast(void);
unsigned char Change(unsigned int );
//================================================
uint ADJUST=EV;
float VOP=0;

void delay_ms(uint x)
{
	uint i;
	for(;x>0;x--)
	for(i=120;i>0;i--);
}
//===================vop�²�����==================
void  temp_compensation()
{
	write_com(0x31);   	// Extension Command 2
	write_com(0xf4);//set temperature gradient compensation coefficient
	write_data(0X00);//MT1,MT0
	write_data(0X00);//MT3,MT2
	write_data(0X00);//MT5,MT4
	write_data(0XFF);//MT7,MT6
	write_data(0X00);//MT9,MT8
	write_data(0X00);//MT11,MT10
	write_data(0X00);//MT13,MT12
	write_data(0X00);//MT15,MT14
}
//================================================
void init()
{   
    	RST=1;
		delay_ms(2);
    	RST=0;
    	delay_ms(2);
    	RST=1;
    	delay_ms(200);
    		
	write_com(0x30);   	// Extension Command 1

	write_com(0x94);   	// Sleep out
	write_com(DISP_OFF);    	//Display off
	delay_ms(50);
		
	write_com(0x20);   	// Power control
	write_data((VB<<3)|(VF<<1)|VR);  // D0=regulator ; D1=follower ; D2=booster,  on:1 off:0

	write_com(0x81);   	// EV control	
	write_data(EV_LSB);  	// VPR[5-0]
	write_data(EV_MSB);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000

	
	write_com(0x31);   	// Extension Command 2
	write_com(0x32);   	// analog circuit set
	write_data(0x00); 	//
	write_data(BOOSTER_EFFICIENCY);   // Frequency on booster capacitors 6khz
	write_data(BIAS);	//1/9 Bias	0X05
	
	write_com(0x51);   	// Booster Level x10
	write_data(0XFA|BOOSTER);
	
	write_com(0x30);   	// Extension Command 1
//	write_com(0xee);   	//disable read modify write   *****

	write_com(0xF0);   	// Gray or mono
	write_data(0X10|DISP_MODE);    //

//	write_com(0x76);  // disable icon

	write_com(0xCA);   	// Display Control
	write_data(CLD<<2);   // 1/2 division  0X04
	write_data(DUTY);	// 1/64duty	 
	write_data(0x00);      	//
	
	write_com(0xBC);   	// data scan direction -----------
	write_data((CL<<2)|(MX<<1)|MY);   	// Data scam direction
	//write_data(0x01); 

	write_com(0XA6|DISP);   	// Normal display
	
	write_com(0x31);   	// Extension Command 2
	write_com(0x40);   	// Internal Power Supply 
	write_com(0x48|HPM);//high power mode set 

	write_com(0xf2);	// set frequency of frame rate
	write_data(TA);	 //0
	write_data(TB);	 //30
	write_data(TC);	 //60
	 
	write_com(0xf0);	// set frequency of frame rate
	write_data(FREQUENCYA);	 //the Frame Rate in temp. -30~TA
	write_data(FREQUENCYB);	 //the Frame Rate in temp. TA~TB
	write_data(FREQUENCYC);	 //the Frame Rate in temp. TB~TC
	write_data(FREQUENCYD);	 //the Frame Rate in temp. TC~90
//	temp_compensation();

	write_com(0x30);
	if(ALL_PIXEL)
	write_com(0X23); //all on	
	write_com(DISP_ON);    	//Display on
	

}
//========================================================
main()
{
//	uchar i,j;
	init();
//    delay_ms(5);
// 	disp_fram(0x00,0x00,0,0);
// 	for(i=0;i<13;i++)
// 	{
// 	write_com(0x30);
// 	write_com(0x75); //set page address
// 	write_data(i);
// 	write_data(i);			     
// 	write_com(0x15); //set column address
// 	write_data(SEG_START);
// 	write_data(SEG_START+172); 
// 	write_com(0x5c);
// 	for(j=0;j<172;j++)
// 	{
// 		write_data( 0XF1  );	//��ʾ�ַ����ϰ��8*8
// 		delay_ms(10);
// 	}
// 	}

	while(1)
{

	disp_fram(0xff,0xff,0xff,0xff);
	if(Change(200)) ;
	delay_ms(700);

	disp_fram(0x00,0x00,0,0);
if(Change(200)) ;
	delay_ms(700);

	disp_fram(0xcc,0xcc,0x33,0x33);
if(Change(200)) ;
	delay_ms(700);

 	disp_fram(0xaa,0x55,0xaa,0x55);
if(Change(200)) ;
	delay_ms(700);
	
	disp_fram(0xaa,0xaa,0xaa,0xaa);
if(Change(200)) ;
	delay_ms(700);

	disp_fram(0xFF,0x00,0xFF,0x00);
if(Change(200)) ;
	delay_ms(700);
// 	
	showpic(pic176104);
if(Change(200)) ;
	delay_ms(1000);

	showpic(pic176104_1);
if(Change(200)) ;
	delay_ms(1000);
//	

//



		
}
}

// ====================================//
// дָ���ӳ���
// �������:comd
// �������:��
// ====================================//
void write_com(unsigned char comd)
{
	unsigned char i;
//4SPIʱ��
	if(MODE==3)
	{
		CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���
		RS = 0;								//�õ�RS��ʾִ��ָ��Ĵ���
		SCK = 0;							//�õ�SCKΪ������������׼��
		for(i=0;i<8;i++)
		{//ѭ�����͹�8λ�Ӹ�λ��ʼ
			if( (comd&0x80)==0x80 ) SDA = 1;//�ж�comd�����λ,��"1"����SDA=1
			else SDA = 0;					//comd���λΪ"0",����SDA=0
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			SCK = 1;						//�õ�SCKΪ������������׼��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			SCK=0;							//�ø�SCK����������
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
	        comd = comd<<1;					//����λ��һλ����
	
		} 
		CS = 1;	
	}							//�ø�CS��ʾ�κζ�����оƬ��Ч
#if 0
	else if(MODE==2)
	{
//8080ʱ��
    CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���
	RS = 0;								//�õ�RS��ʾִ��ָ���
	E_RD = 1;							//�ø�E_RDд����׼��
	dataport = comd;					//��comd���ݹ����Ĳ���д�뵽LCD��������
	RW_WR = 0;							//�õ�RW_WR���������صĲ�����׼��
	_nop_();_nop_();_nop_();_nop_();	//���ݽ���һ��ʱ��
	RW_WR = 1;							//�ø�RW_WR�������������ݴ�ʱ�����оƬ�ڲ���·
	_nop_();_nop_();_nop_();_nop_();	//���ݱ���һ��ʱ��
	CS = 1;								//�ø�CS��ʾ�κζ�����оƬ��Ч
	}
	else if(MODE==1)
	{
 //	6800ʱ��
	CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���,˳��1
	RS = 0;								//�õ�RS��ʾִ��ָ���,˳��2
	RW_WR = 0;							//�õ�RW_WR��ʾд����׼��,˳��3
	E_RD = 1;							//�ø�E_RD�����½��صĲ�����׼��,˳��4
	dataport = comd;					//��comd���ݹ����Ĳ���д�뵽LCD��������,˳��5
	_nop_();_nop_();_nop_();_nop_();	//���ݱ���һ��ʱ��
	E_RD = 0;							//�õ�E_RD�����½������ݴ�ʱ�����оƬ�ڲ���·,˳��6
	_nop_();_nop_();_nop_();_nop_();	//���ݽ���һ��ʱ��
	CS = 1;								//�ø�CS��ʾ�κζ�����оƬ��Ч,˳��7
	}
#endif
}

// ====================================//
// д�����ӳ���
// �������:dat
// �������:��
// ====================================//
void write_data(unsigned char dat)
{
	unsigned char i;
	if(MODE==3)		//4SPIʱ��
	{
		CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���
		RS = 1;								//�ø�RS��ʾִ�����ݵĴ���
		SCK = 0;							//�õ�SCKΪ������������׼��
		for(i=0; i<8; i++)
		{//ѭ�����͹�8λ�Ӹ�λ��ʼ
			if( (dat&0x80)==0x80 ) SDA = 1;//�ж�comd�����λ,��"1"����SDA=1
			else SDA = 0;					//comd���λΪ"0",����SDA=0
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			SCK = 1;						//�õ�SCKΪ������������׼��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			SCK=0;							//�ø�SCK����������
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
			_nop_();_nop_();_nop_();_nop_();	//�����ȶ�һ��ʱ��
	        dat = dat<<1;					//����λ��һλ����
		}
		CS = 1;								//�ø�CS��ʾ�κζ�����оƬ��Ч
	}
#if 0
//8080ʱ��
	else if(MODE==2)
	{
	CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���
	RS = 1;								//�ø�RS��ʾִ�����ݴ���
	E_RD = 1;							//�ø�E_RDд����׼��
	dataport = dat;						//��dat���ݹ����Ĳ���д�뵽LCD��������
	RW_WR = 0;							//�õ�RW_WR���������صĲ�����׼��
	_nop_();_nop_();_nop_();_nop_();	//���ݽ���һ��ʱ��
	RW_WR = 1;							//�ø�RW_WR�������������ݴ�ʱ�����оƬ�ڲ���·
	_nop_();_nop_();_nop_();_nop_();	//���ݱ���һ��ʱ��
	CS = 1;								//�ø�CS��ʾ�κζ�����оƬ��Ч
	}
	else if(MODE==1)
	{
//6800ʱ��
	CS = 0;								//�õ�CS��ʾѡ��оƬ������Ч�Ķ�оƬ���п���,˳��1
	RS = 1;								//�ø�RS��ʾִ�����ݴ���
	RW_WR = 0;							//�õ�RW_WR��ʾд����׼��,˳��3
	E_RD = 1;							//�ø�E_RD�����½��صĲ�����׼��,˳��4
	dataport = dat;						//��dat���ݹ����Ĳ���д�뵽LCD��������,˳��5
	_nop_();_nop_();_nop_();_nop_();	//���ݱ���һ��ʱ��
	E_RD = 0;							//�õ�E_RD�����½������ݴ�ʱ�����оƬ�ڲ���·,˳��6
	_nop_();_nop_();_nop_();_nop_();	//���ݽ���һ��ʱ��
	CS = 1;								//�ø�CS��ʾ�κζ�����оƬ��Ч,˳��7
	}
#endif
}
//========================================================
// ��ʾ���и��м�������̵ĺ���
// �������:˫�ֽ�
// �������:��
// ====================================//
void disp_fram(unsigned char x,unsigned char y,unsigned  char a,unsigned char b)
{
	uint i,j;	//����ֲ��ַ�����

	i=UP_START;
	if(UP_START>=UP_END)
	{
		while(i>=UP_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH/4;j++)
	    	{
	      		write_data(x);
				write_data(y);
	      		write_data(a);
				write_data(b);
	    	}
			i--;
		}
	}
	else
	{
		while(i<=UP_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH/4;j++)
	    	{
	      		write_data(x);
				write_data(y);
	      		write_data(a);
				write_data(b);
	    	}
			i++;
		}
	}
//=====================
	i=DOWN_START;
	if(DOWN_START>=DOWN_END)
	{
		while(i>=DOWN_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH/4;j++)
	    	{
	      		write_data(x);
				write_data(y);
	      		write_data(a);
				write_data(b);
	    	}
			i--;
		}
	}
	else
	{
		while(i<=DOWN_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH/4;j++)
	    	{
	      		write_data(x);
				write_data(y);
	      		write_data(a);
				write_data(b);
	    	}
			i++;
		}
	}
}
//========================================================
void showpic(uchar *p)
{
	int i,j;
	i=UP_START;
	if(UP_START>=UP_END)
	{
		while(i>=UP_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH;j++)	
			{
				write_data(*p++);
			}
			i--;
		}
	}
	else
	{
		while(i<=UP_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH;j++)	
			{
				write_data(*p++);
			}
			i++;
		}
	}
//=====================
	i=DOWN_START;
	if(DOWN_START>=DOWN_END)
	{
		while(i>=DOWN_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH;j++)	
			{
				write_data(*p++);
			}
			i--;
		}
	}
	else
	{
		while(i<=DOWN_END)
		{
			write_com(0x30);
		   	write_com(0x75); //set page address
			write_data(i);
			write_data(i);			     
			write_com(0x15); //set column address
			write_data(SEG_START);
			write_data(SEG_START+WIDTH); 
			write_com(0x5c);
			for(j=0;j<WIDTH;j++)	
			{
				write_data(*p++);
			}
			i++;
		}
	}

}

void disp_char(unsigned int x,unsigned int y,unsigned char dat[])  //һ���ַ���С��16bit*8bit
{
	unsigned char j;

	write_com(0x30);
	write_com(0x75); //set page address
	write_data(x);
	write_data(x);			     
	write_com(0x15); //set column address
	write_data(SEG_START+y*8);
	write_data(SEG_START+y*8+8); 
	write_com(0x5c);
	for(j=0;j<8;j++)
	{
		write_data( dat[j] );	//��ʾ�ַ����ϰ��8*8
	}

	write_com(0x30);
	write_com(0x75); //set page address
	if(LINE1<LINE2)
	{
	write_data(x+1);
	write_data(x+1);
	}
	else
	{
	write_data(x-1);
	write_data(x-1);
	}			     
	write_com(0x15); //set column address
	write_data(SEG_START+y*8);
	write_data(SEG_START+y*8+8);  
	write_com(0x5c);
	for(j=8;j<16;j++)
	{
		write_data( dat[j]  );	//��ʾ�ַ����°��8*8
	}


}
// ====================================//
// �Աȶ��趨����ʾ�ԱȶȲ�������
// �������:ADJUST
// �������:��
// ʵ�ֶԱȶȵĸ���,����ʾ��ǰ��������ֵ
// ====================================//
void SetContrast(void)
{
	char i;
	write_com(0x30);												
	write_com(0x81);	//�Աȶȵ��ڼĴ���	
	write_data(ADJUST&0X3F);  	// VPR[5-0]	  ((EV>>1)&0XE0)>>5
	write_data(((ADJUST>>1)&0Xe0)>>5);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000

	VOP=3.6+ADJUST*0.04+VOP_COMPENSATE;
	i=ADJUST/100;		//ȡ���Աȶ���ֵ�İ�λ
	disp_char(LINE1,0,num[i]);

	i=ADJUST%100/10;		//ȡ���Աȶ���ֵ��ʮλ
	disp_char(LINE1,1,num[i]);

	i=ADJUST%10;		//ȡ���Աȶ���ֵ�ĸ�λ
	disp_char(LINE1,2,num[i]);
//====================================================== ��ʾ��ʽ�����VOPֵ
	disp_char(LINE2,0,disp_vop[0]);
	disp_char(LINE2,1,disp_vop[1]);
	disp_char(LINE2,2,disp_vop[2]);
	disp_char(LINE2,3,disp_vop[3]);

	i=VOP/10;		  //��ʾVOP��ֵ ʮλ
	disp_char(LINE2,4,num[i]);

	i=(int)(VOP)%10;	//��λ
	disp_char(LINE2,5,num[i]);

	disp_char(LINE2,6,disp_vop[4]);//��ʾС����

	i=(int)(VOP*10.0)%10; //��ʾС��λ1
	disp_char(LINE2,7,num[i]);

	i=(int)(VOP*100.0)%10; //��ʾС��λ2
	disp_char(LINE2,8,num[i]);

	disp_char(LINE2,9,disp_vop[5]);	//��ʾV
  
}  

// ====================================//
// ����������(����ʱ�������嶨ʱԼ1ms*Xms)
// �������:Xms
// �������:��ͣ�����±�־
// ����˵��: 
// 1, �ڶ�ʱ�����������ж��Ƿ�����ͣ������,������ʱʧЧ,��ʱͬʱ�ж϶Աȶ����µ��ڼ�����ͣ��,
//    ����Աȶȵ��ڼ��а�������жԱȶȵ���,�����ͣ���а������¼����ͣ���а��µ�״̬������һ��"1"�������˳�����.
// 2, �ڶ�ʱ��������ͣ��һֱû�ж���,��ֱ�ӷ���"0"�������˳�����.
// ====================================//
unsigned char Change(unsigned int Xms)
{
	unsigned int i;
	unsigned char j,h;
	j=h=0;
	for(i=0;i<Xms;i++)
	{//�趨ʱ��ms��
		for(j=0;j<150;j++)
		{//��ʱ�Ļ���Լ1ms
			if( SW==0 )
			{//�����ͣ��������Ч
				delay_ms(10);	//������
				if ( SW==0 )
				{//��ͣ��������Ч
					//disp_fram( 0x00,0x00,0,0 );
					while( SW==0 )
					SetContrast();	//�ȴ���ͣ������
					delay_ms(50);	//������					
					while(1)
					{
						if( SW==0 )
						{//��ͣ���ٴΰ���
							delay_ms(10);	//������
							if ( SW==0 )
							{//��ͣ��������Ч
								while( SW==0 )	//�ȴ���ͣ������
								SetContrast();
								delay_ms(10);	//������
								h = 1;	//����ͣ����־��1
								goto Eixt;	//�����˳�������
							}
						}
						else if( UP==0 )
						{//�Աȶ�+������
							delay_ms(10);	//������
							if ( UP==0 )
							{//�Աȶ�+��������Ч
								ADJUST++;	//�ԱȶȼĴ�����һ��
								while( UP==0 )	//�ȴ��˼�����
								{
									if( DN==0 )
									{
										delay_ms(5);	//������
										if ( DN==0 )
										{//�Աȶ�-��������Ч
											while( (DN==1)&&(UP==1) )
											SetContrast();	//�ȴ��˼�����
											delay_ms(5);	//������
											h = 1;	//����ͣ����־��1
											goto Eixt;	//�����˳�������
										}
									} 
								SetContrast();
								}
						     }
								
								
						}
						else if( (DN==0)&&(UP==1) )
						{//�Աȶ�-������
							delay_ms(10);	//������
							if ( DN==0 )
							{//�Աȶ�-��������Ч 
								if(ADJUST!=0)
								ADJUST--;	//�ԱȶȼĴ�����һ��
								
								while( DN==0 )
								SetContrast();	//�ȴ��˼�����
											
								//======adjust contrast=====�Աȶ��趨
							}				
						}
					}
				}
			}
		}
	}
	Eixt: delay_ms(1);	//�˳�������
	return (h);	//���ز���
}