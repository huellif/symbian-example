# Microsoft Developer Studio Project File - Name="BitmapSprite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BitmapSprite - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BitmapSprite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BitmapSprite.mak" CFG="BitmapSprite - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BitmapSprite - Win32 Uni Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "BitmapSprite - Win32 Uni Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BitmapSprite - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\release\wins\udeb"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS\UDEB"
# ADD CPP /nologo /Zp4 /GF /GX /GR /MTd /W4 /Zi /Od /X /I "\work\example\Graphics\WS\BitmapSprite" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /D "__SYMBIAN32__" /D "__S60_50__" /D "__S60_3X__" /D "__SERIES60_3X__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "_DEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FI"Symbian_OS_v9.3.hrh" /FR /Fd"\epoc32\release\wins\udeb\BitmapSprite.PDB" /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\wins\UDEB\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\bitgdi.lib" "\epoc32\release\WINS\UDEB\gdi.lib" /nologo /fixed:no  /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /pdb:"\epoc32\release\wins\udeb\BitmapSprite.pdb" /debug /machine:IX86 /ignore:4089 /ignore:4005 /nodefaultlib:libcmtd.lib /include:"__E32Startup" /out:"\epoc32\release\wins\udeb\BitmapSprite.exe" /WARN:3
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "BitmapSprite - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\release\wins\urel"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS\UREL"
# ADD CPP /nologo /Zp4 /GF /GX /GR /MT /W4 /O1 /Op /X /I "\work\example\Graphics\WS\BitmapSprite" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /D "__SYMBIAN32__" /D "__S60_50__" /D "__S60_3X__" /D "__SERIES60_3X__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "NDEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FI"Symbian_OS_v9.3.hrh" /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\wins\UREL\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\ws32.lib" "\epoc32\release\WINS\UDEB\bitgdi.lib" "\epoc32\release\WINS\UDEB\gdi.lib" /nologo /fixed:no  /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /machine:IX86 /ignore:4089 /ignore:4005 /nodefaultlib:libcmt.lib /include:"__E32Startup" /out:"\epoc32\release\wins\urel\BitmapSprite.exe" /WARN:3

!ENDIF 

# Begin Target

# Name "BitmapSprite - Win32 Uni Debug"
# Name "BitmapSprite - Win32 Uni Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.rss
USERDEP__BitmapSprite=

!IF  "$(CFG)" == "BitmapSprite - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UDEB\z\Resource\Apps\BitmapSprite.r"

"\epoc32\release\wins\UDEB\z\Resource\Apps\BitmapSprite.r01" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "BitmapSprite - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UREL\z\Resource\Apps\BitmapSprite.r"

"\epoc32\release\wins\UREL\z\Resource\Apps\BitmapSprite.r01" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_reg.rss
USERDEP__BitmapSprite_reg=

!IF  "$(CFG)" == "BitmapSprite - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite_reg.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_reg.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UDEB\z\private\10003a3f\apps\BitmapSprite_reg.r"

"\epoc32\release\wins\UDEB\z\private\10003a3f\apps\BitmapSprite_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "BitmapSprite - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite_reg.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_reg.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UREL\z\private\10003a3f\apps\BitmapSprite_reg.r"

"\epoc32\release\wins\UREL\z\private\10003a3f\apps\BitmapSprite_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_loc.rss
USERDEP__BitmapSprite_loc=

!IF  "$(CFG)" == "BitmapSprite - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite_loc.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_loc.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UDEB\BitmapSprite_loc.r"

"\epoc32\release\wins\UDEB\BitmapSprite_loc.r01" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "BitmapSprite - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\BitmapSprite\BitmapSprite\WINS"
# Begin Custom Build - Building resources from Bitmapsprite_loc.rss
InputPath=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite_loc.rss

BuildCmds= \
	nmake -nologo -f "\work\example\Graphics\WS\BitmapSprite\BitmapSprite.SUP.MAKE"\
  "\epoc32\release\wins\UREL\BitmapSprite_loc.r"

"\epoc32\release\wins\UREL\BitmapSprite_loc.r01" : $(SOURCE) "$(INTDIR)"\
 "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Appholder.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Base.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.uid.cpp
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.mmp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Bitmapsprite.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Appholder.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\BitmapSprite\Base.h
# End Source File
# End Group
# End Target
# End Project
