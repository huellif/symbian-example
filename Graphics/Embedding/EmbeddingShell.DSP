# Microsoft Developer Studio Project File - Name="EmbeddingShell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EmbeddingShell - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EmbeddingShell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EmbeddingShell.mak" CFG="EmbeddingShell - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EmbeddingShell - Win32 Uni Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "EmbeddingShell - Win32 Uni Debug" (based on\
 "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EmbeddingShell - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\release\WINS\UDEB"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS\UDEB"
# ADD CPP /nologo /Zp4 /GF /GX /GR /MTd /W4 /Zi /Od /X /I "\work\example\Graphics\Embedding" /I "\work\example\Graphics\CommonGraphicsExampleFiles" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "_DEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FI"Symbian_OS_v9.3.hrh" /FR /Fd"\epoc32\release\WINS\UDEB\EmbeddingShell.PDB" /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\WINS\UDEB\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\efsrv.lib" "\epoc32\release\WINS\UDEB\estor.lib" "\epoc32\release\WINS\UDEB\gdi.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\eikcoctl.lib" /nologo /fixed:no  /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /pdb:"\epoc32\release\WINS\UDEB\EmbeddingShell.pdb" /debug /machine:IX86 /ignore:4089 /ignore:4005 /nodefaultlib:libcmtd.lib /include:"__E32Startup" /out:"\epoc32\release\WINS\UDEB\EmbeddingShell.exe" /WARN:3
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "EmbeddingShell - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\release\WINS\UREL"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS\UREL"
# ADD CPP /nologo /Zp4 /GF /GX /GR /MT /W4 /O1 /Op /X /I "\work\example\Graphics\Embedding" /I "\work\example\Graphics\CommonGraphicsExampleFiles" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /D "__SYMBIAN32__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "NDEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FI"Symbian_OS_v9.3.hrh" /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\WINS\UREL\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\efsrv.lib" "\epoc32\release\WINS\UDEB\estor.lib" "\epoc32\release\WINS\UDEB\gdi.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\eikcoctl.lib" /nologo /fixed:no  /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /machine:IX86 /ignore:4089 /ignore:4005 /nodefaultlib:libcmt.lib /include:"__E32Startup" /out:"\epoc32\release\WINS\UREL\EmbeddingShell.exe" /WARN:3

!ENDIF 

# Begin Target

# Name "EmbeddingShell - Win32 Uni Debug"
# Name "EmbeddingShell - Win32 Uni Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.rss
USERDEP__EmbeddingShell="\work\example\Graphics\Embedding\EmbeddingShell.hrh"

!IF  "$(CFG)" == "EmbeddingShell - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS"
# Begin Custom Build - Building resources from Embeddingshell.rss
InputPath=\work\example\Graphics\Embedding\Embeddingshell.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\Embedding\EmbeddingShell.SUP.MAKE"\
  "\epoc32\release\WINS\UDEB\z\resource\apps\EmbeddingShell.r"

"\epoc32\release\WINS\UDEB\z\resource\apps\EmbeddingShell.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "EmbeddingShell - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS"
# Begin Custom Build - Building resources from Embeddingshell.rss
InputPath=\work\example\Graphics\Embedding\Embeddingshell.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\Embedding\EmbeddingShell.SUP.MAKE"\
  "\epoc32\release\WINS\UREL\z\resource\apps\EmbeddingShell.r"

"\epoc32\release\WINS\UREL\z\resource\apps\EmbeddingShell.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell_reg.rss
USERDEP__EmbeddingShell_reg=

!IF  "$(CFG)" == "EmbeddingShell - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS"
# Begin Custom Build - Building resources from Embeddingshell_reg.rss
InputPath=\work\example\Graphics\Embedding\Embeddingshell_reg.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\Embedding\EmbeddingShell.SUP.MAKE"\
  "\epoc32\release\WINS\UDEB\z\private\10003a3f\apps\EmbeddingShell_reg.r"

"\epoc32\release\WINS\UDEB\z\private\10003a3f\apps\EmbeddingShell_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "EmbeddingShell - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\Embedding\EmbeddingShell\WINS"
# Begin Custom Build - Building resources from Embeddingshell_reg.rss
InputPath=\work\example\Graphics\Embedding\Embeddingshell_reg.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\Embedding\EmbeddingShell.SUP.MAKE"\
  "\epoc32\release\WINS\UREL\z\private\10003a3f\apps\EmbeddingShell_reg.r"

"\epoc32\release\WINS\UREL\z\private\10003a3f\apps\EmbeddingShell_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddinggraphicscontrol.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Picture.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.uid.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.mmp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\CommonGraphicsExampleFiles\Commongraphicscontrolframework.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddingshell.hrh
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\Embedding\Embeddinggraphicscontrol.h
# End Source File
# End Group
# End Target
# End Project
