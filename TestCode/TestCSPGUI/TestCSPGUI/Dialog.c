//*************************************************************************
// 工程：模态对话框处理模块
// 文件：Dialog.c
//*************************************************************************


//*************************************************************************
//* 检测内存泄露用
#define	_CRTDBG_MAP_ALLOC

#include <crtdbg.h>


//*************************************************************************
// 包含必要的头文件



#include "Global.h"

#include <windowsx.h>
#include <commctrl.h>
//#include <stdlib.h>
#include "resource.h"

//*************************************************************************
// 宏定义


//*************************************************************************
// 数据结构定义


//*************************************************************************
// 内部全局变量
const static CHAR gsTitle[] = "HED_CSP Test Tools";

static LPSTR gTabNames[] = { "HED_CSP调用测试", 0 };
static LPSTR gDlgNames[] = { 
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE2),
							 
							 0 };


//*************************************************************************
// 此代码模块中包含的函数的向前声明
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );


//*************************************************************************
// 函数: Dialog_OnInitDialog
// 功能: 初始化应用所需模块
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPVOID		lpVoid = NULL;

	if ( lpUserData == NULL )
		return TRUE;

	// 初始化对话框相关数据
	lpUserData->hFont = GetWindowFont ( hWnd );
	
	// 设置APDU Script控件
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// 取已运行设备列表框句柄

	// 创建模块链表
	GNODEOP.Create ( &lpUserData->lpModNode );


	// 初始化Tab控件
	New_TabControl ( &lpUserData->sTabCtrl,			// address of TabControl struct
					 GetDlgItem ( hWnd, IDC_TAB ),	// handle to tab control
					 gTabNames,						// text for each tab
					 gDlgNames,						// dialog id's of each tab page dialog
					 &Dialog_WndProc,				// address of main windows proc
					 NULL,							// address of size function
					 TRUE );						// stretch tab page to fit tab ctrl

	return 0;
}

//*************************************************************************
// 函数: Dialog_OnClose ()
// 功能: 对话框窗口退出处理
//*************************************************************************
static void Dialog_OnClose ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;

	
	// 释放TAB空间相关
	TabControl_Destroy ( &lpUserData->sTabCtrl );

	// 释放模块链表
	GNODEOP.Destroy ( &lpUserData->lpModNode, NULL );

	DestroyWindow ( hWnd );
	return;
}

//*************************************************************************
// 函数: Dialog_OnNotify ()
// 功能: Tab窗口标签的点击通知
//*************************************************************************
static BOOL Dialog_OnNotify ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	int			id = (int)wParam;
	LPNMHDR		nm = (LPNMHDR)lParam;
	LPDULNODE	lpDuLNode = NULL;

	if ( id == IDC_TAB )
	{
		TabControl_Select ( &lpUserData->sTabCtrl );	// update internal "this" pointer

		if ( nm->code == TCN_SELCHANGE )
		{
			lpUserData->sTabCtrl.OnSelChanged();	// 原始语句
			lpUserData->nTabSelect = SendMessage ( lpUserData->sTabCtrl.hTab, TCM_GETCURSEL, 0, 0 );
			
		}
	}


	return FALSE;
}



//*************************************************************************
// 函数: Dialog_OnPaint
// 功能: 处理OnPaint消息
//*************************************************************************
LRESULT Dialog_OnPaint ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	
	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	return FALSE;
}

//*************************************************************************
// 函数: Dialog_OnCommand
// 功能: 处理OnCommand消息
//*************************************************************************
LRESULT Dialog_OnCommand ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );

	if ( !lpUserData )
	{
		lpUserData = (LPUSERDATA)GetWindowLong ( GetParent ( hWnd ), GWL_USERDATA );
		if ( !lpUserData ) return -1;
	}

	if ( lpUserData->nTabSelect == 0 )
		CallCSP ( hWnd, uMsg, wParam, lParam );
	

	return 0;
}

//*************************************************************************
// 函数: Dialog_WndProc ()
// 功能: 对话框窗口主消息循环
//*************************************************************************
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:			// 初始化应用所需模块
		Dialog_OnInitDialog ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_NOTIFY:
		Dialog_OnNotify ( hWnd, uMsg, wParam, lParam );
		return FALSE;


	case WM_COMMAND:
		Dialog_OnCommand ( hWnd, uMsg, wParam, lParam );
		return TRUE;
		
	case WM_PAINT:
		return Dialog_OnPaint ( hWnd, uMsg, wParam, lParam );

	case WM_CLOSE:
		Dialog_OnClose ( hWnd, uMsg, wParam, lParam );
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage ( 0 );
		return TRUE;

	default:
		break;
	}

	return FALSE;		// 返回FALSE给缺省对话框函数DefDlgProc(),表示没有处理本消息
}

//*************************************************************************
// 函数: Dialog_Init ()
// 功能: 初始化对话框
//*************************************************************************
HWND Dialog_Init ( HINSTANCE	hInstance,		// handle to current instance (当前实例句柄)
				   HINSTANCE	hPrevInstance,	// handle to previous instance (前一个实例句柄)
				   LPSTR		lpCmdLine,		// command line (命令行字符)
				   int			nCmdShow )		// show state (窗口显示方式)
{
	static USERDATA	UserData = { 0 };	// 应用数据
	HMODULE			hReturn;

	// 初始化数据
	UserData.hInstance = hInstance;		// 存储实例句柄

	// 执行控件所需函数
	InitCommonControls ();						// Tab控件所需
	hReturn = LoadLibrary( "riched32.dll");		// RichEdit控件必须调用此行
	if ( hReturn == NULL )
		return NULL;

	// 创建对话框
	UserData.hMainDlg = CreateDialog ( hInstance, (LPCTSTR)IDD_DIALOG, NULL, (DLGPROC)Dialog_WndProc );
	if ( UserData.hMainDlg == NULL )
		return NULL;

	// 设置非模态对话框标题
	SendMessage ( UserData.hMainDlg, WM_SETTEXT, 0, (LPARAM)gsTitle );

	// 将用户数据指针赋予对话框句柄
	SetWindowLong ( UserData.hMainDlg, GWL_USERDATA, (LONG)&UserData );

	// WM_INITDIALOG第一次是还没有获得用户数据指针，所以再次发送
	SendMessage ( UserData.hMainDlg, WM_INITDIALOG, 0, 0 );

	return UserData.hMainDlg;
}


//***************************************************************************
// END
//***************************************************************************
