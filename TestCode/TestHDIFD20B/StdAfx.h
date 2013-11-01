// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2AC6C813_065D_4D9C_8952_E1C76E491DF4__INCLUDED_)
#define AFX_STDAFX_H__2AC6C813_065D_4D9C_8952_E1C76E491DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>

#ifndef   _WIN32_WINNT 
#define   _WIN32_WINNT   0x0400 
#endif

#include <windows.h>
#include <wincrypt.h>

#include <malloc.h>
#include <stdlib.h>

#define MAX_BUF_LEN     512
#define MAX_APDU_RET_DATA 256
#define MAX_APDU_ISO_T0   262
#define MIN_RET_DATA_LEN  4

/********define by hp 070801 in function(GetReaderList)***********
#define NUMBER_OF_READERS	4
#define NAME_LENGTH			100
#define MAX_RESPONSE		2000
************************************************/

#define EXCUTE_SUC		0x9000

#define CER_RANGE       0xff0c  //The parameters range is invalid 
#define CER_NOCARD		0xff0f
#define CER_APDU	    0xff10   //The APDU data format error 
#define CER_HEXSTR		0xff30	//�Ƿ�ʮ�����ƴ����С�0������9������a������f������A������F��������ַ������ַ���������ż��
#define CER_RESULT		0xff38	//������
#define CER_UNKNOWN     0xffff	//�޴�����Ϣ����	

#define CER_PCSC_SCardReleaseContext		0xff60
#define CER_PCSC_SCardEstablishContext		0xff61
#define CER_PCSC_SCardListReaders           0xff62
#define CER_PCSC_SCardListReaders_NONE      0xff63     //No pcsc reader found
#define CER_PCSC_SCardConnect               0xff64     //
#define CER_PCSC_ProtocolType               0xff65     //Not T0, T1
#define CER_PCSC_SCardStatus                0xff66     //Get ATR error
#define CER_PCSC_SCardReconnect             0xff67     //Reset Card Fail
#define CER_PCSC_SCardDisconnect            0xff68     //Power off fail

#define CER_PCSC_SCardTransmit              0xff69     //SCardTransmit failed.

#define CER_PCSC_SCardTransmit_Data_LOST    0xff6A     //case 2 data less than 2

#define CER_PCSC_SCardGetAttrib             0xff70     
#define CER_PCSC_SCardListCards             0xff71     
#define CER_PCSC_SCardCancel	            0xff72    
#define CER_PCSC_SCardBeginTransaction      0xff73    
#define CER_PCSC_SCardEndTransaction        0xff74    
#define CER_PCSC_SCardGetProviderId         0xff75    
#define CER_PCSC_SCardGetStatusChange         0xff76 


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2AC6C813_065D_4D9C_8952_E1C76E491DF4__INCLUDED_)
