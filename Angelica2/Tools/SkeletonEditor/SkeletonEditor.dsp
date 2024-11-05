# Microsoft Developer Studio Project File - Name="SkeletonEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SkeletonEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SkeletonEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SkeletonEditor.mak" CFG="SkeletonEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SkeletonEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SkeletonEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/SkeletonEditor", WNCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SkeletonEditor - Win32 Release"

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
# ADD LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib Angelica3D.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\SkeletonEditor.exe" /libpath:".\\" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "SkeletonEditor - Win32 Debug"

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
# ADD LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib Angelica3D_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\SkeletonEditor_d.exe" /pdbtype:sept /libpath:".\\" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"

!ENDIF 

# Begin Target

# Name "SkeletonEditor - Win32 Release"
# Name "SkeletonEditor - Win32 Debug"
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

SOURCE=.\SkeletonEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditor.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditor.rc
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonEditorView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Utility Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Box3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Box3D.h
# End Source File
# Begin Source File

SOURCE=.\Hook.cpp
# End Source File
# Begin Source File

SOURCE=.\Hook.h
# End Source File
# Begin Source File

SOURCE=.\InterfaceWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfaceWrapper.h
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# Begin Source File

SOURCE=.\ModelConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Staff.cpp
# End Source File
# Begin Source File

SOURCE=.\Staff.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SkeletonEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\SkeletonEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SkeletonEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Warning.ICO
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgActionProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgActionProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddHanger.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAddHanger.h
# End Source File
# Begin Source File

SOURCE=.\DlgAddSkin.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAddSkin.h
# End Source File
# Begin Source File

SOURCE=.\DlgMergeSkeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMergeSkeleton.h
# End Source File
# Begin Source File

SOURCE=.\DlgModelInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgModelInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgModelProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgModelProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgOption.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOption.h
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.h
# End Source File
# Begin Source File

SOURCE=.\PageActionCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\PageActionCombo.h
# End Source File
# Begin Source File

SOURCE=.\PageEditAction.cpp
# End Source File
# Begin Source File

SOURCE=.\PageEditAction.h
# End Source File
# Begin Source File

SOURCE=.\PageEditHooks.cpp
# End Source File
# Begin Source File

SOURCE=.\PageEditHooks.h
# End Source File
# Begin Source File

SOURCE=.\PageScaleBone.cpp
# End Source File
# Begin Source File

SOURCE=.\PageScaleBone.h
# End Source File
# Begin Source File

SOURCE=.\PageSuppleTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSuppleTest.h
# End Source File
# Begin Source File

SOURCE=.\ToolTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\WndCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\WndCombo.h
# End Source File
# Begin Source File

SOURCE=.\WndInput.cpp
# End Source File
# Begin Source File

SOURCE=.\WndInput.h
# End Source File
# End Group
# End Target
# End Project
