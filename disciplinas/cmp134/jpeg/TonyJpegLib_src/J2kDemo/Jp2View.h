#if !defined(AFX_JP2VIEW_H__9E008C14_DD66_4687_865D_187626A62A38__INCLUDED_)
#define AFX_JP2VIEW_H__9E008C14_DD66_4687_865D_187626A62A38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Jp2View.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJp2View view

class CJp2View : public CScrollView
{
protected:
	CJp2View();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJp2View)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJp2View)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJp2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJp2View)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JP2VIEW_H__9E008C14_DD66_4687_865D_187626A62A38__INCLUDED_)
