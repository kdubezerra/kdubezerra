// J2kDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "J2kDemo.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "BmpDoc.h"
#include "BmpView.h"

#include "JpgDoc.h"
#include "JpgView.h"

#include "JpcDoc.h"
#include "JpcView.h"

#include "Jp2Doc.h"
#include "Jp2View.h"

#include "JppDoc.h"
#include "JppView.h"

#include "UsageDlg.h"
#include "OptionDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int g_nJpegCodec = 1;//0:IJG Jpeg lib; 1:TonyJpegLib.

/////////////////////////////////////////////////////////////////////////////
// CJ2kDemoApp

BEGIN_MESSAGE_MAP(CJ2kDemoApp, CWinApp)
	//{{AFX_MSG_MAP(CJ2kDemoApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_USING, OnHelpUsing)
	ON_COMMAND(ID_FILE_OPTION, OnFileOption)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJ2kDemoApp construction

CJ2kDemoApp::CJ2kDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJ2kDemoApp object

CJ2kDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CJ2kDemoApp initialization

BOOL CJ2kDemoApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
/*
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_J2KDEMTYPE,
		RUNTIME_CLASS(CJ2kDemoDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CJ2kDemoView));
	AddDocTemplate(pDocTemplate);
*/

	//	jpg 文档模板
	m_pJpgDocTemplate = new CMultiDocTemplate(
		IDR_JPGTYPE,
		RUNTIME_CLASS(CJpgDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CJpgView));
	AddDocTemplate(m_pJpgDocTemplate);
	
	//	Bmp 文档模板
	m_pBmpDocTemplate = new CMultiDocTemplate(
		IDR_BMPTYPE,
		RUNTIME_CLASS(CBmpDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CBmpView));
	AddDocTemplate(m_pBmpDocTemplate);

	//	jpc 文档模板； jpeg 2000 的基本形式
	m_pJpcDocTemplate = new CMultiDocTemplate(
		IDR_JPCTYPE,
		RUNTIME_CLASS(CJpcDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CJpcView));
	AddDocTemplate(m_pJpcDocTemplate);

	//	jp2 文档模板；jpeg 2000 的复杂形式
	m_pJp2DocTemplate = new CMultiDocTemplate(
		IDR_JP2TYPE,
		RUNTIME_CLASS(CJp2Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CJp2View));
	AddDocTemplate(m_pJp2DocTemplate);

	//	jpp doc template; My own format
	m_pJppDocTemplate = new CMultiDocTemplate(
		IDR_JPPTYPE,
		RUNTIME_CLASS(CJppDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CJppView));
	AddDocTemplate(m_pJppDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
/*
	//去掉第一个子窗口

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
*/
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CJ2kDemoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CJ2kDemoApp message handlers


void CJ2kDemoApp::OnHelpUsing() 
{
	CUsageDlg dlg;
	dlg.DoModal();
}

void CJ2kDemoApp::OnFileOption() 
{
	COptionDlg dlg;
	dlg.m_nJpegCodec = g_nJpegCodec;
	if( dlg.DoModal() == IDOK )
	{
		g_nJpegCodec = dlg.m_nJpegCodec;
	}
}
