#if !defined(AFX_JPCDOC_H__21655718_373D_404B_9CE1_A4B8E860FC81__INCLUDED_)
#define AFX_JPCDOC_H__21655718_373D_404B_9CE1_A4B8E860FC81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpcDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpcDoc document

class CJpcDoc : public CDocument
{
protected:
	CJpcDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpcDoc)

// Attributes
public:
	CDib m_dib;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpcDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJpcDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpcDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPCDOC_H__21655718_373D_404B_9CE1_A4B8E860FC81__INCLUDED_)
