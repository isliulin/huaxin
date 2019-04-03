// TriDesEnc.cpp : implementation file
//

#include "stdafx.h"
#include "usbhidioc.h"
#include "TriDesEnc.h"
#include "EncTable.h"
#include <fcntl.h>
#include <io.h> 
#include <fstream>


void TriDesEnc::GenerateSamdonNumber(char *samNum)
{
	int i;

	srand(clock());
	for(i = 0;i < RAMDOM_KEY_LEN;i++) 
	{
		samNum[i] = rand() % 100;
	}

}

int TriDesEnc::ByteToBit(ElemType ch, ElemType bit[8])
{
	int cnt;
	for(cnt = 0;cnt < 8; cnt++){
		*(bit+cnt) = (ch>>cnt)&1;
	}
	return 0;
}
  
int TriDesEnc::BitToByte(ElemType bit[8],ElemType *ch)
{
	int cnt;
	for(cnt = 0;cnt < 8; cnt++){
		*ch |= *(bit + cnt)<<cnt;
	}
	return 0;
}
  
int TriDesEnc::Char8ToBit64(ElemType ch[8],ElemType bit[64])
{
	int cnt;
	for(cnt = 0; cnt < 8; cnt++){  
		ByteToBit(*(ch+cnt),bit+(cnt<<3));
	}
	return 0;
}
  
int TriDesEnc::Bit64ToChar8(ElemType bit[64],ElemType ch[8])
{
	int cnt;
	memset(ch,0,8);
	for(cnt = 0; cnt < 8; cnt++){
		BitToByte(bit+(cnt<<3),ch+cnt);
	}
	return 0;
}
  
int TriDesEnc::DES_MakeSubKeys(ElemType key[64],ElemType subKeys[16][48])
{
	ElemType temp[56];
	int cnt;
	DES_PC1_Transform(key,temp);
	for(cnt = 0; cnt < 16; cnt++){
		DES_ROL(temp,MOVE_TIMES[cnt]);
		DES_PC2_Transform(temp,subKeys[cnt]);
	}
	return 0;
}
  

int TriDesEnc::DES_PC1_Transform(ElemType key[64], ElemType tempbts[56])
{
	int cnt; 
	for(cnt = 0; cnt < 56; cnt++){
		tempbts[cnt] = key[PC_1[cnt]];
	}
	return 0;
}

int TriDesEnc::DES_PC2_Transform(ElemType key[56], ElemType tempbts[48])
{
	int cnt;
	for(cnt = 0; cnt < 48; cnt++){
		tempbts[cnt] = key[PC_2[cnt]];
	}
	return 0;
}
  
int TriDesEnc::DES_ROL(ElemType data[56], int time)
{
	ElemType temp[56];
	
	memcpy(temp,data,time);
	memcpy(temp+time,data+28,time);
	
	memcpy(data,data+time,28-time);
	memcpy(data+28-time,temp,time);
	
	memcpy(data+28,data+28+time,28-time);
	memcpy(data+56-time,temp+time,time); 
	
	return 0;
}
  
int TriDesEnc::DES_IP_Transform(ElemType data[64])
{
	int cnt;
	ElemType temp[64];
	for(cnt = 0; cnt < 64; cnt++){
		temp[cnt] = data[IP_Table[cnt]];
	}
	memcpy(data,temp,64);
	return 0;
}
  
int TriDesEnc::DES_IP_1_Transform(ElemType data[64])
{
	int cnt;
	ElemType temp[64];
	for(cnt = 0; cnt < 64; cnt++){
		temp[cnt] = data[IP_1_Table[cnt]];
	}
	memcpy(data,temp,64);
	return 0;
}
  
int TriDesEnc::DES_E_Transform(ElemType data[48])
{
	int cnt;
	ElemType temp[48];
	for(cnt = 0; cnt < 48; cnt++){
		temp[cnt] = data[E_Table[cnt]];
	}
	memcpy(data,temp,48);
	return 0;
}
  
int TriDesEnc::DES_P_Transform(ElemType data[32])
{
	int cnt;
	ElemType temp[32];
	for(cnt = 0; cnt < 32; cnt++){
		temp[cnt] = data[P_Table[cnt]];
	}
	memcpy(data,temp,32);
	return 0;
}
  
int TriDesEnc::DES_XOR(ElemType R[48], ElemType L[48] ,int count)
{
	int cnt;
	for(cnt = 0; cnt < count; cnt++){
		R[cnt] ^= L[cnt];
	}
	return 0;
}
  
int TriDesEnc::DES_SBOX(ElemType data[48])
{
	int cnt;
	int line,row,output;
	int cur1,cur2;
	for(cnt = 0; cnt < 8; cnt++){
		cur1 = cnt*6;
		cur2 = cnt<<2;
		
		line = (data[cur1]<<1) + data[cur1+5];
		row = (data[cur1+1]<<3) + (data[cur1+2]<<2)
			+ (data[cur1+3]<<1) + data[cur1+4];
		output = S[cnt][line][row];
		
		data[cur2] = (output&0X08)>>3;
		data[cur2+1] = (output&0X04)>>2;
		data[cur2+2] = (output&0X02)>>1;
		data[cur2+3] = output&0x01;
 }
 return 0;
}
  
int TriDesEnc::DES_Swap(ElemType left[32], ElemType right[32])
{
	ElemType temp[32];
	memcpy(temp,left,32);
	memcpy(left,right,32);  
	memcpy(right,temp,32);
	return 0;
}
  
/*���ܵ�������*/
int TriDesEnc::DES_EncryptBlock(ElemType plainBlock[8], ElemType subKeys[16][48], ElemType cipherBlock[8])
{
	ElemType plainBits[64];
	ElemType copyRight[48];
	int cnt;
	
	Char8ToBit64(plainBlock,plainBits);  
	/*��ʼ�û���IP�û���*/
	DES_IP_Transform(plainBits);
	
	/*16�ֵ���*/
	for(cnt = 0; cnt < 16; cnt++){ 
		memcpy(copyRight,plainBits+32,32);
		/*���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ*/
		DES_E_Transform(copyRight);
		/*���Ұ벿��������Կ����������*/
		DES_XOR(copyRight,subKeys[cnt],48); 
		/*���������S�У����32λ���*/
		DES_SBOX(copyRight);
		/*P�û�*/
		DES_P_Transform(copyRight);
		/*��������벿�����Ұ벿�ֽ������*/
		DES_XOR(plainBits,copyRight,32);
		if(cnt != 15){
			/*����������Ҳ��Ľ���*/
			DES_Swap(plainBits,plainBits+32);
		}
	}
	/*���ʼ�û���IP^1�û���*/
	DES_IP_1_Transform(plainBits);
	Bit64ToChar8(plainBits,cipherBlock);
	return 0;
}
  
/*���ܵ�������*/
int TriDesEnc::DES_DecryptBlock(ElemType cipherBlock[8], ElemType subKeys[16][48],ElemType plainBlock[8])
{
	ElemType cipherBits[64];
	ElemType copyRight[48];
	int cnt;
	
	Char8ToBit64(cipherBlock,cipherBits); 
	/*��ʼ�û���IP�û���*/
	DES_IP_Transform(cipherBits);
	
	/*16�ֵ���*/
	for(cnt = 15; cnt >= 0; cnt--){
		memcpy(copyRight,cipherBits+32,32);
		/*���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ*/
		DES_E_Transform(copyRight);
		/*���Ұ벿��������Կ����������*/
		DES_XOR(copyRight,subKeys[cnt],48);  
		/*���������S�У����32λ���*/
		DES_SBOX(copyRight);
		/*P�û�*/
		DES_P_Transform(copyRight);  
		/*��������벿�����Ұ벿�ֽ������*/
		DES_XOR(cipherBits,copyRight,32);
		if(cnt != 0){
			/*����������Ҳ��Ľ���*/
			DES_Swap(cipherBits,cipherBits+32);
		}
	}
	/*���ʼ�û���IP^1�û���*/
	DES_IP_1_Transform(cipherBits);
	Bit64ToChar8(cipherBits,plainBlock);
	return 0;
}

int TriDesEnc::DES_EncryptEx(char *plainBlock, char *keyStr,char *cipherBlock)
{
	//ElemType cipherBlock[8],
	ElemType keyBlock[8];
	ElemType bKey[64];
	ElemType subKeys[16][48];
	memcpy(keyBlock,keyStr,8);
	Char8ToBit64(keyBlock,bKey);
	DES_MakeSubKeys(bKey,subKeys);
	DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
	return OK;
}

int TriDesEnc::DES_DecryptEx(char *cipherBlock, char *keyStr,char *plainBlock)
{
	//ElemType cipherBlock[8],keyBlock[8];
	ElemType keyBlock[8];
	ElemType bKey[64];
	ElemType subKeys[16][48];
	memcpy(keyBlock,keyStr,8);
	Char8ToBit64(keyBlock,bKey);
	DES_MakeSubKeys(bKey,subKeys);
	DES_DecryptBlock(cipherBlock,subKeys,plainBlock);	
	return OK;
}

/*�����ļ�*/
int TriDesEnc::DES_Encrypt(char *plainFile, char *keyStr,char *cipherFile)
{
	FILE *plain,*cipher;
	int count;
	ElemType plainBlock[8],cipherBlock[8],keyBlock[8];
	ElemType bKey[64];
	ElemType subKeys[16][48];
//	if((plain = fopen(plainFile,"rb")) == NULL)
	if (fopen_s(&plain,plainFile,"rb")!=NULL)
	{
		return PLAIN_FILE_OPEN_ERROR;
	}

	int size = _filelength(_fileno(plain));
	int prog = 0;

//	int size = plain.GetLength();

//	if((cipher = fopen(cipherFile,"wb")) == NULL)
	if (fopen_s(&cipher,cipherFile,"wb")!=NULL)
	{
		return CIPHER_FILE_OPEN_ERROR;
	}
	/*������Կ*/
	memcpy(keyBlock,keyStr,8);
	/*����Կת��Ϊ��������*/
	Char8ToBit64(keyBlock,bKey);
	/*��������Կ*/
	DES_MakeSubKeys(bKey,subKeys);
	
	while(!feof(plain)){
		/*ÿ�ζ�8���ֽڣ������سɹ���ȡ���ֽ���*/
		if((count = fread(plainBlock,sizeof(char),8,plain)) == 8){
			DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
			fwrite(cipherBlock,sizeof(char),8,cipher);  
		}
	}
	if(count){
		/*���*/
		memset(plainBlock + count,'\0',7 - count);
		/*���һ���ַ�����������һ���ַ����ڵ��������ַ�����*/
		plainBlock[7] = 8 - count;
		DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
		fwrite(cipherBlock,sizeof(char),8,cipher);
	}
	
	//g_progressValue = (++prog)/size;

	fclose(plain);
	fclose(cipher);
	return OK;
}
  
/*�����ļ�*/
int TriDesEnc::DES_Decrypt(char *cipherFile, char *keyStr,char *plainFile)
{
	FILE *plain, *cipher;
	int count,times = 0;
	long fileLen;
	ElemType plainBlock[8],cipherBlock[8],keyBlock[8];
	ElemType bKey[64];
	ElemType subKeys[16][48];
//	if((cipher = fopen(cipherFile,"rb")) == NULL)
	if (fopen_s(&cipher,cipherFile,"rb")!=NULL)
	{
		return CIPHER_FILE_OPEN_ERROR;
	}
	//if((plain = fopen(plainFile,"wb")) == NULL)
	if (fopen_s(&plain,plainFile,"wb")!=NULL)
	{
		return PLAIN_FILE_OPEN_ERROR;
	}
	
	/*������Կ*/
	memcpy(keyBlock,keyStr,8);
	/*����Կת��Ϊ��������*/
	Char8ToBit64(keyBlock,bKey);
	/*��������Կ*/
	DES_MakeSubKeys(bKey,subKeys);
	
	/*ȡ�ļ����� */
	fseek(cipher,0,SEEK_END);/*���ļ�ָ����β*/
	fileLen = ftell(cipher); /*ȡ�ļ�ָ�뵱ǰλ��*/
	rewind(cipher); /*���ļ�ָ����ָ���ļ�ͷ*/
	while(1){
		/*���ĵ��ֽ���һ����8��������*/
		fread(cipherBlock,sizeof(char),8,cipher);
		DES_DecryptBlock(cipherBlock,subKeys,plainBlock);  
		times += 8;
		if(times < fileLen){
			fwrite(plainBlock,sizeof(char),8,plain);
		}
		else{
			break;
		}
	}
	/*�ж�ĩβ�Ƿ����*/
	if(plainBlock[7] < 8){
		for(count = 8 - plainBlock[7]; count < 7; count++){
			if(plainBlock[count] != '\0'){
				break;
			}
		}
	}
	if(count == 7){/*�����*/
		fwrite(plainBlock,sizeof(char),8 - plainBlock[7],plain);
	}
	else{/*�����*/
		fwrite(plainBlock,sizeof(char),8,plain);
	}
	
	fclose(plain);
	fclose(cipher);
	return OK;
}  

/////////////////////////////////////////////////////////////////////////////
// TriDesEnc diagnostic

/////////////////////////////////////////////////////////////////////////////
// TriDesEnc message handlers
