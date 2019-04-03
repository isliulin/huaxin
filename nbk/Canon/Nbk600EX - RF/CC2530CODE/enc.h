#ifndef _ENC_
#define _ENC_

typedef unsigned int u32;

class EncMethod
{
public:
	EncMethod();
	~EncMethod();

private:
	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);
	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);
	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
	void Aes_Encrypt(unsigned char* input);
	void Aes_Decrypt(unsigned char* input);
	unsigned char w[11][4][4];

public:
	void Boot_Encrypt(char *plainBlock, char *keyStr,char *cipherBlock, u32 size);
	void Boot_Decrypt(char *cipherBlock, char *keyStr,char *plainBlock,u32 size);
};

#endif
