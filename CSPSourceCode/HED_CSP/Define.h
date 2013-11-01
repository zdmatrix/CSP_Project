#ifndef _DEFINE_H
#define _DEFINE_H
#include <wincrypt.h>
#include "OPENSSL\SHA.h"
#include "OPENSSL\md5.h"

const unsigned short OPERATION_SUCCESS = 0x9000;
const unsigned short VERIFY_FAILED = 0x63C;

const int LANGUAGE_CHINESE = 1;
const int LANGUAGE_ENGLISH = 2;


#define		PUBLIC_KEY_FLAG		0x00
#define		PRIVATE_KEY_FLAG		0x01

//typedef unsigned int ALG_ID;
struct CardContainer{
	BYTE	pbContName[64];//��Կ��������
	ALG_ID	Algid;	       //��Կ�㷨��ʶ
	BYTE	dwParam;       //��Կ����  �Ƿ���Ե�����
	WORD	wKeyLen;       //��Կ����
	WORD	wKeySSfi;      //˽Կ��ַ
	WORD	wKeyPSfi;      //��Կ��ַ
	BYTE	pbCertName[32];//֤������
	WORD	wCertAddr;     //֤�����ַ
	WORD	wCertSize;     //֤�����С
};

struct AlgEnumalgs
{
	BYTE bAlgIndex;
	BYTE bAlgMax;
	PROV_ENUMALGS  strAlg[10];
};
struct AlgEnumalgsEx
{
	BYTE bAlgIndex;
	BYTE bAlgMax;
	PROV_ENUMALGS_EX  strAlgEx[10];
}; 
struct HashObject
{
	ALG_ID Algid;
	BOOL	bIsFinished;	
	//bool is hash state
	DWORD	HashLen;
	HCRYPTKEY hKey;
	BYTE	pbHashData[40];//���ΪSHA120���ֽ�
	DWORD	dwOIDLen;
	BYTE	OID[40];
	//���Զ�ν���ժҪ
	SHA_CTX SHA1_context;
	MD5_CTX MD5_context;
};

struct KeyObject
{
	ALG_ID	Algid;               //KP_ALGID
	DWORD	dwEffectiveKeyLen;   //KP_EFFECTIVE_KEYLEN
	DWORD	dwIVLen;             //KP_IV
	BYTE	bIV[8];              //KP_IV
	BYTE	bIVFact[8];          //KP_IV
	DWORD	dwMode;              //KP_MODE
	DWORD	dwModeBits;          //KP_MODE_BITS
	DWORD	dwPadding;           //KP_PADDING
	DWORD	dwPermissions;       //KP_PERMISSIONS
	DWORD	dwSaltLen;           //KP_SALT
	BYTE	bSalt[20];           //KP_SALT
	DWORD	dwBlockLen;          //KP_BLOCKLEN��bitΪ��λ�ĳ���
	//�Գ���Կ
	DWORD	dwKeyLen;            //KP_KEYLEN ��bitΪ��λ�ĳ���
	BYTE	bKey[128];           //
	//��˽Կ��
	BOOL	bIsKeyPairs;
	WORD	wPsfi;               //��Կ�ļ���ʶ��
	WORD	wSsfi;               //˽Կ�ļ���ʶ��
};



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//��;���ڵ���CpReleaseContext()��ʱ���ͷ�������hProv�йص���Դ
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
struct CspContainer{	//���գ�CPGetProvParam()
	HANDLE hUKey;
	DWORD	dwFlagsAQ;//������Կ����ʱָ����dwFlag,
						//���dwFlag=CRYPT_VERIFYCONTEX��Ϊ������Կ
						//��˽��Կ����bPkey,bSkey��
	char	cProvider[256];
	DWORD	dwImptype;
	DWORD	dwVersion;
	WORD	wSigKeySizeInc;//->PP_SIG_KEYSIZE_INC
	WORD	wKeyxKeySizeInc;//->PP_KEYX_KEYSIZE_INC
	LPCSTR	szKeySetSecDescr;//->PP_KEYSET_SEC_DESCR
	DWORD	dwProvType;		//->PP_PROVTYPE
//����Ϊ�������CPGetUserKey��CPExportKey
	DWORD	bHaveDefaultKey;
	DWORD	dwPermissions;//Ȩ��
	struct	CardContainer strCardContainer;
};

#endif