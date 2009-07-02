// JppDoc.cpp : implementation file
//

#include "stdafx.h"
#include "J2kDemo.h"
#include "JppDoc.h"

#include "QualityDlg.h"
#include "RateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJppDoc

IMPLEMENT_DYNCREATE(CJppDoc, CDocument)

CJppDoc::CJppDoc()
{
}

BOOL CJppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CJppDoc::~CJppDoc()
{
}


BEGIN_MESSAGE_MAP(CJppDoc, CDocument)
	//{{AFX_MSG_MAP(CJppDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJppDoc diagnostics

#ifdef _DEBUG
void CJppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJppDoc serialization

void CJppDoc::Serialize(CArchive& ar)
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
// CJppDoc commands

BOOL CJppDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_dib.ReadJppFile( lpszPathName );
	return TRUE;
}

void CJppDoc::OnFileSaveAs() 
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
