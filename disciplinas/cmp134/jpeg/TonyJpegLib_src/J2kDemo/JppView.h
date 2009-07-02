#if !defined(AFX_JPPVIEW_H__DC10F5D8_8774_4A62_ABD8_62A4C0D9F301__INCLUDED_)
#define AFX_JPPVIEW_H__DC10F5D8_8774_4A62_ABD8_62A4C0D9F301__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JppView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJppView view

class CJppView : public CScrollView
{
protected:
	CJppView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJppView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJppView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJppView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPPVIEW_H__DC10F5D8_8774_4A62_ABD8_62A4C0D9F301__INCLUDED_)
