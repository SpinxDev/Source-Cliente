# Microsoft Developer Studio Project File - Name="BuildPortal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BuildPortal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BuildPortal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BuildPortal.mak" CFG="BuildPortal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BuildPortal - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BuildPortal - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/BuildPortal", LDCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BuildPortal - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 zliblib.lib mpg123lib.lib Immwrapper.lib ACommon.lib Angelica3D.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\BuildPortal.exe" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"

!ELSEIF  "$(CFG)" == "BuildPortal - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zliblib.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib Angelica3D_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\BuildPortal_d.exe" /pdbtype:sept /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"

!ENDIF 

# Begin Target

# Name "BuildPortal - Win32 Release"
# Name "BuildPortal - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BuildPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildPortal.rc
# End Source File
# Begin Source File

SOURCE=.\BuildPortalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BuildPortal.h
# End Source File
# Begin Source File

SOURCE=.\BuildPortalDlg.h
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

SOURCE=.\res\BuildPortal.ico
# End Source File
# Begin Source File

SOURCE=.\res\BuildPortal.rc2
# End Source File
# End Group
# Begin Group "Portal Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PT_Interface.h
# End Source File
# Begin Source File

SOURCE=.\PT_Internal.h
# End Source File
# Begin Source File

SOURCE=.\PT_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Math.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Math.h
# End Source File
# Begin Source File

SOURCE=.\PT_Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Plane.h
# End Source File
# Begin Source File

SOURCE=.\PT_Sector.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Sector.h
# End Source File
# Begin Source File

SOURCE=.\PT_Winding.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Winding.h
# End Source File
# Begin Source File

SOURCE=.\PT_Write.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_Write.h
# End Source File
# End Group
# End Target
# End Project
