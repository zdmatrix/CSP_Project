//*************************************************************************
// 文件：AppCash.c
// 内容：演示现金余额
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"



//*************************************************************************
// 宏定义


//*************************************************************************
// 全局变量


//*************************************************************************
// 此代码模块中包含的函数的向前声明


//*************************************************************************
//*************************************************************************


//*************************************************************************
// 函数: AppCash_OnCommand ()
// 功能: 按键处理
//*************************************************************************
LRESULT AppCash_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;
	WORD		wNotifyCode, wID;

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
