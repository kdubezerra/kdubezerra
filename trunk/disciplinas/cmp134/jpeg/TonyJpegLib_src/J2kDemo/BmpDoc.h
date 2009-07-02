#if !defined(AFX_BMPDOC_H__2710DAEE_4618_4E60_A4C1_7CD8B2CEDDEF__INCLUDED_)
#define AFX_BMPDOC_H__2710DAEE_4618_4E60_A4C1_7CD8B2CEDDEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpDoc document

class CBmpDoc : public CDocument
{
protected:
	CBmpDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBmpDoc)

// Attributes
public:
	CDib m_dib;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBmpDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPDOC_H__2710DAEE_4618_4E60_A4C1_7CD8B2CEDDEF__INCLUDED_)
