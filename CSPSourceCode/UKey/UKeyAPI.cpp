#include "StdAfx.h"
#include "UKeyAPI.h"
#include "HDIFD20B.H"
#include "../HED_CSP/Define.h"
#include "CheckPin.h"
#include "ChangePin.h"
#include "../HED_CSP/RSAREF.h"
#include <X509.h>

#define WRITE_DEBUG_MESSAGE
typedef struct Usb_Key_State 
{
	HANDLE gKeysHandle;
	int nRefCount;
}USB_KEY_STATE, *PUSB_KEY_STATE;

DWORD g_dwUkeyError = 0;
int gLanguage = LANGUAGE_CHINESE;
PUSB_KEY_STATE pUsbState;
BOOL HaveVerified = FALSE;
char PinCached[16] = {0};
int PinCachedLen = 0;

void DebugMessage(const char * message)
{
#ifdef WRITE_DEBUG_MESSAGE
	FILE *f = fopen("c:\\PKISystemLog.txt", "a+");
	fprintf(f, message);
	fclose(f);
#endif
}

BOOL APDUCommand(HANDLE hUKey, BYTE CLA, BYTE INS, BYTE P1, BYTE P2, BYTE LC, BYTE *DataIn, int DataInLen)
{
	BYTE apduData[255] = {0};
	
	apduData[0] = CLA; //cls
	apduData[1] = INS; //ins
	apduData[2] = P1; //p1
	apduData[3] = P2;   //p2
	apduData[4] = LC;   //lc
	
	short srAPDULen = 5 + DataInLen;
	if (DataInLen > 0)
		memcpy(&apduData[5], DataIn, DataInLen);
	short respDataLen = 0;
	unsigned char respData[255] = {0};
	
	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
	{
		g_dwUkeyError = ret;
		return FALSE;
	}
	g_dwUkeyError = 0;
	return TRUE;
}

BOOL SelectMF(HANDLE hUKey)
{
	BOOL ret = APDUCommand(hUKey, 0x00, 0xA4, 0x00, 0x00, 0x00, NULL, 0);
	return ret;
}

BOOL SelectADF(HANDLE hUKey)
{
	BYTE Indata[] = {0xAD, 0xF1};
	BOOL ret = APDUCommand(hUKey, 0x00, 0xA4, 0x00, 0x00, 0x02, Indata, 2);
	return ret;
};

void WINAPI SetUkeyLastErr(DWORD dwErr)
{
	g_dwUkeyError = dwErr;
}

DWORD WINAPI GetUkeyLastErr(void)
{
	return g_dwUkeyError;
}
BOOL WINAPI UKeyClearPin(HANDLE hUKey)
{
	HaveVerified = FALSE;
	PinCachedLen = 0;
	memset(PinCached, 0, sizeof(PinCached));
	return TRUE;
}

BOOL IsOpened()
{
	HANDLE h = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "www.hed.com.cn_usb_key");
	if (h == 0)
	{
		h = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(USB_KEY_STATE) , "www.hed.com.cn_usb_key");
		pUsbState = (PUSB_KEY_STATE)MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		pUsbState->gKeysHandle = 0;
		pUsbState->nRefCount = 0;
		//CloseHandle(h);
		return FALSE;
	}
	else
	{
		pUsbState = (PUSB_KEY_STATE)MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pUsbState->gKeysHandle > 0)
			pUsbState->nRefCount ++;
		else
			pUsbState->nRefCount = 0;
		//CloseHandle(h);
		return pUsbState->gKeysHandle > 0;
	}
	
}

BOOL CanClosed()
{
	HANDLE h = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "www.hed.com.cn_usb_key");
	if (h == 0)
		return TRUE;
	pUsbState = (PUSB_KEY_STATE)MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if ((pUsbState->nRefCount < 2) || (pUsbState->gKeysHandle = 0))
	{
		UnmapViewOfFile(pUsbState);
		CloseHandle(h);
		return TRUE;
	}
	else
	{
		pUsbState->nRefCount--;
	}
	
	
}


//打开UKey
BOOL WINAPI UKeyOpen(HANDLE *hUKey)
{
	//如果ukey已经打开，直接返回句柄
	//////////////////////
	
	///////////////////////////////////////////////////
//  	if (IsOpened())
//  	{
//  		DebugMessage("[UKEY] AlReady Exists direct return\n");
//  		//HD_ClosePort(gKeysHandle);
//  		*hUKey = pUsbState->gKeysHandle;
//  		return true;
//      }
	//HaveVerified = FALSE;
	unsigned short ret = HD_OpenPort(21, 9600, 8, hUKey);
	if (ret == OPERATION_SUCCESS)
	{
		DebugMessage("[UKEY] UKeyOpen success\n");
		//pUsbState->gKeysHandle = *hUKey;
		//pUsbState->nRefCount = 1;
		return true;
	}
	else
	{
		DebugMessage("[UKEY] UKeyOpen failed\n");
		//pUsbState->gKeysHandle = 0;
		//pUsbState->nRefCount = 0;
		return false;
	}
}
	
//关闭UKey
BOOL WINAPI UKeyClose(HANDLE hUKey)
{
	///////////////////////////////////////////////////
	DebugMessage("[UKEY] UKeyclose\n");
	///////////////////////////////////////////////////
// 	if (!CanClosed())
// 		return true;
	unsigned short ret = HD_ClosePort(hUKey) ;
	//gKeysHandle = 0;
	return ret == OPERATION_SUCCESS;
}

// 验证Pin
BOOL WINAPI UKeyVerifyPIN(HANDLE hUKey, const char *Pin, const short PinLen)
{
	if ((PinLen % 2) != 0)
	{
		if (gLanguage == LANGUAGE_ENGLISH)
			AfxMessageBox("Pin Length Error");
		else
			AfxMessageBox("口令长度错误");
		return FALSE;
	}

	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0x20; //ins
	apduData[2] = 0x00; //p1
	apduData[3] = 0x3f; //p2
	apduData[4] = PinLen / 2; //lc
	int n = 0;
	int i = 0;
	int j = 5;
	char c;
	while (i < PinLen)
	{
		c = Pin[i];
		if ((c >= 'a') && (c <= 'f'))
			n = 'f' - 'a' + 10;
		else if ((c >= 'A') && (c <= 'F'))
			n = 'F' - 'A' + 10;
		else
			n = atoi(&c);
		i++;
		c = Pin[i];
		if ((c >= 'a') && (c <= 'f'))
			n = n * 16 + 'f' - 'a' + 10;
		else if ((c >= 'A') && (c <= 'F'))
			n = n * 16 + 'F' - 'A' + 10;
		else
			n = n * 16 + atoi(&c);
		//n = n * 16 + atoi(&c);
		apduData[j] = n;
		j++;
		i++;
	}
	short srAPDULen = (short)5+ (PinLen / 2);
	short respDataLen = (short)0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret / 0x10) == VERIFY_FAILED)
	{
		int LeftTimes = ret % 0x10;
		CString msg;		
		if (gLanguage == LANGUAGE_ENGLISH)
			msg.Format("Pin error, please re-enter, %d Left", LeftTimes);
		else
			msg.Format("口令错误，请重新输入，剩余%d次", LeftTimes);
		if (!HaveVerified)
		{
			HaveVerified = false;
			AfxMessageBox(msg);
		}
	}
	if ((ret / 100) == 0xFF)
		SetUkeyLastErr(ret);
	else if (ret != OPERATION_SUCCESS)
		SetUkeyLastErr(0xAAAA);
	  
	return ret == OPERATION_SUCCESS;
}

BOOL WINAPI UKeyCheckPIN(HANDLE hUKey, int nLanguage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (HaveVerified)
	{
		if(UKeyVerifyPIN(hUKey, PinCached, PinCachedLen))
		{
			HaveVerified = true;
			return TRUE;
		}
		else
		{
			UKeyClearPin(hUKey);
		}
	}
	CCheckPin pwd;
	CString szMsg;
	WORD wPinCount=0;
	char bBuf[256];
    if (gLanguage != nLanguage)
		gLanguage = nLanguage;
	pwd.m_Language = nLanguage;
	pwd.m_hUKey = hUKey;
	while(1)
	{
		memset(bBuf,0,sizeof(bBuf));
		if(pwd.DoModal()==IDCANCEL)
		{
			return FALSE;
		}
		memcpy(bBuf,pwd.m_Password,strlen(pwd.m_Password));
		if(UKeyVerifyPIN(hUKey, bBuf, strlen(pwd.m_Password)))
		{
			HaveVerified = TRUE;
			PinCachedLen = strlen(pwd.m_Password);
			memcpy(PinCached, bBuf, PinCachedLen);
			return TRUE;
		}
		else
		{
			HaveVerified = FALSE;
			if (PinCachedLen != 0)
			{
				PinCachedLen = 0;
				memset(PinCached, 0, sizeof(PinCached));
			}
			
			DWORD temp = GetUkeyLastErr();
			if ((temp / 100) == 0xFF)
			{
				DebugMessage("[UKEY]Very Error 0xFF69");
				//return false;
			}
		}
		
	}
}

// 验证Pin
BOOL WINAPI UKeyChangePINToUkey(HANDLE hUKey, const char *Pin, const short PinLen)
{
	if ((PinLen % 2) != 0)
		return FALSE;

	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0x5E; //ins
	apduData[2] = 0x01; //p1
	apduData[3] = 0x3f; //p2
	apduData[4] = PinLen / 2; //lc
	int n = 0;
	int i = 0;
	int j = 5;
	char c;
	while (i < PinLen)
	{
		c = Pin[i];
		if ((c >= 'a') && (c <= 'f'))
			n = 'f' - 'a' + 10;
		else if ((c >= 'A') && (c <= 'F'))
			n = 'F' - 'A' + 10;
		else
			n = atoi(&c);
		i++;
		c = Pin[i];
		if ((c >= 'a') && (c <= 'f'))
			n = n * 16 + 'f' - 'a' + 10;
		else if ((c >= 'A') && (c <= 'F'))
			n = n * 16 + 'F' - 'A' + 10;
		else
			n = n * 16 + atoi(&c);
		apduData[j] = n;
		j++;
		i++;
	}
	short srAPDULen = (short)5+ (PinLen / 2);
	short respDataLen = (short)0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret / 0x10) == VERIFY_FAILED)
	{
		int LeftTimes = ret % 0x10;
		CString msg;		
		if (gLanguage == LANGUAGE_ENGLISH)
			msg.Format("Pin error, please re-enter, %d Left", LeftTimes);
		else
			msg.Format("口令错误，请重新输入，剩余%d次", LeftTimes);
		AfxMessageBox(msg);
	}
	return ret == OPERATION_SUCCESS;
}
BOOL WINAPI UKeyChangePIN(HANDLE hUKey, int nLanguage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CChangePin pwd;
	CString szMsg;
	WORD wPinCount=0;
	char bBuf[256];
	char OldPin[32];
	char NewPin1[32];
	char NewPin2[32];
    int OldPinLen, NewPinLen, BufLen;
	if (gLanguage != nLanguage)
		gLanguage = nLanguage;
	pwd.m_Language = nLanguage;
	while(1)
	{
		memset(OldPin,0,sizeof(bBuf));
		memset(NewPin1,0,sizeof(bBuf));
		memset(NewPin2,0,sizeof(bBuf));

		if(pwd.DoModal()==IDCANCEL)
		{
			return FALSE;
		}

		OldPinLen = strlen(pwd.m_OldPin);
		NewPinLen = strlen(pwd.m_NewPin1);
		memcpy(OldPin,pwd.m_OldPin, OldPinLen);
		memcpy(NewPin1,pwd.m_NewPin1, NewPinLen);
		memcpy(NewPin2,pwd.m_NewPin2,strlen(pwd.m_NewPin2));
		if (strcmp(NewPin1, NewPin2) != 0)
		{
			if (gLanguage == LANGUAGE_ENGLISH)
				AfxMessageBox("The password you Entered twice in different");
			else
				AfxMessageBox("两次输入的口令不一致");
			continue;
		}
        memcpy(bBuf, OldPin, OldPinLen);
		memcpy(&bBuf[OldPinLen], "FF", 2);
		memcpy(&bBuf[OldPinLen + 2], NewPin1, NewPinLen);
		
		BufLen = OldPinLen + 2 + NewPinLen;
		if(UKeyChangePINToUkey(hUKey, bBuf, BufLen))
		{
			return TRUE;
		}
	}
}

BOOL WINAPI UKeyGetChallange(HANDLE hUKey, unsigned char *Challange, const short Len)
{
	DebugMessage("[UKEY] UKeyGetChallange\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0x84; //ins
	apduData[2] = 0x00; //p1
	apduData[3] = 0x00; //p2
	apduData[4] = Len; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;

	memcpy(Challange, respData, Len);
	return TRUE;
}

//读取卡内容器数据
BOOL WINAPI UKeyGetContainerData(HANDLE hUKey, unsigned char *Data, short *DataLength, short *ContainerCount)
{
	DebugMessage("[UKEY] UKeyGetContainerData In\n");
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0xB2; //ins
	apduData[2] = 0x01; //p1
	apduData[3] = 0xD5; //p2 从头读到尾
	apduData[4] = 00; //le

	if (!SelectADF(hUKey))
	{
		DebugMessage("[UKEY] UKeyGetContainerData SelectADF ERROR\n");
		return FALSE;
	}
	short respDataLen = 0;
	unsigned char respData[2000] = {0};

    char msg[100];
	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret != OPERATION_SUCCESS) && ((ret/0x100) != 0x61))
	{
		sprintf(msg, "[UKEY]UKeyGetContainerData1 Error: %x\n", ret);
		DebugMessage(msg);
		return FALSE;
	}
    if (respDataLen == 0)
		return FALSE;
    short LeftLen = 0;
	while (ret != OPERATION_SUCCESS)
	{
		// 读取后续未读出的字节
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; //p1
		apduData[3] = 0x00; //p2 
		LeftLen = ret % 0x100;
		apduData[4] = LeftLen; //le

		ret = HD_ApduT0(hUKey, apduData, srAPDULen, &respData[respDataLen], &LeftLen, 0);
		if ((ret != OPERATION_SUCCESS) && ((ret/0x100) != 0x61))
		{	
			sprintf(msg, "[UKEY]UKeyGetContainerData1 Error: %x\n", ret);
			DebugMessage(msg);
			return FALSE;
		
		}
		respDataLen += LeftLen;
	}
	if ((respDataLen % sizeof(CardContainer)) != 0) 
	{
		sprintf(msg, "[UKEY]UKeyGetContainerData respDataLen error: %d\n", respDataLen);
		DebugMessage(msg);
		return FALSE;
	}

	memcpy(Data, respData, respDataLen);
	*DataLength = respDataLen;
	*ContainerCount = respDataLen / sizeof(CardContainer);
	sprintf(msg, "[UKEY] UKeyGetContainerData out respDataLen=%d \n ", respDataLen);
	DebugMessage(msg);
	return TRUE;
}

BOOL WINAPI UKeyGetContainer(HANDLE hUKey,BYTE *pbContainer,BYTE *pstrCardCt)
{
	DebugMessage("[UKEY] UKeyGetContainer\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
  
	CardContainer *pContainer = (CardContainer *)respData;
	int i = 0;
	while (i < count) 
	{
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			memcpy(pstrCardCt, pContainer, sizeof(CardContainer));
			return TRUE;
		}
		pContainer++;
		i++;
	}
	return FALSE;
}

BOOL WINAPI UKeyGetCspContainerCount(HANDLE hUKey, WORD *wCount, BYTE *pContainers)
{
	DebugMessage("[UKEY] UKeyGetCspContainerCount\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	char EmptyData[64];
	for (int k=0; k<64; k++)
		EmptyData[k] = 0xFF;
	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
  
	CardContainer *pContainer = (CardContainer *)respData;
	
	int i = 0;
	int j = 0;
	while (i < count) 
	{
		if ((strlen((char *)pContainer->pbContName) != 0) &&
			(memcmp((char *)pContainer->pbContName, EmptyData, sizeof(EmptyData)) != 0))
		{
			if (pContainers != NULL)
				memcpy((BYTE *)(pContainers + j), pContainer, sizeof(CardContainer));
			j++;
		}
		pContainer++;
		i++;
	}
	*wCount = j;
	return TRUE;
}
BOOL WINAPI UKeyFindContainer(HANDLE hUKey,BYTE *pbContainer)
{
	DebugMessage("[UKEY] UKeyFindContainer\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
  
	CardContainer *pContainer = (CardContainer *)respData;
	int i = 0;
	while (i < count) 
	{
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			return TRUE;
		}
		pContainer++;
		i++;
	}
	return FALSE;
}

BOOL WINAPI UKeyDelContainer(HANDLE hUKey,BYTE *pbContainer)
{
	if ((pbContainer == NULL) || (strlen((char *)pbContainer) == 0))
		return true;
	DebugMessage("[UKEY] UKeyDelContainer\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	short srAPDULen = 5;
	BYTE CName[64] = {0};
	int CNameLen = 0;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));

	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
  
	CardContainer *pContainer = (CardContainer *)respData;
	unsigned short i = 0;
	while (i < count) 
	{
		DebugMessage("Container:->");
		DebugMessage((char *)pContainer->pbContName);
		DebugMessage("<:::>");
		DebugMessage((char *)pbContainer);
		DebugMessage("\n");	
		if (strcmp((char *)pContainer->pbContName, (char *)pbContainer) == 0)
		{
			
			memset(pContainer->pbContName, 0, sizeof(pContainer->pbContName));
			apduData[0] = 0x00; //cls
			apduData[1] = 0xDC; //ins
			apduData[2] = i + 1; //p1
			apduData[3] = 0xD4; //p2 标识符加P1
			apduData[4] = sizeof(CardContainer); //lc
			CardContainer c;
			memset(&c, 0, sizeof(CardContainer));
			memcpy(&apduData[5], &c, sizeof(CardContainer));
			srAPDULen = 5 + sizeof(CardContainer);
			apduData[srAPDULen] = 0;
			unsigned short nRet = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
			if (nRet == OPERATION_SUCCESS) 
			{
				DebugMessage("Container Deleted\n");
				ret = UKeySetContainerName(hUKey, CName, CNameLen);
				return ret;
			}
			DebugMessage("Container not Deleted\n");
			break;
		}
		i++;
		pContainer++;
	}
	
	DebugMessage("Not Found Container\n");
	return FALSE;
}

BOOL WINAPI UKeyAddContainer(HANDLE hUKey,BYTE *strCardCt)
{
	DebugMessage("[UKEY] UKeyAddContainer\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));

	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
	char EmptyData[64];
    for (int k=0; k<64; k++)
		EmptyData[k] = 0xFF;
	CardContainer *pContainer = (CardContainer *)respData;
	CardContainer *p = pContainer;
	CardContainer *pCardCt = (CardContainer *)strCardCt;
	int EmptyPos = 0;
	int i = 0;
	while (i < count) 
	{
		// 找到第一个为空的位置
		if ((strlen((char *)pContainer->pbContName) == 0) ||
			(strcmp((char *)pContainer->pbContName, EmptyData) == 0))
		{
			if (EmptyPos == 0)
				EmptyPos = i;
		}
		//如果有同名的，直接返回错误
		else if (strcmp((char *)pContainer->pbContName, (char *)pCardCt->pbContName) == 0)
		{
			char msg[100];
			sprintf(msg, "[UKEY]Container have exists:%s::<>::%s\n",pContainer->pbContName, pCardCt->pbContName);
			DebugMessage(msg);
			return FALSE;
		}

		i++;
		pContainer++;
	}

	apduData[0] = 0x00;     //cls
	apduData[1] = 0xDC;     //ins
	apduData[2] = EmptyPos + 1; //p1
	apduData[3] = 0xD4;     //p2 标识符加P1
	apduData[4] = sizeof(CardContainer); //lc
	memcpy(&apduData[5], strCardCt, sizeof(CardContainer));
	srAPDULen = 5 + sizeof(CardContainer);
	apduData[srAPDULen] = 0;
	unsigned short nRet = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (nRet == OPERATION_SUCCESS) 
	{
		ret = UKeySetContainerName(hUKey, pContainer->pbContName, strlen((char *)pContainer->pbContName));
		return ret;
	}

	return FALSE;
}

BOOL WINAPI UKeyUpdateContainer(HANDLE hUKey, BYTE *ContainerName, BYTE *strCardCt)
{
	DebugMessage("[UKEY] UKeyUpdateContainer\n");
	short respDataLen = 0;
	unsigned char respData[1000] = {0};
	short count = 0;
	short srAPDULen = 5;
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));

	BOOL ret = UKeyGetContainerData(hUKey, respData, &respDataLen, &count);
	if (!ret)
		return FALSE;
	CardContainer *pContainer = (CardContainer *)respData;
	CardContainer *p = pContainer;
	CardContainer *pCardCt = (CardContainer *)strCardCt;
	int i = 0;
	while (i < count) 
	{
		//
		if (strcmp((char *)pContainer->pbContName, (char *)ContainerName) == 0)
		{
			apduData[0] = 0x00;     //cls
			apduData[1] = 0xDC;     //ins
			apduData[2] = i + 1; //p1
			apduData[3] = 0xD4;     //p2 标识符加P1
			apduData[4] = sizeof(CardContainer); //lc
			memcpy(&apduData[5], strCardCt, sizeof(CardContainer));
			srAPDULen = 5 + sizeof(CardContainer);
			apduData[srAPDULen] = 0;
			unsigned short nRet = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
			if (nRet == OPERATION_SUCCESS) 
			{
				//ret = UKeySetContainerName(hUKey, pContainer->pbContName, strlen((char *)pContainer->pbContName));
				//return ret;
				return TRUE;
			}
		}

		i++;
		pContainer++;
	}

	return FALSE;
}

BOOL WINAPI UKeyGenKey(HANDLE hUKey, WORD wKeyPSfi, WORD wKeySSfi)
{
	DebugMessage("[UKEY] UKeyGenKey\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD0; //ins
	apduData[2] = 0x00 | wKeyPSfi; //p1 保存公钥文件 文件名 0F 
	apduData[3] = 0x00 | wKeySSfi; //p2 保存私钥文件 文件名 0E
	apduData[4] = 0x00; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;

	return TRUE;	
}
BOOL WINAPI UKeyGetKey(HANDLE hUKey, DWORD sfi, unsigned char KeyFlag, unsigned char *Key)
{
	DebugMessage("[UKEY] UKeyGetKey\n");
	//AfxMessageBox("in");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD8; //ins
	apduData[2] = 0x40 | sfi; //p1 根据sfi读取公钥 
	apduData[3] = 0x02; //p2 读取模数
	apduData[4] = 0x80; //le

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	//CString ss;
	//ss.Format("UKeyGetKey return %x", ret);
	//AfxMessageBox(ss);
	char msg[100];
	if (ret != OPERATION_SUCCESS) 
	{
		sprintf(msg, "UKeyGetKey Modul ERROR From Card: %x", ret);
		DebugMessage(msg);
		return FALSE;
	}

	if (KeyFlag == PUBLIC_KEY_FLAG)
	{
		R_RSA_PUBLIC_KEY *p = (R_RSA_PUBLIC_KEY *)Key;
		p->bits = 1024;                           /* length in bits of modulus */
		memcpy(p->modulus, respData, respDataLen);                    /* modulus */
		apduData[3] = 0x01; //p2 读取指数
		apduData[4] = 0x04; //le
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS) 
		{
			sprintf(msg, "UKeyGetKey Exp ERROR From Card: %x", ret);
			DebugMessage(msg);
			return FALSE;
		}
		memcpy(p->exponent, respData, respDataLen);   
	}
	return TRUE;	
}

BOOL WINAPI UKeyRSAEncrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	DebugMessage("[UKEY] UKeyRSAEncrypt\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF6; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取私钥 
	apduData[3] = 0x00; //p2
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, *InDataLen);
	apduData[*InDataLen + 5] = 0x80;
	apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = *InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret != 0x6180) && (ret != OPERATION_SUCCESS))
		return FALSE;
	if (0x6180 == ret)
	{
		// 读取后续未读出的字节
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; //p1
		apduData[3] = 0x00; //p2 
		apduData[4] = 0x80;  //le
		apduData[5] = 0;
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS)
			return FALSE;
	}
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return TRUE;	
}

BOOL WINAPI UKeyRSADecrypt(HANDLE hUKey, DWORD sfi, unsigned char *InData, int *InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	DebugMessage("[UKEY] UKeyRSADecrypt\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF8; //ins
	apduData[2] = 0x80 | sfi; //p1 根据sfi读取私钥 
	apduData[3] = 0x00; //p2 读取莫属
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, *InDataLen);
	apduData[*InDataLen + 5] = 0x80;
	apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = *InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];
    char msg[100] = {0};
	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if ((ret != 0x6180) && (ret != OPERATION_SUCCESS))
	{
		sprintf(msg, "UKeyRSADecrypt Failed 1, ret=%x\n", ret);
		DebugMessage(msg);
		return FALSE;
	}
	if (0x6180 == ret)
	{
		// 读取后续未读出的字节
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; //p1
		apduData[3] = 0x00; //p2 
		apduData[4] = 0x80;  //le
		apduData[5] = 0;
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		if (ret != OPERATION_SUCCESS)
		{
			sprintf(msg, "UKeyRSADecrypt Failed 2, ret=%x\n", ret);
			DebugMessage(msg);
			return FALSE;
		}
	}
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return TRUE;	
}
BOOL WINAPI UKeyRSASignature(HANDLE hUKey, DWORD sfi, unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	DebugMessage("[UKEY] UKeyRSASignature1\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF2; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取公钥 
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, InDataLen);
	apduData[InDataLen + 5] = 0x80;
	apduData[InDataLen + 6] = 0x00;
	short srAPDULen = InDataLen + 6;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret == 0x6180)
	{
		memset(apduData, 0, sizeof(apduData));
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; // p1 
		apduData[3] = 0x00; // p2 
		apduData[4] = 0x80; //le
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	}
	if (ret != OPERATION_SUCCESS) 
	{
		char msg[100] = {0};
		sprintf(msg, "[UKey]UKeyRSASignature Error:%x, sfi=%x, InDataLen=%d\n", ret, sfi, srAPDULen);
		DebugMessage(msg);
		return FALSE;
	}
	else
	{
		char msg[100] = {0};
		sprintf(msg, "[UKey]UKeyRSASignature success:%x, sfi=%x, respDataLen=%d\n", ret, sfi, respDataLen);
		DebugMessage(msg);
	}
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return TRUE;	
}

BOOL WINAPI UKeyRSASignatureVerify(HANDLE hUKey, DWORD sfi, const unsigned char *InData, int InDataLen, 
					unsigned char *OutData, int *OutDataLen)
{
	DebugMessage("[UKEY] UKeyRSASignatureVerify\n");
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xF4; //ins
	apduData[2] = 0x80 | sfi; // p1 根据sfi读取公钥 
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x80; //le

	memcpy(&apduData[5], InData, InDataLen);
	apduData[InDataLen + 5] = 0x00;
	//apduData[*InDataLen + 6] = 0x00;
	short srAPDULen = InDataLen + 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret == 0x6180)
	{
		memset(apduData, 0, sizeof(apduData));
		apduData[0] = 0x00; //cls
		apduData[1] = 0xC0; //ins
		apduData[2] = 0x00; // p1 
		apduData[3] = 0x00; // p2 
		apduData[4] = 0x80; //le
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	}
	if (ret != OPERATION_SUCCESS) 
		return FALSE;
	memset(OutData, 0, *OutDataLen);
	memcpy(OutData, respData, respDataLen);
	*OutDataLen = respDataLen;

	return TRUE;	
}

BOOL WINAPI UKeyGetNameFromCN(char *cn,char *name)
{
	WORD wNamePos,wEmailPos;
	for(wNamePos=0; wNamePos<strlen(cn)-strlen("/CN="); wNamePos++)
	{
		if(!memcmp(cn+wNamePos,"/CN=",strlen("/CN=")))
		{
			wNamePos += strlen("/CN=");
			break;
		}
	}
	DebugMessage(cn);
	if(wNamePos == strlen(cn))
		return FALSE;

	for(wEmailPos=wNamePos; wEmailPos<strlen(cn); wEmailPos++)
	{
		if(!memcmp(cn+wEmailPos,"/",strlen("/")))
			break;
	}
	wEmailPos++;
	if (memcmp(cn + wEmailPos, "CN", 2) == 0)
		memcpy(name, cn + wEmailPos + 3, (strlen(cn) - wEmailPos - 1));
	else
		memcpy(name, cn+wNamePos, (wEmailPos - wNamePos));
	return TRUE;
}

BOOL WINAPI UKeyGetCertName(BYTE *pbData, DWORD dwLen, char *pcOutName)
{
	BYTE  *p;
	X509 *x=NULL;
	X509_NAME *name=NULL;
	char cn[512];
	p = pbData;
	x = d2i_X509(NULL, (const unsigned char **)&p, dwLen);
	
	name = X509_get_subject_name(x);
	memset(cn, 0, sizeof(cn));
	X509_NAME_oneline(name, cn, sizeof(cn));
	//此时cn中就是该证书持有人对应的CN;
	X509_free(x);

	if(!UKeyGetNameFromCN(cn,pcOutName))
		return FALSE;
	return TRUE;
}

BOOL WINAPI UKeyAppendCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen)
{
	DebugMessage("[UKEY] UKeyAppendCert\n");
	char cName[128];
	WORD wCertAddr;
	memset(cName,0,sizeof(cName));
	CardContainer *PContainer = (CardContainer *)pstrCardCt;
	memcpy(cName, PContainer->pbCertName, sizeof(PContainer->pbCertName));
	wCertAddr = PContainer->wCertAddr;
	PContainer->wCertSize = dwLen;
	if (!SelectADF(hUKey))
	{
		DebugMessage("[UKEY] UKeyAppendCert SelectADF ERROR\n");
		return FALSE;
	}
	if(!UKeyGetCertName(pbData, dwLen, cName))//证书格式不对
		return FALSE;
	unsigned char apduData[300];// = ;
	short srAPDULen = 0;
	short respDataLen = 0;
	unsigned char respData[255];
	unsigned short ret = 0;
	unsigned char WriteOnce = 0x80;
	WORD dataAddr = 0;
	//选择文件
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; //cls
	apduData[1] = 0xA4; //ins
	apduData[2] = 0x00; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x02; // wCertAddr
	apduData[5] = HIBYTE(wCertAddr);
	apduData[6] = LOBYTE(wCertAddr);
	apduData[7] = 0;
	srAPDULen = 7;
	ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
			return FALSE;
	//操作二进制文件
	apduData[0] = 0x00; //cls
	apduData[1] = 0xD6; //ins
	//apduData[2] = 0x00; // p1 根据sfi更新
	//apduData[3] = 0x00; // p2 从文件头开始
	int count = dwLen / WriteOnce;
	int i=0;
	//如果长度大于一次可写入的长度，分段写入
	for (i=0; i<count; i++)
	{
		dataAddr = i * WriteOnce;
		apduData[2] = HIBYTE(dataAddr);
		apduData[3] = LOBYTE(dataAddr);
		apduData[4] = WriteOnce;  //lc
		memcpy(&apduData[5], (BYTE *)(pbData + i * WriteOnce), WriteOnce);
		apduData[WriteOnce + 5] = 0x00;
		srAPDULen = 5 + WriteOnce;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);

		if (ret != OPERATION_SUCCESS) 
			return FALSE;
	}
	count = dwLen % WriteOnce;
	if (count > 0)
	{
		dataAddr = i * WriteOnce;
		apduData[2] = HIBYTE(dataAddr);
		apduData[3] = LOBYTE(dataAddr);
		apduData[4] = count; //lc
		memcpy(&apduData[5], (BYTE *)(pbData + i * WriteOnce), count);
		apduData[count + 5] = 0x00;
		srAPDULen = 5 + count;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	
		if (ret != OPERATION_SUCCESS) 
			return FALSE;
	}
    ret = UKeySetContainerName(hUKey, (BYTE *)cName, strlen(cName));
	if (ret)
		strcpy((char *)PContainer->pbCertName, cName);
	char msg[100];
	sprintf(msg, "[UKEY]CertName: %s\n", cName);
	DebugMessage(msg);
	return ret;
}

BOOL WINAPI UKeyGetCert(HANDLE hUKey, BYTE *pstrCardCt, BYTE *pbData, DWORD dwLen)
{
	DebugMessage("[UKEY] UKeyGetCert in\n");

	WORD wCertAddr;
	CardContainer *PContainer = (CardContainer *)pstrCardCt;
	wCertAddr = PContainer->wCertAddr;
	//PContainer->wCertSize = dwLen;
	unsigned char ReadOnce = 0x80;
	unsigned char apduData[255];// = ;
	short srAPDULen = 0;
	short respDataLen = 0;
	unsigned char respData[255];
	unsigned short ret = 0;
	WORD dataAddr = 0;
	char msg[100] = {0};
	memset(apduData, 0, sizeof(apduData));
	if (!SelectADF(hUKey))
	{
		DebugMessage("[UKEY] UKeyGetCert SelectADF ERROR\n");
		return FALSE;
	}
	apduData[0] = 0x00; //cls
	apduData[1] = 0xA4; //ins
	apduData[2] = 0x00; // p1 根据sfi更新
	apduData[3] = 0x00; // p2 从文件头开始
	apduData[4] = 0x02; // wCertAddr
	apduData[5] = HIBYTE(wCertAddr);
	apduData[6] = LOBYTE(wCertAddr);
	apduData[7] = 0;
	srAPDULen = 7;
	ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
	{
		sprintf(msg, "UKeyGetCert Select File Error: %x \n", ret);
		DebugMessage(msg);
		return FALSE;
	}
	DebugMessage("[UKEY] UKeyGetCert Select File OK!\n");
	apduData[0] = 0x00; //cls
	apduData[1] = 0xB0; //ins
	int count = dwLen / ReadOnce;
	int leftlen = 0;
	if ((dwLen % ReadOnce) != 0)
	{	
		leftlen = dwLen - (count * ReadOnce);
		count++;
	}
	int i=0;
	//如果长度大于一次可写入的长度，分段写入
	int templen = ReadOnce;
	sprintf(msg, "UKeyGetCert count=%d,dwLen=%d: %x \n", count, dwLen, ret);
	DebugMessage(msg);
	for (i=0; i<count; i++)
	{
		dataAddr = ReadOnce * i;
		apduData[2] = HIBYTE(dataAddr);
		apduData[3] = LOBYTE(dataAddr);
		if ((count - 1) == i)
			apduData[4] = leftlen;
		else
			apduData[4] = templen;  //le
		apduData[5] = 0x00;
		srAPDULen = 5;
		ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
		
		sprintf(msg, "UKeyGetCert Read complete return: %x \n", ret);
		DebugMessage(msg);
		if (((ret / 0x100) != 61) && (ret != OPERATION_SUCCESS)) 
		{
			
			return FALSE;
		}
		memcpy((BYTE *)(pbData + i * ReadOnce), respData, respDataLen);
		
		templen = ret % 0x100;
		if (0 == templen)
			templen = ReadOnce;
	}
 	//if (ret == OPERATION_SUCCESS) 
	//	break;
	DebugMessage("[UKEY] UKeyGetCert out\n");
	return ret == OPERATION_SUCCESS;
}

BOOL WINAPI UKeyImportKey(HANDLE hUKey, DWORD sfi, unsigned char *Key)
{
	unsigned char apduData[255];// = ;
	short respDataLen = 0;
	unsigned char respData[255];

	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x80; //cls
	apduData[1] = 0xD2; //ins
	apduData[2] = 0x40 | sfi; //p1 根据sfi读取公钥 
	apduData[3] = 0x01; //p2 指数
	apduData[4] = 0x04; //le

	R_RSA_PUBLIC_KEY *p = (R_RSA_PUBLIC_KEY *)Key;
	memcpy(&apduData[5], p->exponent, 4);
	short srAPDULen = 5 + 4;
	apduData[srAPDULen] = 0;

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;

	apduData[3] = 0x02; //p2 模
	apduData[4] = 0x80; //le
	memcpy(&apduData[5], p->modulus, p->bits / 8);
	srAPDULen = 5 + (p->bits / 8);
	apduData[srAPDULen] = 0;
	
	ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;

	return TRUE;
}

BOOL WINAPI UKeyUnLock(HANDLE hUKey, BYTE *NewPin, int NewPinLen)
{
	return TRUE;
}

BOOL WINAPI UKeyGetInfo(HANDLE hUKey,BYTE *Info, int *InfoLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0xBF; // cls
	apduData[1] = 0xC8; // ins
	apduData[2] = 0x00; // p1
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x0F; // le
	apduData[5] = 0x00;

	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];

	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;
	*InfoLen = respDataLen;
	memcpy(Info, respData, respDataLen);
	return TRUE;	
}

BOOL WINAPI UKeyGetContainerName(HANDLE hUKey,BYTE *CName, int *CNameLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; // cls
	apduData[1] = 0xB0; // ins
	apduData[2] = 0x93; // p1
	apduData[3] = 0x00; // p2 
	apduData[4] = 0x4E; // le
	apduData[5] = 0x00;
	
	short srAPDULen = 5;
	short respDataLen = 0;
	unsigned char respData[255];
	if (!SelectADF(hUKey))
	{
		DebugMessage("[UKEY] UKeyGetContainerName SelectADF ERROR\n");
		return FALSE;
	}
	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;
	*CNameLen = respData[0];
	if (*CNameLen > 0)
		memcpy(CName, &respData[1], respDataLen);
	else
		CName[0] = 0;
	return TRUE;	
}
BOOL WINAPI UKeySetContainerName(HANDLE hUKey,BYTE *CName, int CNameLen)
{
	unsigned char apduData[255];// = ;
	memset(apduData, 0, sizeof(apduData));
	apduData[0] = 0x00; // cls
	apduData[1] = 0xD6; // ins
	apduData[2] = 0x93; // p1
	apduData[3] = 0x00; // p2 
	if (CNameLen > 0)
	{
		apduData[4] = CNameLen + 1; // lc
		apduData[5] = CNameLen;
		memcpy(&apduData[6], CName, CNameLen);
		apduData[6 + CNameLen] = 0x00;
		char msg[100];
		sprintf(msg, "[UKEY]CertName: %s, NameLen: %d\n", CName, CNameLen);
		DebugMessage(msg);
	}
	else
	{
		DebugMessage("[UKEY]CertName Deleted\n");
		CNameLen = 64;
		apduData[4] = CNameLen + 1; // lc
		apduData[5] = 0;
		for (int i=0; i<CNameLen; i++)
			apduData[6 + i] = 0;
		apduData[6 + CNameLen] = 0x00;
	}
	
	short srAPDULen = 6 + CNameLen;
	short respDataLen = 0;
	unsigned char respData[255];
	if (!SelectADF(hUKey))
	{
		DebugMessage("[UKEY] UKeySetContainerName SelectADF ERROR\n");
		return FALSE;
	}
	unsigned short ret = HD_ApduT0(hUKey, apduData, srAPDULen, respData, &respDataLen, 0);
	if (ret != OPERATION_SUCCESS) 
		return FALSE;
	return TRUE;	
}