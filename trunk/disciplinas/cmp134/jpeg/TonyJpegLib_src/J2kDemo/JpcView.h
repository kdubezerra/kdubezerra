#if !defined(AFX_JPCVIEW_H__A7C88901_4F17_4E35_A94B_013E2F858A48__INCLUDED_)
#define AFX_JPCVIEW_H__A7C88901_4F17_4E35_A94B_013E2F858A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpcView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpcView view

class CJpcView : public CScrollView
{
protected:
	CJpcView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpcView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpcView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJpcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpcView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPCVIEW_H__A7C88901_4F17_4E35_A94B_013E2F858A48__INCLUDED_)
