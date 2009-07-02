// JpgView.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "JpgView.h"

#include "JpgDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJpgView

IMPLEMENT_DYNCREATE(CJpgView, CScrollView)

CJpgView::CJpgView()
{
}

CJpgView::~CJpgView()
{
}


BEGIN_MESSAGE_MAP(CJpgView, CScrollView)
	//{{AFX_MSG_MAP(CJpgView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpgView drawing

void CJpgView::OnDraw(CDC* pDC)
{
	CJpgDoc* pDoc = (CJpgDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
//	pDoc->m_dib.SetPalette( pDC );
	pDoc->m_dib.Draw( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CJpgView diagnostics

#ifdef _DEBUG
void CJpgView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CJpgView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJpgView message handlers

void CJpgView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	CJpgDoc* pDoc = (CJpgDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	int w, h;
	w = pDoc->m_dib.m_width;
	h = pDoc->m_dib.m_height;
	
	CSize sizeTotal(w, h);
    CSize sizePage(sizeTotal.cx / 2, sizeTotal.cy / 2);
    CSize sizeLine(sizeTotal.cx / 50, sizeTotal.cy / 50);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);	
	
}
