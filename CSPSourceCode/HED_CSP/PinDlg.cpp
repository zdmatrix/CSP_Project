// PinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HED_CSP.h"
#include "PinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PinDlg dialog


PinDlg::PinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PinDlg)
	m_sPassword = _T("");
	//}}AFX_DATA_INIT
}


void PinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PinDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDV_MaxChars(pDX, m_sPassword, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PinDlg, CDialog)
	//{{AFX_MSG_MAP(PinDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PinDlg message handlers
