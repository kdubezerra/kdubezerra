// JpcView.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "JpcView.h"

#include "JpcDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJpcView

IMPLEMENT_DYNCREATE(CJpcView, CScrollView)

CJpcView::CJpcView()
{
}

CJpcView::~CJpcView()
{
}


BEGIN_MESSAGE_MAP(CJpcView, CScrollView)
	//{{AFX_MSG_MAP(CJpcView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpcView drawing

void CJpcView::OnDraw(CDC* pDC)
{
	CJpcDoc* pDoc = (CJpcDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
//	pDoc->m_dib.SetPalette( pDC );
	pDoc->m_dib.Draw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CJpcView diagnostics

#ifdef _DEBUG
void CJpcView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CJpcView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJpcView message handlers

void CJpcView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	CJpcDoc* pDoc = (CJpcDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	int w, h;
	w = pDoc->m_dib.m_width;
	h = pDoc->m_dib.m_height;
	
	CSize sizeTotal(w, h);
    CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
    CSize sizeLine(sizeTotal.cx / 50, sizeTotal.cy / 50);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);		
}
