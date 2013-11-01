#if !defined(AFX_PINDLG1_H__605BD9F6_B4D8_4C72_8C7B_F9CC621E82FD__INCLUDED_)
#define AFX_PINDLG1_H__605BD9F6_B4D8_4C72_8C7B_F9CC621E82FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PinDlg1.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPinDlg dialog

class CPinDlg : public CDialog
{
// Construction
public:
	CPinDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPinDlg)
	enum { IDD = IDD_DIALOG1 };
	CString	m_sPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPinDlg)
	virtual void OnOK();
	afx_msg void OnOk2();
	afx_msg void OnCancel2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINDLG1_H__605BD9F6_B4D8_4C72_8C7B_F9CC621E82FD__INCLUDED_)
