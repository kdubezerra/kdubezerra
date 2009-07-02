#if !defined(AFX_JPGVIEW_H__3A1CB88E_5C31_4DF0_BBE5_961570C451B7__INCLUDED_)
#define AFX_JPGVIEW_H__3A1CB88E_5C31_4DF0_BBE5_961570C451B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpgView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpgView view

class CJpgView : public CScrollView
{
protected:
	CJpgView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpgView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpgView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJpgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpgView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPGVIEW_H__3A1CB88E_5C31_4DF0_BBE5_961570C451B7__INCLUDED_)
