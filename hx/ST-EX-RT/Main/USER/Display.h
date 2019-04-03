#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//#include "uart.h"
//#define char signed char
#include "menu.h"
#define MODE_COMMON_ARR	3
#define MODE_GR_ARR		5
#define MASTER				0

#define RF_GROUP_TOTAL 5

#ifdef SN_DEBUG
//#define BOOT_UPDATE
#endif

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
	char multi_exp;
#endif
	
}WL_ArrPara;


typedef struct RADIO_STATUE_PARA
{
	unsigned char cmdReady;
	unsigned char cmdSta;
	unsigned char devLinked;
	unsigned char devCnt[5];
	unsigned char adept;
	unsigned char rssi[16];
	unsigned char cmpFlag;
	unsigned char delay_mod;		// different  in 5dmarkIII 
}RadioStatue;

extern WL_ArrPara *mstPara;

typedef struct GROUP_ARRPARA
{
	signed char mode;
	signed char m_exp1;
	signed char m_exp2;
	signed char ttl_cmp1;
	signed char ttl_cmp2;
}RFGr_ArrPara;

typedef struct RADIO_SETTING
{
	signed char rfChan;
	unsigned int id;
	//signed char rfChanSet;
	//unsigned int idSet;
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
	unsigned char moto_location;
}MotoLoca;


typedef struct MAIN_PARA_SET
{
	signed char workMode;
	unsigned char  cmdMode;
	//signed char m_mstOn;
	signed char m_chan;
	signed char arr_mode;
	unsigned char m_isMstFla;
	WL_ArrPara modGr[MODE_COMMON_ARR];	

	unsigned char s_chan;
	unsigned char s_array;
	
	MotoLoca motoLoca;
	
	signed char wlSlave_chan;
	signed char wlSlave_arr;
	RadioSet rfSet;
	//unsigned char multi_exp;
	int multi_Frq;
	int multi_times;
	signed char synMode;
	unsigned int counter;	
	char snrMenuSet[MENU_ITEMS];

#ifdef BOOT_UPDATE
	u16 bootUp;
#endif


	u8 sumVeri;

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
	signed char FEB_count;		//after setting surranding, this parameter record the flash times  ���������Χ֮����������ļ�¼
	signed char febEn;
	//char rear_syn;		//mode certain sychronic 1:rear certain 2: frount certian    ǰ����ģʽ��ǰ��Ϊ0����Ϊ1
	signed char ttl_FEB1;
	signed char ttl_FEB2;
	signed char sleep;				//recored the sleep statue   �Ƿ����˯��ģʽ
	signed char charge_ok;		//recored the charge statue	ʱ������� 0 ����� 1�������� 2����絽�ɷ���ָ��
	signed char statue_link;	//recored whether linking the camera��		ʱ���������
	signed int iso;					//record the iso setting of camera				�����iso
	unsigned int apert;	//record the apert setting of camera		����Ĺ�Ȧֵ
	unsigned int flaRecord;	//record the flash times of flashlite		��¼���������Ĵ���
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
	char motoDir;      //direction  ��﷽�򣬸��ݶ�λ�������ã�ѹ�¶�λ����Ϊ0���ɿ���λ����Ϊ1
	char moto_en;		//enable   ����ƶ�ʹ��
	int moto_step;		//indicate step to move ��¼����ƶ��Ĳ���
	char moto_ready;	//indicate finished a turn on set up  ��︴λʱ��1�� ��λ�����0
	unsigned char current_location; 	  //��ǰ��λ��
	unsigned char *dest_location;     //��Ҫ�ƶ���Ŀ��λ��
	unsigned char destRsv;
#ifdef _CANON_
	unsigned char modRsv;
#endif
	char moving;		//indicate the moto is on the moving, so this is no response to new location ��¼����Ƿ���ת��
	char focLen;		//��ʾ����Ļ�ϵ����λ��
	signed char focIndex;  //������ֶ�ģʽ�µ�����ֵ
	unsigned char *mode;			//mode manu or auto Ī��ģʽ��0Ϊauto�� 1Ϊmanu
	char motoRstCount; //֪ͨ����ֹͣ���λ�������ת��ʱ����Ҫ֪ͨ����ֹͣ���
	//char motoRstCount;  //��λ��ʱ��ֵ��������һ��ʱ����︴λû�����ʱ����ʾ����쳣
	char resetFlag;  //��λ��־�����ñ�־��λʱ����︴λ
	char uartSendFlag;
}MotoSet;

extern MotoSet motoSet;

typedef struct WL_SLAVE_PARA
{
	unsigned char *s_chan;
	unsigned char *s_array;
	unsigned char s_wlMode;
	unsigned char s_flashStatue;
	unsigned char s_extCode;
	
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
	signed char cCmp;
	//unsigned char cCmp;
	//unsigned char otpCode;
	//unsigned char otpCode[3];
	//unsigned char BotpCode;
	//unsigned char CotpCode;
	//WL_ArrPara m_arrPara[4];
	signed char *arr_mode;
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


#define EEPROM_INDEX_MODE						0		//����ģʽ����8bit����ǰ����ͬ��������bit��ʾ�Ƿ���������������λ����ttl,manu,multi,masterģ�
#ifdef _CANON_
#endif
#ifdef _NIKON_
#define EEPROM_INDEX_TTLCMP					1		//����ttlģʽ�Ĳ���ֵ	ǰ4bit����ֵ�ֵ����4bit����΢��ֵ
#define EEPROM_INDEX_EXP						2		//����manuģʽ�Ĳ���ֵ	ǰ4bit����ֵ�ֵ����4bit����΢��ֵ
#define EEPROM_INDEX_MULTI_EXPOSE			3		//����multiģʽ�Ĳ���ֵ	ǰ4bit����ֵ�ֵ����4bit����΢��ֵ
#endif
#define EEPROM_INDEX_MOTOMODE				5		//�ڰ˸�bit������﹤��ģʽ������bit�����ֶ�ģʽ�µ�����ֵ
#define EEPROM_INDEX_LOCATION					6		//��������λ��ֵ
#define EEPROM_INDEX_SWL_CHANARR				8		//this is presevered for wireless slave mode
#define EEPROM_INDEX_MWL_ABCMODE			9		//����ģʽ��ABC�����ģʽ��ÿһ��������bit��ʾ

#define EEPROM_INDEX_MWL_ARRMEXP				10
#define EEPROM_INDEX_MWL_ARRAEXP				11	//����ģʽ��A��Mģʽ�µ��������
#define EEPROM_INDEX_MWL_ARRBEXP				12	//����ģʽ��B��Mģʽ�µ��������
#define EEPROM_INDEX_MWL_ARRCEXP				13		//����ģʽ��C��Mģʽ�µ��������


#define EEPROM_INDEX_MWL_ARRMCMP			14
#define EEPROM_INDEX_MWL_ARRACMP			15	//����ģʽ��A��TTLģʽ�µĲ���ֵ
#define EEPROM_INDEX_MWL_ARRBCMP				16	//����ģʽ��B��TTLģʽ�µĲ���ֵ
#define EEPROM_INDEX_MWL_ARRCCMP				17		//����ģʽ��C��TTLģʽ�µĲ���ֵ

#define EEPROM_INDEX_MULTI_TIMES				7	//Ƶ��ģʽ�µ��������
#define EEPROM_INDEX_MULTI_FRQ				4	//Ƶ��ģʽ�µ�����Ƶ��


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

