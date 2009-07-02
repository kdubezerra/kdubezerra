// BmpDoc.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "BmpDoc.h"
#include "RateDlg.h"
#include "QualityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpDoc

IMPLEMENT_DYNCREATE(CBmpDoc, CDocument)

CBmpDoc::CBmpDoc()
{
}

BOOL CBmpDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBmpDoc::~CBmpDoc()
{
}


BEGIN_MESSAGE_MAP(CBmpDoc, CDocument)
	//{{AFX_MSG_MAP(CBmpDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpDoc diagnostics

#ifdef _DEBUG
void CBmpDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBmpDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBmpDoc serialization

void CBmpDoc::Serialize(CArchive& ar)
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
// CBmpDoc commands

BOOL CBmpDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_dib.Load( lpszPathName );
	
	return TRUE;
}

void CBmpDoc::OnFileSaveAs() 
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
