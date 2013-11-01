#if !defined(AFX_CHANGEPIN_H__183CC40F_FFFC_4C35_A292_1E6D17D15917__INCLUDED_)
#define AFX_CHANGEPIN_H__183CC40F_FFFC_4C35_A292_1E6D17D15917__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangePin dialog

class CChangePin : public CDialog
{
// Construction
public:
	CChangePin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangePin)
	enum { IDD = IDD_DIALOG2 };
	CString	m_NewPin1;
	CString	m_NewPin2;
	CString	m_OldPin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePin)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPIN_H__183CC40F_FFFC_4C35_A292_1E6D17D15917__INCLUDED_)
