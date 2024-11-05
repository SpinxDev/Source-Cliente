# Microsoft Developer Studio Project File - Name="PhysDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PhysDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PhysDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PhysDemo.mak" CFG="PhysDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PhysDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PhysDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Samples/PhysDemo", JIHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PhysDemo - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Include\Gfx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "GFX_EDITOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 GfxEditor.lib CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib AngelicaFile.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\PhysDemo.exe" /libpath:".\Lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "PhysDemo - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Include\Gfx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "GFX_EDITOR" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 GfxEditor_d.lib CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib Angelica3D_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\PhysDemo_d.exe" /pdbtype:sept /libpath:".\Lib"

!ENDIF 

# Begin Target

# Name "PhysDemo - Win32 Release"
# Name "PhysDemo - Win32 Debug"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Game.h
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceWrapper.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\PhysDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysDemo.h
# End Source File
# Begin Source File

SOURCE=.\PhysDemoDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysDemoDoc.h
# End Source File
# Begin Source File

SOURCE=.\PhysDemoView.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysDemoView.h
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

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Object Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\PhysModelObject.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysModelObject.h
# End Source File
# Begin Source File

SOURCE=.\PhysObject.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysObject.h
# End Source File
# Begin Source File

SOURCE=.\PhysObjectMan.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysObjectMan.h
# End Source File
# Begin Source File

SOURCE=.\PhysStaticObject.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysStaticObject.h
# End Source File
# Begin Source File

SOURCE=.\PhysTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysTerrain.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\PhysDemo.ico
# End Source File
# Begin Source File

SOURCE=.\PhysDemo.rc
# End Source File
# Begin Source File

SOURCE=.\res\PhysDemo.rc2
# End Source File
# Begin Source File

SOURCE=.\res\PhysDemo_TB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PhysDemoDoc.ico
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgPhysXTrnGen.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPhysXTrnGen.h
# End Source File
# End Group
# End Target
# End Project
