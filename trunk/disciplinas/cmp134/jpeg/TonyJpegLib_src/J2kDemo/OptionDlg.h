#if !defined(AFX_OPTIONDLG_H__C5B64B99_7CF5_40CF_A77C_7CF072BFC2F8__INCLUDED_)
#define AFX_OPTIONDLG_H__C5B64B99_7CF5_40CF_A77C_7CF072BFC2F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog

class COptionDlg : public CDialog
{
// Construction
public:
	COptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionDlg)
	enum { IDD = IDD_OPTION_DLG };
	int		m_nJpegCodec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDLG_H__C5B64B99_7CF5_40CF_A77C_7CF072BFC2F8__INCLUDED_)
