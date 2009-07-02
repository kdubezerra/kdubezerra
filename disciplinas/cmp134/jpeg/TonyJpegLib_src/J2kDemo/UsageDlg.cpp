// UsageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "UsageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg dialog


CUsageDlg::CUsageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUsageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUsageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUsageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUsageDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUsageDlg, CDialog)
	//{{AFX_MSG_MAP(CUsageDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg message handlers
