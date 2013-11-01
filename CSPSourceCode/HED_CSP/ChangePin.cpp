// ChangePin.cpp : implementation file
//

#include "stdafx.h"
#include "HED_CSP.h"
#include "ChangePin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePin dialog


CChangePin::CChangePin(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePin)
	m_NewPin1 = _T("");
	m_NewPin2 = _T("");
	m_OldPin = _T("");
	//}}AFX_DATA_INIT
}


void CChangePin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePin)
	DDX_Text(pDX, IDC_EDITNEWPIN1, m_NewPin1);
	DDX_Text(pDX, IDC_EDITNEWPIN2, m_NewPin2);
	DDX_Text(pDX, IDC_EDITOLDPIN, m_OldPin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePin, CDialog)
	//{{AFX_MSG_MAP(CChangePin)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePin message handlers
