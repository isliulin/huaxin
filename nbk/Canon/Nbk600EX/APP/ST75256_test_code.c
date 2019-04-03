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
#define DISP           NORMAL	 //正显or反显  
#define VOP_COMPENSATE  -0.03   //VOP补偿	  
//=======================================================
#define DISP_ON           0XAE|ON
#define DISP_OFF          0XAE|OFF
#define EV_LSB		 EV&0X3F
#define EV_MSB		 ((EV>>1)&0XE0)>>5


//模块并行接口定义(接口按实际测架更改)
#define dataport  P1			   //并口通讯数据脚
//sbit CS    = P3^4;		          //定义片选脚为P3.0
//sbit RST   = P3^0;		          //定义复位脚为P3.1
//sbit RS    = P3^3;		          //定义数据/指令控制脚为P3.2
sbit RW_WR = P3^1;		          //定义读写控制(68)或写(80)脚为P3.3
sbit E_RD  = P3^2;		          //定义读写使能(68)或读(80)脚为P3.4
//---------------------------------------------------------


//模块串行接口定义(接口按实际测架更改)
sbit SDA = P1^4;  //SDA			  //定义串行数据脚为P1.4
sbit SCK = P1^3;				  //定义串行时钟脚为P1.3
sbit RS  = P1^1;  //A0			 //定义数据/指令控制脚为P1.2
sbit RST = P1^2;				 //定义复位脚为P1.1
sbit CS  = P1^0;			   //定义片选脚为P10
//---------------------------------------------------------	

 
//按键接口定义(与测试板一致)
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
//===================vop温补函数==================
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
// 		write_data( 0XF1  );	//显示字符的上半节8*8
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
// 写指令子程序
// 输入参数:comd
// 输出参数:无
// ====================================//
void write_com(unsigned char comd)
{
	unsigned char i;
//4SPI时序
	if(MODE==3)
	{
		CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制
		RS = 0;								//置低RS表示执行指令的传送
		SCK = 0;							//置低SCK为产生上升沿作准备
		for(i=0;i<8;i++)
		{//循环传送共8位从高位开始
			if( (comd&0x80)==0x80 ) SDA = 1;//判断comd的最高位,是"1"则置SDA=1
			else SDA = 0;					//comd最高位为"0",则置SDA=0
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			SCK = 1;						//置低SCK为产生上升沿作准备
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			SCK=0;							//置高SCK产生上升沿
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
	        comd = comd<<1;					//往高位移一位继续
	
		} 
		CS = 1;	
	}							//置高CS表示任何动作对芯片无效
#if 0
	else if(MODE==2)
	{
//8080时序
    CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制
	RS = 0;								//置低RS表示执行指令传输
	E_RD = 1;							//置高E_RD写动作准备
	dataport = comd;					//把comd传递过来的参数写入到LCD数据线上
	RW_WR = 0;							//置低RW_WR进行上升沿的产生作准备
	_nop_();_nop_();_nop_();_nop_();	//数据建立一定时间
	RW_WR = 1;							//置高RW_WR产生上升沿数据此时锁存进芯片内部电路
	_nop_();_nop_();_nop_();_nop_();	//数据保持一定时间
	CS = 1;								//置高CS表示任何动作对芯片无效
	}
	else if(MODE==1)
	{
 //	6800时序
	CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制,顺序1
	RS = 0;								//置低RS表示执行指令传输,顺序2
	RW_WR = 0;							//置低RW_WR表示写动作准备,顺序3
	E_RD = 1;							//置高E_RD进行下降沿的产生作准备,顺序4
	dataport = comd;					//把comd传递过来的参数写入到LCD数据线上,顺序5
	_nop_();_nop_();_nop_();_nop_();	//数据保持一定时间
	E_RD = 0;							//置低E_RD产生下降沿数据此时锁存进芯片内部电路,顺序6
	_nop_();_nop_();_nop_();_nop_();	//数据建立一定时间
	CS = 1;								//置高CS表示任何动作对芯片无效,顺序7
	}
#endif
}

// ====================================//
// 写数据子程序
// 输入参数:dat
// 输出参数:无
// ====================================//
void write_data(unsigned char dat)
{
	unsigned char i;
	if(MODE==3)		//4SPI时序
	{
		CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制
		RS = 1;								//置高RS表示执行数据的传送
		SCK = 0;							//置低SCK为产生上升沿作准备
		for(i=0; i<8; i++)
		{//循环传送共8位从高位开始
			if( (dat&0x80)==0x80 ) SDA = 1;//判断comd的最高位,是"1"则置SDA=1
			else SDA = 0;					//comd最高位为"0",则置SDA=0
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			SCK = 1;						//置低SCK为产生上升沿作准备
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			SCK=0;							//置高SCK产生上升沿
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
			_nop_();_nop_();_nop_();_nop_();	//数据稳定一定时间
	        dat = dat<<1;					//往高位移一位继续
		}
		CS = 1;								//置高CS表示任何动作对芯片无效
	}
#if 0
//8080时序
	else if(MODE==2)
	{
	CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制
	RS = 1;								//置高RS表示执行数据传输
	E_RD = 1;							//置高E_RD写动作准备
	dataport = dat;						//把dat传递过来的参数写入到LCD数据线上
	RW_WR = 0;							//置低RW_WR进行上升沿的产生作准备
	_nop_();_nop_();_nop_();_nop_();	//数据建立一定时间
	RW_WR = 1;							//置高RW_WR产生上升沿数据此时锁存进芯片内部电路
	_nop_();_nop_();_nop_();_nop_();	//数据保持一定时间
	CS = 1;								//置高CS表示任何动作对芯片无效
	}
	else if(MODE==1)
	{
//6800时序
	CS = 0;								//置低CS表示选中芯片可以有效的对芯片进行控制,顺序1
	RS = 1;								//置高RS表示执行数据传输
	RW_WR = 0;							//置低RW_WR表示写动作准备,顺序3
	E_RD = 1;							//置高E_RD进行下降沿的产生作准备,顺序4
	dataport = dat;						//把dat传递过来的参数写入到LCD数据线上,顺序5
	_nop_();_nop_();_nop_();_nop_();	//数据保持一定时间
	E_RD = 0;							//置低E_RD产生下降沿数据此时锁存进芯片内部电路,顺序6
	_nop_();_nop_();_nop_();_nop_();	//数据建立一定时间
	CS = 1;								//置高CS表示任何动作对芯片无效,顺序7
	}
#endif
}
//========================================================
// 显示隔行隔列间隔点棋盘的函数
// 输入参数:双字节
// 输出参数:无
// ====================================//
void disp_fram(unsigned char x,unsigned char y,unsigned  char a,unsigned char b)
{
	uint i,j;	//定义局部字符变量

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

void disp_char(unsigned int x,unsigned int y,unsigned char dat[])  //一个字符大小：16bit*8bit
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
		write_data( dat[j] );	//显示字符的上半节8*8
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
		write_data( dat[j]  );	//显示字符的下半节8*8
	}


}
// ====================================//
// 对比度设定及显示对比度参数函数
// 输入参数:ADJUST
// 输出参数:无
// 实现对比度的更新,及显示当前调整的数值
// ====================================//
void SetContrast(void)
{
	char i;
	write_com(0x30);												
	write_com(0x81);	//对比度调节寄存器	
	write_data(ADJUST&0X3F);  	// VPR[5-0]	  ((EV>>1)&0XE0)>>5
	write_data(((ADJUST>>1)&0Xe0)>>5);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000

	VOP=3.6+ADJUST*0.04+VOP_COMPENSATE;
	i=ADJUST/100;		//取出对比度数值的百位
	disp_char(LINE1,0,num[i]);

	i=ADJUST%100/10;		//取出对比度数值的十位
	disp_char(LINE1,1,num[i]);

	i=ADJUST%10;		//取出对比度数值的个位
	disp_char(LINE1,2,num[i]);
//====================================================== 显示公式计算的VOP值
	disp_char(LINE2,0,disp_vop[0]);
	disp_char(LINE2,1,disp_vop[1]);
	disp_char(LINE2,2,disp_vop[2]);
	disp_char(LINE2,3,disp_vop[3]);

	i=VOP/10;		  //显示VOP数值 十位
	disp_char(LINE2,4,num[i]);

	i=(int)(VOP)%10;	//个位
	disp_char(LINE2,5,num[i]);

	disp_char(LINE2,6,disp_vop[4]);//显示小数点

	i=(int)(VOP*10.0)%10; //显示小数位1
	disp_char(LINE2,7,num[i]);

	i=(int)(VOP*100.0)%10; //显示小数位2
	disp_char(LINE2,8,num[i]);

	disp_char(LINE2,9,disp_vop[5]);	//显示V
  
}  

// ====================================//
// 按键处理函数(带定时功能总体定时约1ms*Xms)
// 输入参数:Xms
// 输出参数:暂停键按下标志
// 功能说明: 
// 1, 在定时过程中首先判断是否有暂停键按下,如有则定时失效,此时同时判断对比度上下调节键及暂停键,
//    如果对比度调节键有按下则进行对比度调整,如果暂停键有按下则记录下暂停键有按下的状态并返回一个"1"参数并退出程序.
// 2, 在定时过程中暂停键一直没有动作,则直接返回"0"参数并退出程序.
// ====================================//
unsigned char Change(unsigned int Xms)
{
	unsigned int i;
	unsigned char j,h;
	j=h=0;
	for(i=0;i<Xms;i++)
	{//需定时的ms数
		for(j=0;j<150;j++)
		{//定时的基数约1ms
			if( SW==0 )
			{//如果暂停键按下有效
				delay_ms(10);	//消抖动
				if ( SW==0 )
				{//暂停键按下有效
					//disp_fram( 0x00,0x00,0,0 );
					while( SW==0 )
					SetContrast();	//等待暂停键弹起
					delay_ms(50);	//消抖动					
					while(1)
					{
						if( SW==0 )
						{//暂停键再次按下
							delay_ms(10);	//消抖动
							if ( SW==0 )
							{//暂停键按下有效
								while( SW==0 )	//等待暂停键弹起
								SetContrast();
								delay_ms(10);	//消抖动
								h = 1;	//把暂停键标志置1
								goto Eixt;	//跳到退出程序标号
							}
						}
						else if( UP==0 )
						{//对比度+键按下
							delay_ms(10);	//消抖动
							if ( UP==0 )
							{//对比度+键按下有效
								ADJUST++;	//对比度寄存器加一档
								while( UP==0 )	//等待此键弹起
								{
									if( DN==0 )
									{
										delay_ms(5);	//消抖动
										if ( DN==0 )
										{//对比度-键按下有效
											while( (DN==1)&&(UP==1) )
											SetContrast();	//等待此键弹起
											delay_ms(5);	//消抖动
											h = 1;	//把暂停键标志置1
											goto Eixt;	//跳到退出程序标号
										}
									} 
								SetContrast();
								}
						     }
								
								
						}
						else if( (DN==0)&&(UP==1) )
						{//对比度-键按下
							delay_ms(10);	//消抖动
							if ( DN==0 )
							{//对比度-键按下有效 
								if(ADJUST!=0)
								ADJUST--;	//对比度寄存器减一档
								
								while( DN==0 )
								SetContrast();	//等待此键弹起
											
								//======adjust contrast=====对比度设定
							}				
						}
					}
				}
			}
		}
	}
	Eixt: delay_ms(1);	//退出程序标号
	return (h);	//返回参数
}