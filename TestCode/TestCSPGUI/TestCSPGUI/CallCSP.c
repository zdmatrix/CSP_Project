//*************************************************************************
// �ļ���CallCSP.c
// ���ݣ�����CSP���Զ���ĺ���ʵ���ڿ�����ʾ���ֵĹ���
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC

#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�



#include "Global.h"

#include "resource.h"

#include "cspdk.h"





//*************************************************************************
// �궨��


//*************************************************************************
// ȫ�ֱ���


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����


//*************************************************************************
//*************************************************************************


//*************************************************************************
// ����: CallCSP ()
// ����: ��������
//*************************************************************************
LRESULT CallCSP ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

	HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    DWORD dwParam;
    BYTE *pbData;
    DWORD dwFlags;

	extern DWORD	index;

	hProv = (HCRYPTPROV)malloc(sizeof(HCRYPTPROV));
	hKey = (HCRYPTKEY)malloc(sizeof(HCRYPTKEY));
	dwParam = KP_SALT;
	pbData = (BYTE*)malloc(sizeof(BYTE));

	// ȷ��˽�����ݵ���ȷ��
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	
	// OnCommand��Ϣ��ϸ��
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier

	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// ���������Ϣ
		if ( wID == 0x0411 )
		{
			index ++;
			dwFlags = 0x30 + (index % 10);

			CPSetKeyParam(hProv, hKey, dwParam, pbData, dwFlags);
			free(hProv);
			free(hKey);
			free(pbData);
		}		
		else
			return -1;
		break;

	default:
		return -1;
	}

	return TRUE;
}



//***************************************************************************
// END
//***************************************************************************
