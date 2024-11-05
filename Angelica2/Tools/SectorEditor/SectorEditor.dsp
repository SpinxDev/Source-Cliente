# Microsoft Developer Studio Project File - Name="SectorEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SectorEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SectorEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SectorEditor.mak" CFG="SectorEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SectorEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SectorEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/SectorEditor", ZECAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SectorEditor - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\SectorEditor.exe" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"

!ELSEIF  "$(CFG)" == "SectorEditor - Win32 Debug"

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
# ADD LINK32 zliblib.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib Angelica3D_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\SectorEditor_d.exe" /pdbtype:sept /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"

!ENDIF 

# Begin Target

# Name "SectorEditor - Win32 Release"
# Name "SectorEditor - Win32 Debug"
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

SOURCE=.\MainFrmUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SectorEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SectorEditor.h
# End Source File
# Begin Source File

SOURCE=.\SectorEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SectorEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\SectorEditorDocFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SectorEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\SectorEditorView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Utility.cpp
# End Source File
# Begin Source File

SOURCE=.\Utility.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SectorEditor.ico
# End Source File
# Begin Source File

SOURCE=.\SectorEditor.rc
# End Source File
# Begin Source File

SOURCE=.\res\SectorEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SectorEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Viewport Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WndOrthogonal.cpp
# End Source File
# Begin Source File

SOURCE=.\WndOrthogonal.h
# End Source File
# Begin Source File

SOURCE=.\WndPerspective.cpp
# End Source File
# Begin Source File

SOURCE=.\WndPerspective.h
# End Source File
# Begin Source File

SOURCE=.\WndViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\WndViewport.h
# End Source File
# Begin Source File

SOURCE=.\WndXY.cpp
# End Source File
# Begin Source File

SOURCE=.\WndXY.h
# End Source File
# Begin Source File

SOURCE=.\WndXZ.cpp
# End Source File
# Begin Source File

SOURCE=.\WndXZ.h
# End Source File
# Begin Source File

SOURCE=.\WndYZ.cpp
# End Source File
# Begin Source File

SOURCE=.\WndYZ.h
# End Source File
# End Group
# Begin Group "Data Files"

# PROP Default_Filter ""
# Begin Group "Portal Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PT_BSP.cpp
# End Source File
# Begin Source File

SOURCE=.\PT_BSP.h
# End Source File
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
# Begin Source File

SOURCE=.\EditObject.h
# End Source File
# Begin Source File

SOURCE=.\Portal.cpp
# End Source File
# Begin Source File

SOURCE=.\Portal.h
# End Source File
# Begin Source File

SOURCE=.\SceneFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneFrame.h
# End Source File
# Begin Source File

SOURCE=.\SceneTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneTexture.h
# End Source File
# Begin Source File

SOURCE=.\Sector.cpp
# End Source File
# Begin Source File

SOURCE=.\Sector.h
# End Source File
# End Group
# Begin Group "Operation Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OptBase.cpp
# End Source File
# Begin Source File

SOURCE=.\OptBase.h
# End Source File
# Begin Source File

SOURCE=.\OptCreatePortal.cpp
# End Source File
# Begin Source File

SOURCE=.\OptCreatePortal.h
# End Source File
# Begin Source File

SOURCE=.\OptCreateSector.cpp
# End Source File
# Begin Source File

SOURCE=.\OptCreateSector.h
# End Source File
# Begin Source File

SOURCE=.\OptCreateSidePortal.cpp
# End Source File
# Begin Source File

SOURCE=.\OptCreateSidePortal.h
# End Source File
# Begin Source File

SOURCE=.\OptLinkSector.cpp
# End Source File
# Begin Source File

SOURCE=.\OptLinkSector.h
# End Source File
# Begin Source File

SOURCE=.\OptMoveObject.cpp
# End Source File
# Begin Source File

SOURCE=.\OptMoveObject.h
# End Source File
# Begin Source File

SOURCE=.\OptNamePortal.cpp
# End Source File
# Begin Source File

SOURCE=.\OptNamePortal.h
# End Source File
# Begin Source File

SOURCE=.\OptSizePortal.cpp
# End Source File
# Begin Source File

SOURCE=.\OptSizePortal.h
# End Source File
# Begin Source File

SOURCE=.\OptSizeSector.cpp
# End Source File
# Begin Source File

SOURCE=.\OptSizeSector.h
# End Source File
# Begin Source File

SOURCE=.\OptSnapPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\OptSnapPortal.h
# End Source File
# Begin Source File

SOURCE=.\OptZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\OptZoom.h
# End Source File
# End Group
# Begin Group "Render Assist Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bound.cpp
# End Source File
# Begin Source File

SOURCE=.\Bound.h
# End Source File
# Begin Source File

SOURCE=.\Box3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Box3D.h
# End Source File
# Begin Source File

SOURCE=.\OrthoCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\OrthoCamera.h
# End Source File
# Begin Source File

SOURCE=.\OrthoGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\OrthoGrid.h
# End Source File
# Begin Source File

SOURCE=.\OrthoViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\OrthoViewport.h
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\WireCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\WireCollector.h
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgExportPtFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExportPtFile.h
# End Source File
# Begin Source File

SOURCE=.\DlgMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMessage.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgPortalProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPortalProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgPortalSector.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPortalSector.h
# End Source File
# End Group
# End Target
# End Project
