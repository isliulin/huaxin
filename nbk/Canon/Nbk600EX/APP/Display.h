#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//#include "uart.h"
//#define char signed char
#include "menu.h"
#define MODE_COMMON_ARR	3
#define MODE_GR_ARR		5
#define MASTER				0

#define RF_GROUP_TOTAL 5

typedef struct WL_ARR_PARA
{
#ifdef _NIKON_
	signed char mode;
#endif
	signed char output1;
	signed char output2;
	signed char cmp1;
	signed char cmp2;
#ifdef _CANON_
	signed char multi_exp;
#endif
	
}WL_ArrPara;


typedef struct RADIO_STATUE_PARA
{
	unsigned char cmdReady;
	unsigned char cmdSta;
	unsigned char devLinked;
	unsigned char devCnt[5];
	unsigned char devStatue[5];
	unsigned char adept;
	unsigned char rssi[16];
	unsigned char cmpFlag;
	unsigned char delay_mod;		// different  in 5dmarkIII 
}RadioStatue;

extern RadioStatue radioStatue;

extern WL_ArrPara *mstPara;

typedef struct GROUP_ARRPARA
{
	signed char mode;
	signed char isOff;
	signed char m_exp1;
	signed char m_exp2;
	signed char ttl_cmp1;
	signed char ttl_cmp2;
}RFGr_ArrPara;

typedef struct RADIO_SETTING
{
	signed char rfChan;
	unsigned int id;
	//unsigned char rfMod;
	//unsigned char* extAddr;
	RFGr_ArrPara rfGr[RF_GROUP_TOTAL];
}RadioSet;

typedef struct SN_MAC
{
	unsigned char sn[8];
	unsigned char mac[8];
}SnMac;

/*
typedef struct RADIO_PARA
{
	unsigned char optCode[RF_GROUP_TOTAL];
}RadioPara;
*/

typedef struct MOTO_LOCA
{
	unsigned char mode;
	u16 moto_location;
}MotoLoca;


typedef struct MAIN_PARA_SET
{
	signed char workMode;
	signed char  cmdMode;
	//signed char m_mstOn;
	signed char m_chan;
	signed char ttl_Ratio;
	signed char arr_mode;
	unsigned char m_isMstFla;
	WL_ArrPara modGr[4];	

	unsigned char s_chan;
	unsigned char s_array;

	int extIso;
	int extApert;
	
	MotoLoca motoLoca;
	
	signed char wlSlave_chan;
	signed char wlSlave_arr;
	RadioSet rfSet;
	//unsigned char multi_exp;
	int multi_Frq;
	signed int multi_times;
	signed char synMode;
	unsigned int counter;	
	signed char snrMenuSet[MENU_ITEMS];

#ifdef FIBER_ADJUST_PROC
	u8 isFiberAdjust;
	s8 fiberCoe;
	u8 isRgbChk;
	//u16 rgbRcd[2];
	u16 rgb1Chan[3];
	u16 rgb2Chan[3];
	u16 rgb3Chan[3];
	u16 rgb4Chan[3];
#endif

	char sumVeri;

	char rsv;

//	int rsv[2];
}MainPara;

extern MainPara mainPara;


typedef struct RFGROUP_COMMUNICATION_PARA
{
	unsigned char optCode;
	unsigned char ttlExpVal;
	unsigned char totalLinkedDevice;
}RfGr_CmuPara;

typedef struct	 FLASHPARAMETER
{
	signed char FEB_count;		//after setting surranding, this parameter record the flash times  设置闪光包围之后闪光次数的记录
	//char rear_syn;		//mode certain sychronic 1:rear certain 2: frount certian    前后帘模式，前帘为0后帘为1
	signed char ttl_FEB1;
	signed char ttl_FEB2;
	signed char cmrCmp1;
	signed char cmrCmp2;
	unsigned char cmrCmpCode;
	signed char sleep;				//recored the sleep statue   是否进入睡眠模式
	signed char charge_ok;		//recored the charge statue	时候充电完成 0 充电中 1：充电可闪 2：充电到可发送指令
	signed char statue_link;	//recored whether linking the camera；		时候连接相机
	signed int iso;					//record the iso setting of camera				相机的iso
	unsigned int apert;	//record the apert setting of camera		相机的光圈值
	unsigned int flaRecord;	//record the flash times of flashlite		记录闪光灯闪光的次数
	unsigned char ledStatue;
	unsigned char surrand_state;
	unsigned char hpFlash;
	unsigned char isoCmp;
	unsigned char shutter;
	unsigned char preflashIndex;
	unsigned char rfGrInfoFlag[5];
	RfGr_CmuPara rfCmuPara[5];
	int quickTime;	
	//unsigned char staLink;
	//unsigned char hpFlash;
}FlaParameter;

extern FlaParameter flaPara;

typedef struct MOTO_SET
{
	char motoDir;      //direction  马达方向，根据定位开关设置，压下定位开关为0，松开定位开关为1
	char moto_en;		//enable   马达移动使能
	int moto_step;		//indicate step to move 记录马达移动的步数
	char moto_ready;	//indicate finished a turn on set up  马达复位时置1， 复位完成清0
	//unsigned char current_location; 	  //当前的位置
	u16 current_location;
	u16 *dest_location;     //需要移动的目的位置
	u16 autoDest;
	char destRsv;
#ifdef _CANON_
	unsigned char modRsv;
#endif
	char moving;		//indicate the moto is on the moving, so this is no response to new location 记录马达是否在转动
	u16 currentSet;
	char destSet;
	char focLen;		//显示在屏幕上的马达位置
	char autoRsv;
	signed char focIndex;  //马达在手动模式下的索引值
	unsigned char *mode;			//mode manu or auto 莫达模式，0为auto， 1为manu
	//char motoRstCount; //通知主控停止充电位，当马达转动时，需要通知主控停止充电
	u16 motoRstCount;
	//char motoRstCount;  //复位的时间值，当超过一定时间马达复位没有完成时，显示马达异常
	char resetFlag;  //复位标志，当该标志置位时，马达复位
	char uartSendFlag;
}MotoSet;

extern MotoSet motoSet;

typedef struct WL_SLAVE_PARA
{
	unsigned char *s_chan;
	unsigned char *s_array;
	unsigned char s_wlMode;
	//unsigned char s_flashStatue;
	unsigned char s_extCode;
	unsigned int testCnt;
	unsigned char testPilot;
	
}Wl_Slave;

extern Wl_Slave wlSlave;


typedef struct WL_SET
{		
	int  m_atvArr;			
	signed char *m_chan;				//
#ifdef _NIKON_
	//WL_ArrPara m_arrPara[4];		//record the respective arrary's parameter
#endif
	char m_current_arr;					//the selecting arrary currently
#ifdef _CANON_
//	unsigned char m_chan;
	//unsigned char *m_on;
	//unsigned char m_mode;
	//unsigned char m_preIndex;
	unsigned char *is_masterOn;
	unsigned char abCmp;
	signed char *abRatio;
	unsigned char cCmp;
	signed char cmrCmp1;
	signed char cmrCmp2;
	//unsigned char otpCode;
	//unsigned char otpCode[3];
	//unsigned char BotpCode;
	//unsigned char CotpCode;
	//WL_ArrPara m_arrPara[4];
	signed char *arr_mode;
	signed char *ttl_Ratio;
	WL_ArrPara *m_arrPara;
	unsigned char WlmMul[3];
	char m_flaCmd[8];		//this is load for flash command data
	//unsigned char m_isMstFla;
	char m_cmdIndex;
	unsigned char m_cmdSendId;
	unsigned char m_cmdProc;
	unsigned char m_flaConfirm;
	unsigned char m_ttlCmd[3];
	unsigned int muitl_realTimes;
#endif	
}WlSet;

extern WlSet wlSet;

#if 0
typedef struct INT_PARA_FLAG
{
	char iso;
	char apert;
	char focLen;
	char rmtPara;
	char mod;
	char cls;
	char fp;
	char sleep;
	char preserve1;
	char preserve2;
	unsigned char psvPara[4];
#ifdef _CANON_
	char ttlCmp;
	char mulSet;
	char abR;
	char cCmp;
	char wlmOpt;
	char mulWlm;
	char mstFla;
	char otp;
	char ext;
	char exitSlave;
	//char bOpt;
	//char cOpt;
#endif
	unsigned char paraRes;
#ifdef HP_SAMPLE_TEST
	char hpsSample;
	unsigned int hpsVal;
	unsigned int hpsValRev;
#endif
}IntParaFlag;

extern IntParaFlag intParaFlag;
#endif


#define EEPROM_INDEX_MODE						0		//保存模式，第8bit保存前后帘同步，第七bit表示是否开启声音，其余六位保存ttl,manu,multi,master模�
#ifdef _CANON_
#endif
#ifdef _NIKON_
#define EEPROM_INDEX_TTLCMP					1		//保存ttl模式的补偿值	前4bit保存粗调值，后4bit保存微调值
#define EEPROM_INDEX_EXP						2		//保存manu模式的补偿值	前4bit保存粗调值，后4bit保存微调值
#define EEPROM_INDEX_MULTI_EXPOSE			3		//保存multi模式的补偿值	前4bit保存粗调值，后4bit保存微调值
#endif
#define EEPROM_INDEX_MOTOMODE				5		//第八个bit保存马达工作模式，其余bit保存手动模式下的索引值
#define EEPROM_INDEX_LOCATION					6		//保存马达的位置值
#define EEPROM_INDEX_SWL_CHANARR				8		//this is presevered for wireless slave mode
#define EEPROM_INDEX_MWL_ABCMODE			9		//主控模式下ABC三组的模式，每一组用两个bit表示

#define EEPROM_INDEX_MWL_ARRMEXP				10
#define EEPROM_INDEX_MWL_ARRAEXP				11	//主控模式下A组M模式下的闪光输出
#define EEPROM_INDEX_MWL_ARRBEXP				12	//主控模式下B组M模式下的闪光输出
#define EEPROM_INDEX_MWL_ARRCEXP				13		//主控模式下C组M模式下的闪光输出


#define EEPROM_INDEX_MWL_ARRMCMP			14
#define EEPROM_INDEX_MWL_ARRACMP			15	//主控模式下A组TTL模式下的补偿值
#define EEPROM_INDEX_MWL_ARRBCMP				16	//主控模式下B组TTL模式下的补偿值
#define EEPROM_INDEX_MWL_ARRCCMP				17		//主控模式下C组TTL模式下的补偿值

#define EEPROM_INDEX_MULTI_TIMES				7	//频闪模式下的闪光次数
#define EEPROM_INDEX_MULTI_FRQ				4	//频闪模式下的闪光频率


#ifdef _CANON_
#define EEPROM_CANON_RATIO   			EEPROM_INDEX_MWL_ABCMODE      //QUETO AS 580EX
#define EEPROM_CANON_ARRAEXP      		EEPROM_INDEX_MWL_ARRAEXP
#define EEPROM_CANON_ARRBEXP	    		EEPROM_INDEX_MWL_ARRBEXP
#define EEPROM_CANON_ARRCEXP      		EEPROM_INDEX_MWL_ARRCEXP
//#define EEPROM_CANON_CMPAB	    		EEPROM_INDEX_MWL_ARRACMP
#define EEPROM_CANON_ABR        	             	EEPROM_INDEX_MWL_ARRBCMP
#define EEPROM_CANON_CMPC            	      	EEPROM_INDEX_MWL_ARRCCMP
#define EEPROM_CANON_MULARRA	      		18//EEPROM_INDEX_MWL_ARRACMP
#define EEPROM_CANON_MULARRB      	       19//18
#define EEPROM_CANON_MULARRC	     		20//19
#define EEPROM_CANON_CMPA				21//20
#define EEPROM_CANON_CMPB				22//21
#define EEPROM_INDEX_TTLCMP			EEPROM_CANON_CMPA
#define EEPROM_INDEX_EXP				EEPROM_CANON_ARRAEXP
#define EEPROM_INDEX_MULTI_EXPOSE	EEPROM_CANON_MULARRA
#define EEPROM_CANON_MSTFLA			26
#define EEPROM_CANON_SYNMODE			27
#endif


#define EEPROM_INDEX_COUNTER1		24 //23
#define EEPROM_INDEX_COUNTER2 	25//24


#if 0
#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_MULTI 	2
#ifdef _NIKON_
#define WORKINGMODE_MASTER	3
#endif
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#ifdef _CANON_
#define WORKINGMODE_GR		6
#define WORKINGMODE_EXTA		7
#endif
#ifdef _SPEED_LIGHT_
#define WORKINGMODE_F1		6
#define WORKINGMODE_F2		7
#endif
#define WORKINGMODE_OFF		0xff
#define WORKINGMODE_AUTO		2
#define WORKINGMODE_TURNOFF	3

#define WORKINGMODE_SIGN 		0xcc
#endif

#endif

