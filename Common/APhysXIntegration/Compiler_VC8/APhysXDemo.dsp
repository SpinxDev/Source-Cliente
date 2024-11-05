# Microsoft Developer Studio Project File - Name="APhysXDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=APhysXDemo - Win32 DebugStaticLib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APhysXDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APhysXDemo.mak" CFG="APhysXDemo - Win32 DebugStaticLib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APhysXDemo - Win32 ReleaseStaticLib" (based on "Win32 (x86) Application")
!MESSAGE "APhysXDemo - Win32 DebugStaticLib" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Samples/APhysXDemo", JIHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APhysXDemo - Win32 ReleaseStaticLib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXDemo\ReleaseStaticLib"
# PROP Intermediate_Dir "Build\APhysXDemo\ReleaseStaticLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\GfxCommon" /I "..\APhysXEngine\Include" /I "..\APhysXDemo\Include\APhysX" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib AngelicaFile.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib GfxEditor.lib /nologo /subsystem:windows /machine:I386 /out:"..\Bin\APhysXDemo_VC6.exe" /libpath:"..\..\GfxCommon\EditorRelease" /libpath:"..\Lib_VC6" /libpath:"..\APhysXDemo\Lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "APhysXDemo - Win32 DebugStaticLib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\APhysXDemo\DebugStaticLib"
# PROP Intermediate_Dir "Build\APhysXDemo\DebugStaticLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\GfxCommon" /I "..\APhysXEngine\Include" /I "..\APhysXDemo\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib Angelica3D_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib GfxEditor_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Bin\APhysXDemo_VC6_d.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\EditorDebug" /libpath:"..\Lib_VC6" /libpath:"..\APhysXDemo\Lib"

!ENDIF 

# Begin Target

# Name "APhysXDemo - Win32 ReleaseStaticLib"
# Name "APhysXDemo - Win32 DebugStaticLib"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\APhysXDemo\Game.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Game.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Global.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Global.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\InterfaceWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\InterfaceWrapper.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemo.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemoDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemoView.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Render.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Render.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\Resource.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\StdAfx.h
# End Source File
# End Group
# Begin Group "Object Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\APhysXDemo\ClothSkinConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\ClothSkinConfig.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\CoordinateDirection.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\CoordinateDirection.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemo.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemoDoc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemoView.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysModelObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysModelObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysObjectMan.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysObjectMan.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysParticleObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysParticleObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysRayTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysRayTrace.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysStaticObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysStaticObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysTerrain.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysTerrain.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\APhysXDemo\res\Edit.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\PhysDemo.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\PhysDemo.rc
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\PhysDemo.rc2
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\PhysDemo_TB.bmp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\PhysDemoDoc.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\Play.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\rotation.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\res\Simulate.ico
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXDemo\DlgPhysXTrnGen.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXDemo\DlgPhysXTrnGen.h
# End Source File
# End Group
# End Target
# End Project
