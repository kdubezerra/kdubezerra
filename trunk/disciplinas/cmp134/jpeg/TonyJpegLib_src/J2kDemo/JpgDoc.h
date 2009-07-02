#if !defined(AFX_JPGDOC_H__4EC515D8_B6A7_4F8C_BFBD_9AA0E447F805__INCLUDED_)
#define AFX_JPGDOC_H__4EC515D8_B6A7_4F8C_BFBD_9AA0E447F805__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpgDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpgDoc document

class CJpgDoc : public CDocument
{
protected:
	CJpgDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpgDoc)

// Attributes
public:
	CDib m_dib;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpgDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJpgDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpgDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPGDOC_H__4EC515D8_B6A7_4F8C_BFBD_9AA0E447F805__INCLUDED_)
