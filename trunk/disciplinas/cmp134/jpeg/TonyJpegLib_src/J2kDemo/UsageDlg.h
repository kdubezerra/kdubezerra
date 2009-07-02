#if !defined(AFX_USAGEDLG_H__724B0296_5FD5_4219_B035_0FCBF14E8CF6__INCLUDED_)
#define AFX_USAGEDLG_H__724B0296_5FD5_4219_B035_0FCBF14E8CF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UsageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg dialog

class CUsageDlg : public CDialog
{
// Construction
public:
	CUsageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUsageDlg)
	enum { IDD = IDD_USAGE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUsageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUsageDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USAGEDLG_H__724B0296_5FD5_4219_B035_0FCBF14E8CF6__INCLUDED_)
