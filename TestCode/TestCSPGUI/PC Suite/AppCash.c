//*************************************************************************
// �ļ���AppCash.c
// ���ݣ���ʾ�ֽ����
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"



//*************************************************************************
// �궨��


//*************************************************************************
// ȫ�ֱ���


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����


//*************************************************************************
//*************************************************************************


//*************************************************************************
// ����: AppCash_OnCommand ()
// ����: ��������
//*************************************************************************
LRESULT AppCash_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

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
		if ( wID == IDC_BUTTON_ADDHUNDRED )
		{
			;	
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
