# Microsoft Developer Studio Project File - Name="AutoPFImp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AutoPFImp - Win32 EditerDebugCRT
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoPFImp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoPFImp.mak" CFG="AutoPFImp - Win32 EditerDebugCRT"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoPFImp - Win32 EditerDebug" (based on "Win32 (x86) Application")
!MESSAGE "AutoPFImp - Win32 EditorRelease" (based on "Win32 (x86) Application")
!MESSAGE "AutoPFImp - Win32 EditerDebugCRT" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AutoPFImp", SPQAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoPFImp - Win32 EditerDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AutoPFImp___Win32_EditerDebug"
# PROP BASE Intermediate_Dir "AutoPFImp___Win32_EditerDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AUTOMOVE_EDITOR" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AUTOMOVE_EDITOR" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AngelicaMedia_d.lib AngelicaCommon_d.lib AngelicaFile_d.lib zliblib.lib Angelica3D_d.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper_d.lib dsound.lib dinput8.lib dxguid.lib mpg123lib_d.lib AutoMoveImpEditor_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib shlwapi.lib dbghelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"./bin/AutoPFImp_d.exe" /pdbtype:sept /libpath:".\lib"
# ADD LINK32 AngelicaMedia_d.lib AngelicaCommon_d.lib AngelicaFile_d.lib zliblib.lib Angelica3D_d.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper_d.lib dsound.lib dinput8.lib dxguid.lib mpg123lib_d.lib AutoMoveImpEditor_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib shlwapi.lib dbghelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"../CBin/AutoPFImp_d.exe" /pdbtype:sept /libpath:".\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase"

!ELSEIF  "$(CFG)" == "AutoPFImp - Win32 EditorRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AutoPFImp___Win32_EditorRelease"
# PROP BASE Intermediate_Dir "AutoPFImp___Win32_EditorRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AUTOMOVE_EDITOR" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AUTOMOVE_EDITOR" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AngelicaMedia.lib AngelicaCommon.lib AngelicaFile.lib zliblib.lib Angelica3D.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper.lib dsound.lib dinput8.lib dxguid.lib mpg123lib.lib AutoMoveImpEditor.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib /nologo /subsystem:windows /machine:I386 /out:"./bin/AutoPFImp.exe" /libpath:"./lib"
# ADD LINK32 AngelicaMedia.lib AngelicaCommon.lib AngelicaFile.lib zliblib.lib Angelica3D.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper.lib dsound.lib dinput8.lib dxguid.lib mpg123lib.lib AutoMoveImpEditor.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /subsystem:windows /machine:I386 /out:"../CBin/AutoPFImp.exe" /libpath:"./lib" /libpath:".\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase"

!ELSEIF  "$(CFG)" == "AutoPFImp - Win32 EditerDebugCRT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AutoPFImp___Win32_EditerDebugCRT"
# PROP BASE Intermediate_Dir "AutoPFImp___Win32_EditerDebugCRT"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EditerDebugCRT"
# PROP Intermediate_Dir "EditerDebugCRT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\SDK\3rdSDK\include" /I "..\SDK\A3DSDK\Include" /I "..\SDK\acsdk\Include" /I "..\SDK\afsdk\include" /I "..\SDK\Amsdk\include" /I "..\SDK\Apsdk\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AUTOMOVE_EDITOR" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_AUTOMOVE_EDITOR" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "UNICODE" /D "_UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AngelicaMedia_d.lib AngelicaCommon_d.lib AngelicaFile_d.lib zliblib.lib Angelica3D_d.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper_d.lib dsound.lib dinput8.lib dxguid.lib mpg123lib_d.lib AutoMoveImpEditor_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib shlwapi.lib dbghelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"./bin/AutoPFImp_d.exe" /pdbtype:sept /libpath:".\lib" /libpath:"..\SDK\3rdSDK\lib" /libpath:"..\SDK\A3DSDK\Lib" /libpath:"..\SDK\acsdk\lib" /libpath:"..\SDK\afsdk\lib" /libpath:"..\SDK\Amsdk\lib" /libpath:"..\SDK\Apsdk\Lib" /libpath:"..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\SDK\3rdSDK\lib\dshowbase"
# ADD LINK32 AngelicaMedia_ud.lib AngelicaCommon_ud.lib AngelicaFile_ud.lib zliblib.lib Angelica3D_ud.lib d3d8.lib d3dx8.lib Imm32.lib Immwrapper_d.lib dsound.lib dinput8.lib dxguid.lib mpg123lib_d.lib AutoMoveImpEditor_ud.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib shlwapi.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRTd" /out:"../CBin/AutoPFImp_ud.exe" /pdbtype:sept /libpath:".\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase"

!ENDIF 

# Begin Target

# Name "AutoPFImp - Win32 EditerDebug"
# Name "AutoPFImp - Win32 EditorRelease"
# Name "AutoPFImp - Win32 EditerDebugCRT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoPFImp.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoPFImp.rc
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\CWButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CWControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CWTabctrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\FormClu.cpp
# End Source File
# Begin Source File

SOURCE=.\FormIsland.cpp
# End Source File
# Begin Source File

SOURCE=.\FormMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\GridRender.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoPFImp.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\CWButton.h
# End Source File
# Begin Source File

SOURCE=.\CWControlBar.h
# End Source File
# Begin Source File

SOURCE=.\CWTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=.\FormClu.h
# End Source File
# Begin Source File

SOURCE=.\FormIsland.h
# End Source File
# Begin Source File

SOURCE=.\FormMap.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\GridRender.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AutoPFImp.ico
# End Source File
# Begin Source File

SOURCE=.\res\AutoPFImp.rc2
# End Source File
# Begin Source File

SOURCE=.\res\imagelist.bmp
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
