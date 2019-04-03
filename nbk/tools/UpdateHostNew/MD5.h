#ifndef _HUAXIN_MD5_
#define _HUAXIN_MD5_

/* POINTER defines a generic pointer type */
typedef unsigned char * POINTER;

/* UINT2 defines a two byte word */
//typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;


/* MD5 context. */
typedef struct {
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

class MD5_ENC
{
private:
	MD5_CTX md5c; 
	//MD5_ENC();
	void MD5_memcpy (POINTER output, POINTER input, unsigned int len);
	void MD5Transform (UINT4 state[4], unsigned char block[64]);
	void Encode (unsigned char *output, UINT4 *input, unsigned int len);
	void MD5_memset (POINTER output, int value, unsigned int len);
	void Decode (UINT4 *output, unsigned char *input, unsigned int len);
public:
	void MD5Init (MD5_CTX *context);
	void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
	void MD5UpdaterString(MD5_CTX *context,const char *string);
	int MD5FileUpdateFile (MD5_CTX *context,char *filename);
	void MD5Final (unsigned char digest[16], MD5_CTX *context);
#if 0
	void MD5_Block(char *pBlcok, unsigned int len, unsigned char digest[16]);
#endif
	int HX_FwMd5Check(char *fmw);
	void MDString (char *string,unsigned char digest[16]);
	int MD5File (char *filename,unsigned char digest[16]);
};

#endif

