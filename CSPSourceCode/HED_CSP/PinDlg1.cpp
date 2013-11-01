// PinDlg1.cpp : implementation file
//

#include "stdafx.h"
#include "HED_CSP.h"
#include "PinDlg1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPinDlg dialog


CPinDlg::CPinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPinDlg)
	m_sPassword = _T("");

	//}}AFX_DATA_INIT
}


void CPinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPinDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDV_MaxChars(pDX, m_sPassword, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPinDlg, CDialog)
	//{{AFX_MSG_MAP(CPinDlg)
	ON_BN_CLICKED(IDOK2, OnOk2)
	ON_BN_CLICKED(IDCANCEL2, OnCancel2)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPinDlg message handlers

void CPinDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CPinDlg::OnOk2() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CPinDlg::OnCancel2() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CPinDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	CEdit *e = (CEdit *)GetDlgItem(IDC_PASSWORD);
	e->SetFocus();
	// TODO: Add your message handler code here
	
}
