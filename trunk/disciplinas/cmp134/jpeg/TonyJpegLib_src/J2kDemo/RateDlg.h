#if !defined(AFX_RATEDLG_H__C478E2A6_0F2F_4955_A2E3_8EFD519A79D2__INCLUDED_)
#define AFX_RATEDLG_H__C478E2A6_0F2F_4955_A2E3_8EFD519A79D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRateDlg dialog

class CRateDlg : public CDialog
{
// Construction
public:
	CRateDlg(CWnd* pParent = NULL);   // standard constructor
	
	int m_nTrackbar1;

// Dialog Data
	//{{AFX_DATA(CRateDlg)
	enum { IDD = IDD_RATE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RATEDLG_H__C478E2A6_0F2F_4955_A2E3_8EFD519A79D2__INCLUDED_)
