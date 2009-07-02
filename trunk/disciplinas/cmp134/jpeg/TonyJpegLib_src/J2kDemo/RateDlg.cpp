// RateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "RateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRateDlg dialog


CRateDlg::CRateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRateDlg::IDD, pParent)
{
	m_nTrackbar1 = 50;//default compression rate

	//{{AFX_DATA_INIT(CRateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate) {
		TRACE("updating trackbar data members\n");
		CSliderCtrl* pSlide1 =
			(CSliderCtrl*) GetDlgItem(IDC_TRACKBAR1);
		m_nTrackbar1 = pSlide1->GetPos();	
	}
}


BEGIN_MESSAGE_MAP(CRateDlg, CDialog)
	//{{AFX_MSG_MAP(CRateDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRateDlg message handlers

BOOL CRateDlg::OnInitDialog() 
{
	CString strText1;
	CSliderCtrl* pSlide1 = (CSliderCtrl*) GetDlgItem(IDC_TRACKBAR1);
	pSlide1->SetRange(1, 300);
	pSlide1->SetPos(m_nTrackbar1);
	strText1.Format("%d", pSlide1->GetPos());
	SetDlgItemText(IDC_STATIC_TRACK1, strText1);
	
	return CDialog::OnInitDialog();
}

void CRateDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    CSliderCtrl* pSlide = (CSliderCtrl*) pScrollBar;
    CString strText;

    switch(pScrollBar->GetDlgCtrlID()) {
    case IDC_TRACKBAR1:
        strText.Format("%d", pSlide->GetPos());
        SetDlgItemText(IDC_STATIC_TRACK1, strText);
        break;
	}
}
