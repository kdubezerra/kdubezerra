#if !defined(AFX_JP2DOC_H__D2877F71_3728_4E04_9EB6_0E4A4EF89791__INCLUDED_)
#define AFX_JP2DOC_H__D2877F71_3728_4E04_9EB6_0E4A4EF89791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Jp2Doc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJp2Doc document

class CJp2Doc : public CDocument
{
protected:
	CJp2Doc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJp2Doc)

// Attributes
public:
	CDib m_dib;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJp2Doc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJp2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJp2Doc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JP2DOC_H__D2877F71_3728_4E04_9EB6_0E4A4EF89791__INCLUDED_)
