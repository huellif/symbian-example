# Microsoft Developer Studio Project File - Name="Simple" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Simple - Win32 Uni Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Simple.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Simple.mak" CFG="Simple - Win32 Uni Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Simple - Win32 Uni Release" (based on "Win32 (x86) Application")
!MESSAGE "Simple - Win32 Uni Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Simple - Win32 Uni Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Win32_Un"
# PROP BASE Intermediate_Dir ".\Win32_Un"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\release\wins\urel"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS\UREL"
# ADD CPP /nologo /Zp4 /MT /W4 /GR /GX /O1 /Op /X /I "\work\example\Graphics\WS\Simple" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /FI"Symbian_OS_v9.3.hrh" /D "__SYMBIAN32__" /D "__S60_50__" /D "__S60_3X__" /D "__SERIES60_3X__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "NDEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\wins\UREL\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\ws32.lib" /nologo /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /machine:IX86 /nodefaultlib:"libcmt.lib" /include:"__E32Startup" /fixed:no /ignore:4089 /ignore:4005 /WARN:3

!ELSEIF  "$(CFG)" == "Simple - Win32 Uni Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_U0"
# PROP BASE Intermediate_Dir ".\Win32_U0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\release\wins\udeb"
# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS\UDEB"
# ADD CPP /nologo /Zp4 /MTd /W4 /GR /GX /Zi /Od /X /I "\work\example\Graphics\WS\Simple" /I "\epoc32\include" /I "\epoc32\include\techview" /I "\epoc32\include\variant" /FI"Symbian_OS_v9.3.hrh" /D "__SYMBIAN32__" /D "__S60_50__" /D "__S60_3X__" /D "__SERIES60_3X__" /D "__VC32__" /D "__WINS__" /D "__MSVC6__" /D "__MSVC600__" /D "__EXE__" /D "_DEBUG" /D "_UNICODE" /D "__SUPPORT_CPP_EXCEPTIONS__" /FR /Fd"\epoc32\release\wins\udeb\Simple.PDB" /GF /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "\epoc32\release\wins\UDEB\EEXE.LIB" "kernel32.lib" "\epoc32\release\WINS\UDEB\euser.lib" "\epoc32\release\WINS\UDEB\apparc.lib" "\epoc32\release\WINS\UDEB\cone.lib" "\epoc32\release\WINS\UDEB\eikcore.lib" "\epoc32\release\WINS\UDEB\ws32.lib" /nologo /entry:"?_E32Bootstrap@@YGXXZ" /subsystem:windows /debug /machine:IX86 /nodefaultlib:"libcmtd.lib" /include:"__E32Startup" /fixed:no /ignore:4089 /ignore:4005 /WARN:3
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Simple - Win32 Uni Release"
# Name "Simple - Win32 Uni Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Appholder.cpp
DEP_CPP_APPHO=\
	"..\..\..\..\..\epoc32\include\apacmdln.h"\
	"..\..\..\..\..\epoc32\include\apadef.h"\
	"..\..\..\..\..\epoc32\include\apaflrec.h"\
	"..\..\..\..\..\epoc32\include\apaid.h"\
	"..\..\..\..\..\epoc32\include\apmstd.h"\
	"..\..\..\..\..\epoc32\include\apparc.h"\
	"..\..\..\..\..\epoc32\include\babitflags.h"\
	"..\..\..\..\..\epoc32\include\badesca.h"\
	"..\..\..\..\..\epoc32\include\baerrhan.h"\
	"..\..\..\..\..\epoc32\include\bamdesca.h"\
	"..\..\..\..\..\epoc32\include\barsc.h"\
	"..\..\..\..\..\epoc32\include\basched.h"\
	"..\..\..\..\..\epoc32\include\bidi.h"\
	"..\..\..\..\..\epoc32\include\biditext.h"\
	"..\..\..\..\..\epoc32\include\bitbase.h"\
	"..\..\..\..\..\epoc32\include\bitdev.h"\
	"..\..\..\..\..\epoc32\include\bitdev.inl"\
	"..\..\..\..\..\epoc32\include\bitmap.h"\
	"..\..\..\..\..\epoc32\include\bitstd.h"\
	"..\..\..\..\..\epoc32\include\coeaui.h"\
	"..\..\..\..\..\epoc32\include\coeccntx.h"\
	"..\..\..\..\..\epoc32\include\coecntrl.h"\
	"..\..\..\..\..\epoc32\include\coecobs.h"\
	"..\..\..\..\..\epoc32\include\coecoloruse.h"\
	"..\..\..\..\..\epoc32\include\coecontrolarray.h"\
	"..\..\..\..\..\epoc32\include\coedef.h"\
	"..\..\..\..\..\epoc32\include\coefont.h"\
	"..\..\..\..\..\epoc32\include\coehelp.h"\
	"..\..\..\..\..\epoc32\include\coeinput.h"\
	"..\..\..\..\..\epoc32\include\coemain.h"\
	"..\..\..\..\..\epoc32\include\coemain.inl"\
	"..\..\..\..\..\epoc32\include\coemop.h"\
	"..\..\..\..\..\epoc32\include\coetextdrawer.h"\
	"..\..\..\..\..\epoc32\include\coeview.h"\
	"..\..\..\..\..\epoc32\include\d32locd.h"\
	"..\..\..\..\..\epoc32\include\d32locd.inl"\
	"..\..\..\..\..\epoc32\include\e32base.h"\
	"..\..\..\..\..\epoc32\include\e32base.inl"\
	"..\..\..\..\..\epoc32\include\e32capability.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.inl"\
	"..\..\..\..\..\epoc32\include\e32const.h"\
	"..\..\..\..\..\epoc32\include\e32debug.h"\
	"..\..\..\..\..\epoc32\include\e32def.h"\
	"..\..\..\..\..\epoc32\include\e32des16.h"\
	"..\..\..\..\..\epoc32\include\e32des8.h"\
	"..\..\..\..\..\epoc32\include\e32err.h"\
	"..\..\..\..\..\epoc32\include\e32event.h"\
	"..\..\..\..\..\epoc32\include\e32hal.h"\
	"..\..\..\..\..\epoc32\include\e32keys.h"\
	"..\..\..\..\..\epoc32\include\e32ktran.h"\
	"..\..\..\..\..\epoc32\include\e32ldr.h"\
	"..\..\..\..\..\epoc32\include\e32lmsg.h"\
	"..\..\..\..\..\epoc32\include\e32notif.h"\
	"..\..\..\..\..\epoc32\include\e32std.h"\
	"..\..\..\..\..\epoc32\include\e32std.inl"\
	"..\..\..\..\..\epoc32\include\e32svr.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecom.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomerrorcodes.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.inl"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.h"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.inl"\
	"..\..\..\..\..\epoc32\include\eikamnt.h"\
	"..\..\..\..\..\epoc32\include\eikapp.h"\
	"..\..\..\..\..\epoc32\include\eikappui.h"\
	"..\..\..\..\..\epoc32\include\eikaufty.h"\
	"..\..\..\..\..\epoc32\include\eikcmobs.h"\
	"..\..\..\..\..\epoc32\include\eikcolor.hrh"\
	"..\..\..\..\..\epoc32\include\eikdef.h"\
	"..\..\..\..\..\epoc32\include\eikdgfty.h"\
	"..\..\..\..\..\epoc32\include\eikdoc.h"\
	"..\..\..\..\..\epoc32\include\eikenv.h"\
	"..\..\..\..\..\epoc32\include\eikfctry.h"\
	"..\..\..\..\..\epoc32\include\eiklibry.h"\
	"..\..\..\..\..\epoc32\include\eikmobs.h"\
	"..\..\..\..\..\epoc32\include\EikStart.h"\
	"..\..\..\..\..\epoc32\include\eikvcurs.h"\
	"..\..\..\..\..\epoc32\include\f32file.h"\
	"..\..\..\..\..\epoc32\include\f32file.inl"\
	"..\..\..\..\..\epoc32\include\fbs.h"\
	"..\..\..\..\..\epoc32\include\FbsMessage.h"\
	"..\..\..\..\..\epoc32\include\fldbase.h"\
	"..\..\..\..\..\epoc32\include\fldbltin.h"\
	"..\..\..\..\..\epoc32\include\fldinfo.h"\
	"..\..\..\..\..\epoc32\include\fldset.h"\
	"..\..\..\..\..\epoc32\include\fntstore.h"\
	"..\..\..\..\..\epoc32\include\frmlaydt.h"\
	"..\..\..\..\..\epoc32\include\frmparam.h"\
	"..\..\..\..\..\epoc32\include\frmtlay.h"\
	"..\..\..\..\..\epoc32\include\frmvis.h"\
	"..\..\..\..\..\epoc32\include\gdi.h"\
	"..\..\..\..\..\epoc32\include\gdi.inl"\
	"..\..\..\..\..\epoc32\include\graphicsaccelerator.h"\
	"..\..\..\..\..\epoc32\include\gulalign.h"\
	"..\..\..\..\..\epoc32\include\gulbordr.h"\
	"..\..\..\..\..\epoc32\include\gulcolor.h"\
	"..\..\..\..\..\epoc32\include\guldef.h"\
	"..\..\..\..\..\epoc32\include\gulftflg.hrh"\
	"..\..\..\..\..\epoc32\include\gulicon.h"\
	"..\..\..\..\..\epoc32\include\gulsprite.h"\
	"..\..\..\..\..\epoc32\include\gulutil.h"\
	"..\..\..\..\..\epoc32\include\lafmain.h"\
	"..\..\..\..\..\epoc32\include\linebreak.h"\
	"..\..\..\..\..\epoc32\include\openfont.h"\
	"..\..\..\..\..\epoc32\include\partitions.h"\
	"..\..\..\..\..\epoc32\include\s32buf.h"\
	"..\..\..\..\..\epoc32\include\s32buf.inl"\
	"..\..\..\..\..\epoc32\include\s32page.h"\
	"..\..\..\..\..\epoc32\include\s32page.inl"\
	"..\..\..\..\..\epoc32\include\s32share.h"\
	"..\..\..\..\..\epoc32\include\s32share.inl"\
	"..\..\..\..\..\epoc32\include\s32std.h"\
	"..\..\..\..\..\epoc32\include\s32std.inl"\
	"..\..\..\..\..\epoc32\include\s32stor.h"\
	"..\..\..\..\..\epoc32\include\s32stor.inl"\
	"..\..\..\..\..\epoc32\include\s32strm.h"\
	"..\..\..\..\..\epoc32\include\s32strm.inl"\
	"..\..\..\..\..\epoc32\include\savenotf.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\configuration.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\eventlog.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\instrumentationhandler.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\locking.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\raiseevent.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\recordtypes.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\systemmonitor.h"\
	"..\..\..\..\..\epoc32\include\systemmonitor\systemmonitorerrors.h"\
	"..\..\..\..\..\epoc32\include\tagma.h"\
	"..\..\..\..\..\epoc32\include\techview\controls.hrh"\
	"..\..\..\..\..\epoc32\include\techview\eikbctrl.h"\
	"..\..\..\..\..\epoc32\include\techview\eikmenup.h"\
	"..\..\..\..\..\epoc32\include\techview\eikon.hrh"\
	"..\..\..\..\..\epoc32\include\techview\eiksbobs.h"\
	"..\..\..\..\..\epoc32\include\techview\techview.hrh"\
	"..\..\..\..\..\epoc32\include\test\testinstrumentation.h"\
	"..\..\..\..\..\epoc32\include\txtetext.h"\
	"..\..\..\..\..\epoc32\include\txtetext.inl"\
	"..\..\..\..\..\epoc32\include\txtfmlyr.h"\
	"..\..\..\..\..\epoc32\include\txtfmlyr.inl"\
	"..\..\..\..\..\epoc32\include\txtfmstm.h"\
	"..\..\..\..\..\epoc32\include\txtfrmat.h"\
	"..\..\..\..\..\epoc32\include\txtfrmat.inl"\
	"..\..\..\..\..\epoc32\include\txtstyle.h"\
	"..\..\..\..\..\epoc32\include\txtstyle.inl"\
	"..\..\..\..\..\epoc32\include\uikon.hrh"\
	"..\..\..\..\..\epoc32\include\vwsdef.h"\
	"..\..\..\..\..\epoc32\include\w32std.h"\
	".\Appholder.h"\
	".\Base.h"\
	".\Simple.h"\
	
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Base.cpp
DEP_CPP_BASE_=\
	"..\..\..\..\..\epoc32\include\bitbase.h"\
	"..\..\..\..\..\epoc32\include\bitdev.h"\
	"..\..\..\..\..\epoc32\include\bitdev.inl"\
	"..\..\..\..\..\epoc32\include\bitmap.h"\
	"..\..\..\..\..\epoc32\include\bitstd.h"\
	"..\..\..\..\..\epoc32\include\d32locd.h"\
	"..\..\..\..\..\epoc32\include\d32locd.inl"\
	"..\..\..\..\..\epoc32\include\e32base.h"\
	"..\..\..\..\..\epoc32\include\e32base.inl"\
	"..\..\..\..\..\epoc32\include\e32capability.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.inl"\
	"..\..\..\..\..\epoc32\include\e32const.h"\
	"..\..\..\..\..\epoc32\include\e32debug.h"\
	"..\..\..\..\..\epoc32\include\e32def.h"\
	"..\..\..\..\..\epoc32\include\e32des16.h"\
	"..\..\..\..\..\epoc32\include\e32des8.h"\
	"..\..\..\..\..\epoc32\include\e32err.h"\
	"..\..\..\..\..\epoc32\include\e32event.h"\
	"..\..\..\..\..\epoc32\include\e32hal.h"\
	"..\..\..\..\..\epoc32\include\e32keys.h"\
	"..\..\..\..\..\epoc32\include\e32ktran.h"\
	"..\..\..\..\..\epoc32\include\e32ldr.h"\
	"..\..\..\..\..\epoc32\include\e32lmsg.h"\
	"..\..\..\..\..\epoc32\include\e32notif.h"\
	"..\..\..\..\..\epoc32\include\e32std.h"\
	"..\..\..\..\..\epoc32\include\e32std.inl"\
	"..\..\..\..\..\epoc32\include\e32svr.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecom.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomerrorcodes.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.inl"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.h"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.inl"\
	"..\..\..\..\..\epoc32\include\f32file.h"\
	"..\..\..\..\..\epoc32\include\f32file.inl"\
	"..\..\..\..\..\epoc32\include\fbs.h"\
	"..\..\..\..\..\epoc32\include\FbsMessage.h"\
	"..\..\..\..\..\epoc32\include\fntstore.h"\
	"..\..\..\..\..\epoc32\include\gdi.h"\
	"..\..\..\..\..\epoc32\include\gdi.inl"\
	"..\..\..\..\..\epoc32\include\graphicsaccelerator.h"\
	"..\..\..\..\..\epoc32\include\openfont.h"\
	"..\..\..\..\..\epoc32\include\partitions.h"\
	"..\..\..\..\..\epoc32\include\s32std.h"\
	"..\..\..\..\..\epoc32\include\s32std.inl"\
	"..\..\..\..\..\epoc32\include\s32strm.h"\
	"..\..\..\..\..\epoc32\include\s32strm.inl"\
	"..\..\..\..\..\epoc32\include\w32std.h"\
	".\Base.h"\
	
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple.cpp
DEP_CPP_SIMPL=\
	"..\..\..\..\..\epoc32\include\bitbase.h"\
	"..\..\..\..\..\epoc32\include\bitdev.h"\
	"..\..\..\..\..\epoc32\include\bitdev.inl"\
	"..\..\..\..\..\epoc32\include\bitmap.h"\
	"..\..\..\..\..\epoc32\include\bitstd.h"\
	"..\..\..\..\..\epoc32\include\d32locd.h"\
	"..\..\..\..\..\epoc32\include\d32locd.inl"\
	"..\..\..\..\..\epoc32\include\e32base.h"\
	"..\..\..\..\..\epoc32\include\e32base.inl"\
	"..\..\..\..\..\epoc32\include\e32capability.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.inl"\
	"..\..\..\..\..\epoc32\include\e32const.h"\
	"..\..\..\..\..\epoc32\include\e32debug.h"\
	"..\..\..\..\..\epoc32\include\e32def.h"\
	"..\..\..\..\..\epoc32\include\e32des16.h"\
	"..\..\..\..\..\epoc32\include\e32des8.h"\
	"..\..\..\..\..\epoc32\include\e32err.h"\
	"..\..\..\..\..\epoc32\include\e32event.h"\
	"..\..\..\..\..\epoc32\include\e32hal.h"\
	"..\..\..\..\..\epoc32\include\e32keys.h"\
	"..\..\..\..\..\epoc32\include\e32ktran.h"\
	"..\..\..\..\..\epoc32\include\e32ldr.h"\
	"..\..\..\..\..\epoc32\include\e32lmsg.h"\
	"..\..\..\..\..\epoc32\include\e32notif.h"\
	"..\..\..\..\..\epoc32\include\e32std.h"\
	"..\..\..\..\..\epoc32\include\e32std.inl"\
	"..\..\..\..\..\epoc32\include\e32svr.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecom.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomerrorcodes.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.h"\
	"..\..\..\..\..\epoc32\include\ecom\ecomresolverparams.inl"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.h"\
	"..\..\..\..\..\epoc32\include\ecom\implementationinformation.inl"\
	"..\..\..\..\..\epoc32\include\f32file.h"\
	"..\..\..\..\..\epoc32\include\f32file.inl"\
	"..\..\..\..\..\epoc32\include\fbs.h"\
	"..\..\..\..\..\epoc32\include\FbsMessage.h"\
	"..\..\..\..\..\epoc32\include\fntstore.h"\
	"..\..\..\..\..\epoc32\include\gdi.h"\
	"..\..\..\..\..\epoc32\include\gdi.inl"\
	"..\..\..\..\..\epoc32\include\graphicsaccelerator.h"\
	"..\..\..\..\..\epoc32\include\openfont.h"\
	"..\..\..\..\..\epoc32\include\partitions.h"\
	"..\..\..\..\..\epoc32\include\s32std.h"\
	"..\..\..\..\..\epoc32\include\s32std.inl"\
	"..\..\..\..\..\epoc32\include\s32strm.h"\
	"..\..\..\..\..\epoc32\include\s32strm.inl"\
	"..\..\..\..\..\epoc32\include\w32std.h"\
	".\Base.h"\
	".\Simple.h"\
	
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple.mmp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple.rss

!IF  "$(CFG)" == "Simple - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple.rss
InputPath=\work\example\Graphics\WS\Simple\Simple.rss

"\epoc32\release\wins\UREL\z\Resource\Apps\Simple.r01" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UREL\z\Resource\Apps\Simple.r"

# End Custom Build

!ELSEIF  "$(CFG)" == "Simple - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple.rss
InputPath=\work\example\Graphics\WS\Simple\Simple.rss

"\epoc32\release\wins\UDEB\z\Resource\Apps\Simple.r01" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UDEB\z\Resource\Apps\Simple.r"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple.uid.cpp
DEP_CPP_SIMPLE=\
	"..\..\..\..\..\epoc32\include\e32capability.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.h"\
	"..\..\..\..\..\epoc32\include\e32cmn.inl"\
	"..\..\..\..\..\epoc32\include\e32const.h"\
	"..\..\..\..\..\epoc32\include\e32def.h"\
	"..\..\..\..\..\epoc32\include\e32des16.h"\
	"..\..\..\..\..\epoc32\include\e32des8.h"\
	"..\..\..\..\..\epoc32\include\e32err.h"\
	
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple_loc.rss

!IF  "$(CFG)" == "Simple - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple_loc.rss
InputPath=\work\example\Graphics\WS\Simple\Simple_loc.rss

"\epoc32\release\wins\UREL\Simple_loc.r01" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UREL\Simple_loc.r"

# End Custom Build

!ELSEIF  "$(CFG)" == "Simple - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple_loc.rss
InputPath=\work\example\Graphics\WS\Simple\Simple_loc.rss

"\epoc32\release\wins\UDEB\Simple_loc.r01" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UDEB\Simple_loc.r"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple_reg.rss

!IF  "$(CFG)" == "Simple - Win32 Uni Release"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple_reg.rss
InputPath=\work\example\Graphics\WS\Simple\Simple_reg.rss

"\epoc32\release\wins\UREL\z\private\10003a3f\apps\Simple_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UREL\z\private\10003a3f\apps\Simple_reg.r"

# End Custom Build

!ELSEIF  "$(CFG)" == "Simple - Win32 Uni Debug"

# PROP Intermediate_Dir "\EPOC32\BUILD\work\example\Graphics\WS\Simple\Simple\WINS"
# Begin Custom Build - Building resources from Simple_reg.rss
InputPath=\work\example\Graphics\WS\Simple\Simple_reg.rss

"\epoc32\release\wins\UDEB\z\private\10003a3f\apps\Simple_reg.rSC.dummy" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -nologo -f "\work\example\Graphics\WS\Simple\Simple.SUP.MAKE"   "\epoc32\release\wins\UDEB\z\private\10003a3f\apps\Simple_reg.r"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Appholder.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Base.h
# End Source File
# Begin Source File

SOURCE=\work\example\Graphics\WS\Simple\Simple.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\epoc32\include\w32std.h
# End Source File
# End Group
# End Target
# End Project
