// Jp2View.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "Jp2View.h"

#include "Jp2Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJp2View

IMPLEMENT_DYNCREATE(CJp2View, CScrollView)

CJp2View::CJp2View()
{
}

CJp2View::~CJp2View()
{
}


BEGIN_MESSAGE_MAP(CJp2View, CScrollView)
	//{{AFX_MSG_MAP(CJp2View)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJp2View drawing

void CJp2View::OnDraw(CDC* pDC)
{
	CJp2Doc* pDoc = (CJp2Doc*)GetDocument();
	ASSERT_VALID(pDoc);
	
//	pDoc->m_dib.SetPalette( pDC );
	pDoc->m_dib.Draw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CJp2View diagnostics

#ifdef _DEBUG
void CJp2View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CJp2View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJp2View message handlers

void CJp2View::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();	
		
	CJp2Doc* pDoc = (CJp2Doc*)GetDocument();
	ASSERT_VALID(pDoc);

	int w, h;
	w = pDoc->m_dib.m_width;
	h = pDoc->m_dib.m_height;
	
	CSize sizeTotal(w, h);
    CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
    CSize sizeLine(sizeTotal.cx / 50, sizeTotal.cy / 50);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);	
}
