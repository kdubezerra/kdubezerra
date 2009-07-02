// BmpView.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "BmpView.h"

#include "BmpDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpView

IMPLEMENT_DYNCREATE(CBmpView, CScrollView)

CBmpView::CBmpView()
{
}

CBmpView::~CBmpView()
{
}


BEGIN_MESSAGE_MAP(CBmpView, CScrollView)
	//{{AFX_MSG_MAP(CBmpView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpView drawing

void CBmpView::OnDraw(CDC* pDC)
{
	CBmpDoc* pDoc = (CBmpDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_dib.SetPalette( pDC );
	pDoc->m_dib.Draw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CBmpView diagnostics

#ifdef _DEBUG
void CBmpView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBmpView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBmpView message handlers

void CBmpView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	CBmpDoc* pDoc = (CBmpDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	int w, h;
	w = pDoc->m_dib.m_width;
	h = pDoc->m_dib.m_height;
	
	CSize sizeTotal(w, h);
    CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
    CSize sizeLine(sizeTotal.cx / 50, sizeTotal.cy / 50);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);	
}
