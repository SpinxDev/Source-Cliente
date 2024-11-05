# Microsoft Developer Studio Project File - Name="Terrain2View" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Terrain2View - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Terrain2View.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Terrain2View.mak" CFG="Terrain2View - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Terrain2View - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Terrain2View - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/Terrain2View", SEHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Terrain2View - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib AngelicaFile.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Terrain2View - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib Angelica3D_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Terrain2View - Win32 Release"
# Name "Terrain2View - Win32 Debug"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Terrain2View.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain2View.h
# End Source File
# Begin Source File

SOURCE=.\Terrain2ViewDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain2ViewDoc.h
# End Source File
# Begin Source File

SOURCE=.\Terrain2ViewView.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain2ViewView.h
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DlgBuildHoriCull.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBuildHoriCull.h
# End Source File
# Begin Source File

SOURCE=.\DlgInputFlot.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInputFlot.h
# End Source File
# Begin Source File

SOURCE=.\DlgPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPosition.h
# End Source File
# Begin Source File

SOURCE=.\DlgTerrainInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTerrainInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgTerrainLOD.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTerrainLOD.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Terrain2View.ico
# End Source File
# Begin Source File

SOURCE=.\Terrain2View.rc
# End Source File
# Begin Source File

SOURCE=.\res\Terrain2View.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Terrain2View_TB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Terrain2ViewDoc.ico
# End Source File
# End Group
# Begin Group "Algorithm Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\A3DTerrain2Wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2Wrapper.h
# End Source File
# Begin Source File

SOURCE=.\TerrainSquare.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainSquare.h
# End Source File
# End Group
# End Target
# End Project
