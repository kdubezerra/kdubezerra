// QualityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "QualityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQualityDlg dialog


CQualityDlg::CQualityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQualityDlg::IDD, pParent)
{
	m_nTrackbar1 = 75;//default quality

	//{{AFX_DATA_INIT(CQualityDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CQualityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQualityDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		TRACE("updating trackbar data members\n");
		CSliderCtrl* pSlide1 =
			(CSliderCtrl*) GetDlgItem(IDC_TRACKBAR1);
		m_nTrackbar1 = pSlide1->GetPos();	
	}
}


BEGIN_MESSAGE_MAP(CQualityDlg, CDialog)
	//{{AFX_MSG_MAP(CQualityDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQualityDlg message handlers

BOOL CQualityDlg::OnInitDialog() 
{
	CString strText1;
	CSliderCtrl* pSlide1 = (CSliderCtrl*) GetDlgItem(IDC_TRACKBAR1);
	pSlide1->SetRange(1, 100);
	pSlide1->SetPos(m_nTrackbar1);
	strText1.Format("%d", pSlide1->GetPos());
	SetDlgItemText(IDC_STATIC_TRACK1, strText1);
	
	return CDialog::OnInitDialog();
}

void CQualityDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
