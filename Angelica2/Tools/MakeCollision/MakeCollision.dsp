# Microsoft Developer Studio Project File - Name="MakeCollision" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MakeCollision - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MakeCollision.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeCollision.mak" CFG="MakeCollision - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeCollision - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MakeCollision - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/MakeCollision", CJCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeCollision - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\MakeCollision.exe" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"

!ELSEIF  "$(CFG)" == "MakeCollision - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zliblib.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib Angelica3D_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\MakeCollision_d.exe" /pdbtype:sept /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"

!ENDIF 

# Begin Target

# Name "MakeCollision - Win32 Release"
# Name "MakeCollision - Win32 Debug"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DlgBuildESP.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBuildESP.h
# End Source File
# Begin Source File

SOURCE=.\DlgBuildKDT.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBuildKDT.h
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\MakeCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeCollision.h
# End Source File
# Begin Source File

SOURCE=.\MakeCollision.rc
# End Source File
# Begin Source File

SOURCE=.\MakeCollisionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeCollisionDlg.h
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MakeCollision.ico
# End Source File
# Begin Source File

SOURCE=.\res\MakeCollision.rc2
# End Source File
# End Group
# Begin Group "KD-Tree Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KDT_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\KDT_Main.h
# End Source File
# Begin Source File

SOURCE=.\KDT_Types.h
# End Source File
# Begin Source File

SOURCE=.\KDT_Write.cpp
# End Source File
# Begin Source File

SOURCE=.\KDT_Write.h
# End Source File
# End Group
# Begin Group "ESP Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ESP_Leaf.cpp
# End Source File
# Begin Source File

SOURCE=.\ESP_Leaf.h
# End Source File
# Begin Source File

SOURCE=.\ESP_Types.h
# End Source File
# Begin Source File

SOURCE=.\ESP_Write.cpp
# End Source File
# Begin Source File

SOURCE=.\ESP_Write.h
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Brush.cpp
# End Source File
# Begin Source File

SOURCE=.\Brush.h
# End Source File
# Begin Source File

SOURCE=.\MathLib.cpp
# End Source File
# Begin Source File

SOURCE=.\MathLib.h
# End Source File
# Begin Source File

SOURCE=.\Plane.cpp
# End Source File
# Begin Source File

SOURCE=.\Plane.h
# End Source File
# Begin Source File

SOURCE=.\Side.cpp
# End Source File
# Begin Source File

SOURCE=.\Side.h
# End Source File
# Begin Source File

SOURCE=.\Types.h
# End Source File
# Begin Source File

SOURCE=.\Winding.cpp
# End Source File
# Begin Source File

SOURCE=.\Winding.h
# End Source File
# End Group
# End Target
# End Project
