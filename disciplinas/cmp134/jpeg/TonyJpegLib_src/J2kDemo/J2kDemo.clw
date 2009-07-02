; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CBmpDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "J2kDemo.h"
LastPage=0

ClassCount=20
Class1=CJ2kDemoApp
Class2=CJ2kDemoDoc
Class3=CJ2kDemoView
Class4=CMainFrame

ResourceCount=11
Resource1=IDD_OPTION_DLG
Resource2=IDR_JPGTYPE
Class5=CChildFrame
Class6=CAboutDlg
Class7=CBmpDoc
Class8=CBmpView
Class9=CJpgDoc
Class10=CJpgView
Class11=CJpcDoc
Class12=CJpcView
Class13=CJp2Doc
Class14=CJp2View
Resource3=IDD_USAGE_DLG
Resource4=IDR_JP2TYPE
Resource5=IDR_JPPTYPE
Resource6=IDR_JPCTYPE
Class15=CRateDlg
Resource7=IDD_QUALITY_DLG
Class16=CQualityDlg
Resource8=IDD_RATE_DLG
Class17=CUsageDlg
Resource9=IDR_BMPTYPE
Class18=CJppDoc
Class19=CJppView
Resource10=IDD_ABOUTBOX
Class20=COptionDlg
Resource11=IDR_MAINFRAME

[CLS:CJ2kDemoApp]
Type=0
HeaderFile=J2kDemo.h
ImplementationFile=J2kDemo.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=ID_HELP_USING

[CLS:CJ2kDemoDoc]
Type=0
HeaderFile=J2kDemoDoc.h
ImplementationFile=J2kDemoDoc.cpp
Filter=N
LastObject=CJ2kDemoDoc

[CLS:CJ2kDemoView]
Type=0
HeaderFile=J2kDemoView.h
ImplementationFile=J2kDemoView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M


[CLS:CAboutDlg]
Type=0
HeaderFile=J2kDemo.cpp
ImplementationFile=J2kDemo.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=10
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_OPTION
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_APP_ABOUT
Command6=ID_HELP_USING
CommandCount=6

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_OPTION
Command4=ID_FILE_SAVE_AS
Command5=ID_APP_ABOUT
Command6=ID_HELP_USING
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[CLS:CBmpDoc]
Type=0
HeaderFile=BmpDoc.h
ImplementationFile=BmpDoc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC
LastObject=CBmpDoc

[CLS:CBmpView]
Type=0
HeaderFile=BmpView.h
ImplementationFile=BmpView.cpp
BaseClass=CScrollView
Filter=C
VirtualFilter=VWC

[MNU:IDR_BMPTYPE]
Type=1
Class=CJ2kDemoView
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_OPTION
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
Command12=ID_HELP_USING
CommandCount=12

[CLS:CJpgDoc]
Type=0
HeaderFile=JpgDoc.h
ImplementationFile=JpgDoc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC

[CLS:CJpgView]
Type=0
HeaderFile=JpgView.h
ImplementationFile=JpgView.cpp
BaseClass=CScrollView
Filter=C
VirtualFilter=VWC

[CLS:CJpcDoc]
Type=0
HeaderFile=JpcDoc.h
ImplementationFile=JpcDoc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC

[CLS:CJpcView]
Type=0
HeaderFile=JpcView.h
ImplementationFile=JpcView.cpp
BaseClass=CScrollView
Filter=C
VirtualFilter=VWC

[CLS:CJp2Doc]
Type=0
HeaderFile=Jp2Doc.h
ImplementationFile=Jp2Doc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC

[CLS:CJp2View]
Type=0
HeaderFile=Jp2View.h
ImplementationFile=Jp2View.cpp
BaseClass=CScrollView
Filter=C
LastObject=CJp2View
VirtualFilter=VWC

[MNU:IDR_JPGTYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_OPTION
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
Command12=ID_HELP_USING
CommandCount=12

[MNU:IDR_JPCTYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_OPTION
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
Command12=ID_HELP_USING
CommandCount=12

[MNU:IDR_JP2TYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_OPTION
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
Command12=ID_HELP_USING
CommandCount=12

[DLG:IDD_RATE_DLG]
Type=1
Class=CRateDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_TRACKBAR1,msctls_trackbar32,1342242840
Control3=IDC_STATIC_TRACK1,static,1342308352

[CLS:CRateDlg]
Type=0
HeaderFile=RateDlg.h
ImplementationFile=RateDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CRateDlg
VirtualFilter=dWC

[DLG:IDD_QUALITY_DLG]
Type=1
Class=CQualityDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_TRACKBAR1,msctls_trackbar32,1342242840
Control3=IDC_STATIC_TRACK1,static,1342308352

[CLS:CQualityDlg]
Type=0
HeaderFile=QualityDlg.h
ImplementationFile=QualityDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CQualityDlg
VirtualFilter=dWC

[DLG:IDD_USAGE_DLG]
Type=1
Class=CUsageDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[CLS:CUsageDlg]
Type=0
HeaderFile=UsageDlg.h
ImplementationFile=UsageDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CUsageDlg

[MNU:IDR_JPPTYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_OPTION
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_WINDOW_NEW
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
Command12=ID_HELP_USING
CommandCount=12

[CLS:CJppDoc]
Type=0
HeaderFile=JppDoc.h
ImplementationFile=JppDoc.cpp
BaseClass=CDocument
Filter=N
VirtualFilter=DC

[CLS:CJppView]
Type=0
HeaderFile=JppView.h
ImplementationFile=JppView.cpp
BaseClass=CScrollView
Filter=C
LastObject=CJppView
VirtualFilter=VWC

[DLG:IDD_OPTION_DLG]
Type=1
Class=COptionDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_RADIO1,button,1342373897
Control4=IDC_RADIO2,button,1342242825
Control5=IDC_STATIC,button,1342177287

[CLS:COptionDlg]
Type=0
HeaderFile=OptionDlg.h
ImplementationFile=OptionDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_RADIO1
VirtualFilter=dWC

