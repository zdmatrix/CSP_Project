//*************************************************************************
// ���̣�ģ̬�Ի�����ģ��
// �ļ���Dialog.c
//*************************************************************************


//*************************************************************************
//* ����ڴ�й¶��
#define	_CRTDBG_MAP_ALLOC

#include <crtdbg.h>


//*************************************************************************
// ������Ҫ��ͷ�ļ�



#include "Global.h"

#include <windowsx.h>
#include <commctrl.h>
//#include <stdlib.h>
#include "resource.h"

//*************************************************************************
// �궨��


//*************************************************************************
// ���ݽṹ����


//*************************************************************************
// �ڲ�ȫ�ֱ���
const static CHAR gsTitle[] = "HED_CSP Test Tools";

static LPSTR gTabNames[] = { "HED_CSP���ò���", 0 };
static LPSTR gDlgNames[] = { 
							 MAKEINTRESOURCE(IDD_DIALOG_PAGE2),
							 
							 0 };


//*************************************************************************
// �˴���ģ���а����ĺ�������ǰ����
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM  lParam );


//*************************************************************************
// ����: Dialog_OnInitDialog
// ����: ��ʼ��Ӧ������ģ��
//*************************************************************************
LRESULT Dialog_OnInitDialog ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPVOID		lpVoid = NULL;

	if ( lpUserData == NULL )
		return TRUE;

	// ��ʼ���Ի����������
	lpUserData->hFont = GetWindowFont ( hWnd );
	
	// ����APDU Script�ؼ�
	lpUserData->hComboNewDev = GetDlgItem ( hWnd, IDC_COMBO_RUNDEV );	// ȡ�������豸�б����

	// ����ģ������
	GNODEOP.Create ( &lpUserData->lpModNode );


	// ��ʼ��Tab�ؼ�
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
// ����: Dialog_OnClose ()
// ����: �Ի��򴰿��˳�����
//*************************************************************************
static void Dialog_OnClose ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPUSERDATA	lpUserData = (LPUSERDATA)GetWindowLong ( hWnd, GWL_USERDATA );
	LPDULNODE	lpDuLNode = NULL;

	
	// �ͷ�TAB�ռ����
	TabControl_Destroy ( &lpUserData->sTabCtrl );

	// �ͷ�ģ������
	GNODEOP.Destroy ( &lpUserData->lpModNode, NULL );

	DestroyWindow ( hWnd );
	return;
}

//*************************************************************************
// ����: Dialog_OnNotify ()
// ����: Tab���ڱ�ǩ�ĵ��֪ͨ
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
			lpUserData->sTabCtrl.OnSelChanged();	// ԭʼ���
			lpUserData->nTabSelect = SendMessage ( lpUserData->sTabCtrl.hTab, TCM_GETCURSEL, 0, 0 );
			
		}
	}


	return FALSE;
}



//*************************************************************************
// ����: Dialog_OnPaint
// ����: ����OnPaint��Ϣ
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
// ����: Dialog_OnCommand
// ����: ����OnCommand��Ϣ
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
// ����: Dialog_WndProc ()
// ����: �Ի��򴰿�����Ϣѭ��
//*************************************************************************
LRESULT Dialog_WndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:			// ��ʼ��Ӧ������ģ��
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

	return FALSE;		// ����FALSE��ȱʡ�Ի�����DefDlgProc(),��ʾû�д�����Ϣ
}

//*************************************************************************
// ����: Dialog_Init ()
// ����: ��ʼ���Ի���
//*************************************************************************
HWND Dialog_Init ( HINSTANCE	hInstance,		// handle to current instance (��ǰʵ�����)
				   HINSTANCE	hPrevInstance,	// handle to previous instance (ǰһ��ʵ�����)
				   LPSTR		lpCmdLine,		// command line (�������ַ�)
				   int			nCmdShow )		// show state (������ʾ��ʽ)
{
	static USERDATA	UserData = { 0 };	// Ӧ������
	HMODULE			hReturn;

	// ��ʼ������
	UserData.hInstance = hInstance;		// �洢ʵ�����

	// ִ�пؼ����躯��
	InitCommonControls ();						// Tab�ؼ�����
	hReturn = LoadLibrary( "riched32.dll");		// RichEdit�ؼ�������ô���
	if ( hReturn == NULL )
		return NULL;

	// �����Ի���
	UserData.hMainDlg = CreateDialog ( hInstance, (LPCTSTR)IDD_DIALOG, NULL, (DLGPROC)Dialog_WndProc );
	if ( UserData.hMainDlg == NULL )
		return NULL;

	// ���÷�ģ̬�Ի������
	SendMessage ( UserData.hMainDlg, WM_SETTEXT, 0, (LPARAM)gsTitle );

	// ���û�����ָ�븳��Ի�����
	SetWindowLong ( UserData.hMainDlg, GWL_USERDATA, (LONG)&UserData );

	// WM_INITDIALOG��һ���ǻ�û�л���û�����ָ�룬�����ٴη���
	SendMessage ( UserData.hMainDlg, WM_INITDIALOG, 0, 0 );

	return UserData.hMainDlg;
}


//***************************************************************************
// END
//***************************************************************************
