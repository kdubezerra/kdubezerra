// J2kDemo.h : main header file for the J2KDEMO application
//

#if !defined(AFX_J2KDEMO_H__6FE07413_FCC7_4107_953C_C9B0AECC3248__INCLUDED_)
#define AFX_J2KDEMO_H__6FE07413_FCC7_4107_953C_C9B0AECC3248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CJ2kDemoApp:
// See J2kDemo.cpp for the implementation of this class
//

class CJ2kDemoApp : public CWinApp
{
public:
	CJ2kDemoApp();

	CMultiDocTemplate* m_pBmpDocTemplate;
	CMultiDocTemplate* m_pJppDocTemplate;
	CMultiDocTemplate* m_pJpgDocTemplate;
	CMultiDocTemplate* m_pJpcDocTemplate;
	CMultiDocTemplate* m_pJp2DocTemplate;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJ2kDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CJ2kDemoApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelpUsing();
	afx_msg void OnFileOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_J2KDEMO_H__6FE07413_FCC7_4107_953C_C9B0AECC3248__INCLUDED_)
