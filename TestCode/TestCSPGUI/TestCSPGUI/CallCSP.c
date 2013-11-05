//*************************************************************************
// 文件：CallCSP.c
// 内容：调用CSP中自定义的函数实现在卡上显示数字的功能
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC

#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件



#include "Global.h"

#include "resource.h"

#include "cspdk.h"





//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量


//*************************************************************************
// 此代码模块中包含的函数的向前声明


//*************************************************************************
//*************************************************************************


//*************************************************************************
// 函数: CallCSP ()
// 功能: 按键处理
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

	// 确保私有数据的正确性
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	
	// OnCommand消息的细节
	wNotifyCode	= HIWORD(wParam);	// notification code
	wID			= LOWORD(wParam);	// item, control, or accelerator identifier

	switch ( wNotifyCode )
	{
	case BN_CLICKED:
		// 按键点击消息
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
