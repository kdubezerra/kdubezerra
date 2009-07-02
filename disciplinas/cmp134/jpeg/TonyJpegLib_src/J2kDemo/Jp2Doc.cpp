// Jp2Doc.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "Jp2Doc.h"
#include "RateDlg.h"
#include "QualityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJp2Doc

IMPLEMENT_DYNCREATE(CJp2Doc, CDocument)

CJp2Doc::CJp2Doc()
{
}

BOOL CJp2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CJp2Doc::~CJp2Doc()
{
}


BEGIN_MESSAGE_MAP(CJp2Doc, CDocument)
	//{{AFX_MSG_MAP(CJp2Doc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJp2Doc diagnostics

#ifdef _DEBUG
void CJp2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJp2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJp2Doc serialization

void CJp2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJp2Doc commands

BOOL CJp2Doc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_dib.LoadFrom( lpszPathName );
	
	return TRUE;
}

void CJp2Doc::OnFileSaveAs() 
{

	char BASED_CODE szFilter[] = 
		"BMP Files (*.bmp)|*.bmp|JPG Files (*.jpg)|*.jpg|\
		JPP Files (*.jpp)|*.jpp|JPC Files (*.jpc)|*.jpc|JP2 Files (*.jp2)|*.jp2||";

	CString strPath, strFile, strExt;

	CFileDialog dlg( FALSE, "bmp", "tmp",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter );

	if( dlg.DoModal() == IDOK )
	{
		strPath = dlg.GetPathName();
		strFile = dlg.GetFileName();
		strExt = dlg.GetFileExt();

		if(( strExt == "bmp" )||( strExt == "BMP" ))
		{
			m_dib.Save( strPath );
		}
		else if(( strExt == "jpg" )||( strExt == "JPG" ))
		{
			CQualityDlg dlg;
			dlg.DoModal();
			int quality = dlg.m_nTrackbar1;			
			m_dib.SaveJpg( strPath, true, quality );
		}
		else if(( strExt == "jpp" )||( strExt == "JPP" ))
		{			
			m_dib.SaveJppFile( strPath );
		}
		else if(( strExt == "jpc" )||( strExt == "JPC" ))
		{	
			CRateDlg dlg;
			dlg.DoModal();
			int rate = dlg.m_nTrackbar1;
			m_dib.SaveAs( strPath, rate );
		}
		else if(( strExt == "jp2" )||( strExt == "JP2" ))
		{	
			CRateDlg dlg;
			dlg.DoModal();
			int rate = dlg.m_nTrackbar1;
			m_dib.SaveAs( strPath, rate );
		}
		else
		{
			AfxMessageBox( "unsupported file type !" );
			return;
		}
	}
}
