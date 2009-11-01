# Microsoft Developer Studio Project File - Name="AudioClientEx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AudioClientEx - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AudioClientEx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AudioClientEx.mak" CFG="AudioClientEx - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AudioClientEx - Win32 Uni Release" (based on "Win32 (x86) Application")
!MESSAGE "AudioClientEx - Win32 Uni Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AudioClientEx - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\release\WINS\UREL"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS\UREL"
# ADD CPP /nologo /Zp4 /MT /W4 /GR /GX /O1 /Op /X /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /FI"Symbian_OS_v9.3.hrh" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "NDEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\WINS\UREL\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\efsrv.lib" "\epoc32\release\WINS\UDEB\bafl.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\eikdlg.lib" "\epoc32\release\WINS\UDEB\eikcoctl.lib" "\epoc32\release\WINS\UDEB\eikfile.lib" "\epoc32\release\WINS\UDEB\eikctl.lib" "\epoc32\release\WINS\UDEB\gdi.lib" "\epoc32\release\WINS\UDEB\mediaclientaudio.lib" "\epoc32\release\WINS\UDEB\mediaclientaudiostream.lib" "\epoc32\release\WINS\UDEB\mmfcontrollerframework.lib" /nologo /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /machine:IX86 /nodefaultlib:"libcmt.lib" /include:"__E32Startup" /fixed:no /ignore:4089 /ignore:4005 /WARN:3

!ELSEIF  "$(CFG)" == "AudioClientEx - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\release\WINS\UDEB"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS\UDEB"
# ADD CPP /nologo /Zp4 /MTd /W4 /GR /GX /Zi /Od /X /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /FI"Symbian_OS_v9.3.hrh" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "_DEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FR /Fd"\epoc32\release\WINS\UDEB\AudioClientEx.PDB" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\WINS\UDEB\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\efsrv.lib" "\epoc32\release\WINS\UDEB\bafl.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\eikdlg.lib" "\epoc32\release\WINS\UDEB\eikcoctl.lib" "\epoc32\release\WINS\UDEB\eikfile.lib" "\epoc32\release\WINS\UDEB\eikctl.lib" "\epoc32\release\WINS\UDEB\gdi.lib" "\epoc32\release\WINS\UDEB\mediaclientaudio.lib" "\epoc32\release\WINS\UDEB\mediaclientaudiostream.lib" "\epoc32\release\WINS\UDEB\mmfcontrollerframework.lib" /nologo /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /debug /machine:IX86 /nodefaultlib:"libcmtd.lib" /include:"__E32Startup" /fixed:no /ignore:4089 /ignore:4005 /WARN:3
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AudioClientEx - Win32 Uni Release"
# Name "AudioClientEx - Win32 Uni Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Audioclientex.mmp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Audioclientex.rss

!IF  "$(CFG)" == "AudioClientEx - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS"
# Begin Custom Build - Building resources from Audioclientex.rss
InputPath=\work\Example\Multimedia\AudioClientEx\Audioclientex.rss

"\epoc32\release\WINS\UREL\z\resource\apps\AudioClientEx.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\Example\Multimedia\AudioClientEx\AudioClientEx.SUP.MAKE"   "\epoc32\release\WINS\UREL\z\resource\apps\AudioClientEx.r"

# End Custom Build

!ELSEIF  "$(CFG)" == "AudioClientEx - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS"
# Begin Custom Build - Building resources from Audioclientex.rss
InputPath=\work\Example\Multimedia\AudioClientEx\Audioclientex.rss

"\epoc32\release\WINS\UDEB\z\resource\apps\AudioClientEx.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\Example\Multimedia\AudioClientEx\AudioClientEx.SUP.MAKE"   "\epoc32\release\WINS\UDEB\z\resource\apps\AudioClientEx.r"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Audioclientex.uid.cpp
# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Audioclientex_reg.rss

!IF  "$(CFG)" == "AudioClientEx - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS"
# Begin Custom Build - Building resources from Audioclientex_reg.rss
InputPath=\work\Example\Multimedia\AudioClientEx\Audioclientex_reg.rss

"\epoc32\release\WINS\UREL\z\private\10003a3f\apps\AudioClientEx_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\Example\Multimedia\AudioClientEx\AudioClientEx.SUP.MAKE"   "\epoc32\release\WINS\UREL\z\private\10003a3f\apps\AudioClientEx_reg.r"

# End Custom Build

!ELSEIF  "$(CFG)" == "AudioClientEx - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\Example\Multimedia\AudioClientEx\AudioClientEx\WINS"
# Begin Custom Build - Building resources from Audioclientex_reg.rss
InputPath=\work\Example\Multimedia\AudioClientEx\Audioclientex_reg.rss

"\epoc32\release\WINS\UDEB\z\private\10003a3f\apps\AudioClientEx_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\Example\Multimedia\AudioClientEx\AudioClientEx.SUP.MAKE"   "\epoc32\release\WINS\UDEB\z\private\10003a3f\apps\AudioClientEx_reg.r"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Cmediaclientapplication.cpp
# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Cmediaclientappui.cpp
# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Cmediaclientdocument.cpp
# End Source File
# Begin Source File

SOURCE=\work\Example\Multimedia\AudioClientEx\Cmediaclientengine.cpp
# End Source File
# Begin Source File

SOURCE=.\CMediaClientView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CMediaClientApplication.h
# End Source File
# Begin Source File

SOURCE=.\CMediaClientAppUI.h
# End Source File
# Begin Source File

SOURCE=.\CMediaClientDocument.h
# End Source File
# Begin Source File

SOURCE=.\CMediaClientEngine.h
# End Source File
# End Group
# End Target
# End Project
