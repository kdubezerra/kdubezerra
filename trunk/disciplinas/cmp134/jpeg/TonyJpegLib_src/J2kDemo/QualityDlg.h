#if !defined(AFX_QUALITYDLG_H__8FBBA0B0_121A_493F_BA7B_E1A19916B26D__INCLUDED_)
#define AFX_QUALITYDLG_H__8FBBA0B0_121A_493F_BA7B_E1A19916B26D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QualityDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQualityDlg dialog

class CQualityDlg : public CDialog
{
// Construction
public:
	CQualityDlg(CWnd* pParent = NULL);   // standard constructor

	int m_nTrackbar1;

// Dialog Data
	//{{AFX_DATA(CQualityDlg)
	enum { IDD = IDD_QUALITY_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQualityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQualityDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUALITYDLG_H__8FBBA0B0_121A_493F_BA7B_E1A19916B26D__INCLUDED_)
