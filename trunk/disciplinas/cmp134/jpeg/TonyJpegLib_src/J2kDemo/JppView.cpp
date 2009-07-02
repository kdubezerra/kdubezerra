// JppView.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "JppView.h"

#include "JppDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJppView

IMPLEMENT_DYNCREATE(CJppView, CScrollView)

CJppView::CJppView()
{
}

CJppView::~CJppView()
{
}


BEGIN_MESSAGE_MAP(CJppView, CScrollView)
	//{{AFX_MSG_MAP(CJppView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJppView drawing

void CJppView::OnDraw(CDC* pDC)
{
	CJppDoc* pDoc = (CJppDoc*) GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_dib.Draw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CJppView diagnostics

#ifdef _DEBUG
void CJppView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CJppView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJppView message handlers

void CJppView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	CJppDoc* pDoc = (CJppDoc*) GetDocument();
	ASSERT_VALID(pDoc);
		
	int w, h;
	w = pDoc->m_dib.m_width;
	h = pDoc->m_dib.m_height;
	
	CSize sizeTotal(w, h);
    CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
    CSize sizeLine(sizeTotal.cx / 50, sizeTotal.cy / 50);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);	
}
