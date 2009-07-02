#if !defined(AFX_JPPDOC_H__9D0B042E_C445_4A0E_B6BA_77F849812DE4__INCLUDED_)
#define AFX_JPPDOC_H__9D0B042E_C445_4A0E_B6BA_77F849812DE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JppDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJppDoc document

class CJppDoc : public CDocument
{
protected:
	CJppDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJppDoc)

// Attributes
public:
	CDib m_dib;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJppDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJppDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPPDOC_H__9D0B042E_C445_4A0E_B6BA_77F849812DE4__INCLUDED_)
