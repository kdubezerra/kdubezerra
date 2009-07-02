# Microsoft Developer Studio Project File - Name="J2kDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=J2kDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "J2kDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "J2kDemo.mak" CFG="J2kDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "J2kDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "J2kDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "J2kDemo - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 J2kDll.lib jpeglib.lib /nologo /subsystem:windows /machine:I386 /out:"J2kDemo.exe"

!ELSEIF  "$(CFG)" == "J2kDemo - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 J2kDll_d.lib jpeglib_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"debug/J2kDemo_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "J2kDemo - Win32 Release"
# Name "J2kDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BmpDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DIB.cpp
# End Source File
# Begin Source File

SOURCE=.\J2kDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\J2kDemo.rc
# End Source File
# Begin Source File

SOURCE=.\Jp2Doc.cpp
# End Source File
# Begin Source File

SOURCE=.\Jp2View.cpp
# End Source File
# Begin Source File

SOURCE=.\JpcDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\JpcView.cpp
# End Source File
# Begin Source File

SOURCE=.\Jpegfile.cpp
# End Source File
# Begin Source File

SOURCE=.\JpgDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\JpgView.cpp
# End Source File
# Begin Source File

SOURCE=.\JppDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\JppView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniJpegDec.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniJpegEnc.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QualityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TonyJpegDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\TonyJpegEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\UsageDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BmpDoc.h
# End Source File
# Begin Source File

SOURCE=.\BmpView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\DIB.h
# End Source File
# Begin Source File

SOURCE=.\J2kDemo.h
# End Source File
# Begin Source File

SOURCE=.\j2kdll.h
# End Source File
# Begin Source File

SOURCE=.\Jp2Doc.h
# End Source File
# Begin Source File

SOURCE=.\Jp2View.h
# End Source File
# Begin Source File

SOURCE=.\JpcDoc.h
# End Source File
# Begin Source File

SOURCE=.\JpcView.h
# End Source File
# Begin Source File

SOURCE=.\Jpegfile.h
# End Source File
# Begin Source File

SOURCE=.\JpgDoc.h
# End Source File
# Begin Source File

SOURCE=.\JpgView.h
# End Source File
# Begin Source File

SOURCE=.\JppDoc.h
# End Source File
# Begin Source File

SOURCE=.\JppView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MiniJpegDec.h
# End Source File
# Begin Source File

SOURCE=.\MiniJpegEnc.h
# End Source File
# Begin Source File

SOURCE=.\OptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\QualityDlg.h
# End Source File
# Begin Source File

SOURCE=.\RateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TonyJpegDecoder.h
# End Source File
# Begin Source File

SOURCE=.\TonyJpegEncoder.h
# End Source File
# Begin Source File

SOURCE=.\UsageDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Hammer.ico
# End Source File
# Begin Source File

SOURCE=.\res\HAND.ICO
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_jpct.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_jpg.ico
# End Source File
# Begin Source File

SOURCE=.\res\J2kDemo.rc2
# End Source File
# Begin Source File

SOURCE=.\res\J2kDemoDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\PBRUSH.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
