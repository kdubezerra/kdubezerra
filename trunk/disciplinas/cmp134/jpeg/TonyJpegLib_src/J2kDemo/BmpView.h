#if !defined(AFX_BMPVIEW_H__C1D52FFC_C865_4B7D_BFDC_5BB2D8224596__INCLUDED_)
#define AFX_BMPVIEW_H__C1D52FFC_C865_4B7D_BFDC_5BB2D8224596__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpView view

class CBmpView : public CScrollView
{
protected:
	CBmpView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBmpView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBmpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPVIEW_H__C1D52FFC_C865_4B7D_BFDC_5BB2D8224596__INCLUDED_)
