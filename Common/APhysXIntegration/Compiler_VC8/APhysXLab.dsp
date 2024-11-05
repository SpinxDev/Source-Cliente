# Microsoft Developer Studio Project File - Name="APhysXLab" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=APhysXLab - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APhysXLab.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APhysXLab.mak" CFG="APhysXLab - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APhysXLab - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "APhysXLab - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "APhysXLab - Win32 Debug ForbidAngelicaNewDelete" (based on "Win32 (x86) Application")
!MESSAGE "APhysXLab - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "APhysXLab - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "APhysXLab - Win32 Release_NoDebugRender" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/APhysXIntegration/Compiler_VC6", ATPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build/APhysXLab/Release"
# PROP Intermediate_Dir "Build/APhysXLab/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaFile.lib Angelica3D.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_v.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy.lib shlwapi.lib LuaWrapper.lib lua5.1.mtd.lib CHBasedCD.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /subsystem:windows /machine:I386 /out:"../Bin/APhysXLab.exe" /libpath:"..\..\GfxCommon\PhyRelease" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build/APhysXLab/Debug"
# PROP Intermediate_Dir "Build/APhysXLab/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib AngelicaFile_d.lib Angelica3D_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy_d.lib shlwapi.lib LuaWrapper_d.lib lua5.1.mtd.lib CHBasedCD_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Bin/APhysXLab_d.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\PhyDebug" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APhysXLab___Win32_Debug_ForbidAngelicaNewDelete"
# PROP BASE Intermediate_Dir "APhysXLab___Win32_Debug_ForbidAngelicaNewDelete"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build/APhysXLab/Debug_ForbidAngelicaNewDelete"
# PROP Intermediate_Dir "Build/APhysXLab/Debug_ForbidAngelicaNewDelete"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXLab\Common" /I "..\..\GfxCommon" /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_sd.lib AngelicaFile_sd.lib Angelica3D_sd.lib AngelicaMedia_sd.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_sd.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib GfxEditor_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Bin/APhysXLab_d.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\EditorDebug" /libpath:"..\Lib_VC6"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_sd.lib AngelicaFile_sd.lib Angelica3D_sd.lib AngelicaMedia_sd.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_sd.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib GfxEditor_sd.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Bin/APhysXLab_d.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\EditorDebugForbidNewDel" /libpath:"..\Lib_VC6" /libpath:"..\Extra_Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APhysXLab___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "APhysXLab___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build/APhysXLab/Unicode_Debug"
# PROP Intermediate_Dir "Build/APhysXLab/Unicode_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CommonFile_d.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_d.lib AngelicaFile_d.lib Angelica3D_d.lib AngelicaMedia_d.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy_d.lib shlwapi.lib LuaWrapper_d.lib lua5.1.mtd.lib CHBasedCD_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Bin/APhysXLab_d.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\PhyDebug" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 CommonFile_ud.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib AngelicaMedia_ud.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_ud.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy_d.lib shlwapi.lib LuaWrapper_du.lib lua5.1.mtd.lib CHBasedCD_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../Bin/APhysXLab_ud.exe" /pdbtype:sept /libpath:"..\..\GfxCommon\PhyDebug" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXLab___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "APhysXLab___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build/APhysXLab/Unicode_Release"
# PROP Intermediate_Dir "Build/APhysXLab/Unicode_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaFile.lib Angelica3D.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy.lib shlwapi.lib LuaWrapper.lib lua5.1.mtd.lib CHBasedCD.lib /nologo /subsystem:windows /machine:I386 /out:"../Bin/APhysXLab.exe" /libpath:"..\..\GfxCommon\PhyRelease" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"
# ADD LINK32 CommonFile_u.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaFile_u.lib Angelica3D_u.lib AngelicaMedia_u.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_uv.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy.lib shlwapi.lib LuaWrapper_u.lib lua5.1.mtd.lib CHBasedCD.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../Bin/APhysXLab_u.exe" /libpath:"..\..\GfxCommon\PhyRelease" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXLab___Win32_Release_NoDebugRender"
# PROP BASE Intermediate_Dir "APhysXLab___Win32_Release_NoDebugRender"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build/APhysXLab/Release_NoDebugRender"
# PROP Intermediate_Dir "Build/APhysXLab/Release_NoDebugRender"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\APhysXLab\AppFrame" /I "..\APhysXLab\BrushCollision" /I "..\APhysXLab\Common" /I "..\APhysXLab\Test" /I "..\..\GfxCommon" /I "..\..\GfxCommon\Include" /I "..\APhysXEngine\Include" /I "..\..\LuaWrapperExport\Include" /I "..\..\LuaWrapper\Include" /I "..\Extra_Include" /I "..\Extra_Include\cd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaFile.lib Angelica3D.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic_v.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy.lib shlwapi.lib LuaWrapper.lib lua5.1.mtd.lib CHBasedCD.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /subsystem:windows /machine:I386 /out:"../Bin/APhysXLab.exe" /libpath:"..\..\GfxCommon\PhyRelease" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"
# ADD LINK32 CommonFile.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaFile.lib Angelica3D.lib AngelicaMedia.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib GfxCommonPhy.lib shlwapi.lib LuaWrapper.lib lua5.1.mtd.lib CHBasedCD.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /subsystem:windows /machine:I386 /out:"../Bin/APhysXLab_NoDebugRender.exe" /libpath:"..\..\GfxCommon\PhyRelease" /libpath:"..\Lib_VC6" /libpath:"..\..\LuaWrapperExport\Lib" /libpath:"..\..\LuaWrapper\Lib" /libpath:"..\Extra_Lib"

!ENDIF 

# Begin Target

# Name "APhysXLab - Win32 Release"
# Name "APhysXLab - Win32 Debug"
# Name "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"
# Name "APhysXLab - Win32 Unicode Debug"
# Name "APhysXLab - Win32 Unicode Release"
# Name "APhysXLab - Win32 Release_NoDebugRender"
# Begin Group "AppFrame"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLab.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLab.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLab.rc
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLabCoreHeaders.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLabDoc.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLabDoc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLabView.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\APhysXLabView.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgFFEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgFFEdit.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgFlags.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgFlags.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgPhysXTrnGen.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\DlgPhysXTrnGen.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\MainFrm.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\Resource.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\StdAfx.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\APhysXLab\Common\ActionPlayerBase.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ActionPlayerBase.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ALogWrapper.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ALogWrapper.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ApxAttack.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ApxAttack.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ApxSort.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ApxSort.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\base64.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\base64.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\CameraController.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\CameraController.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ClothSkinConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ClothSkinConfig.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\CoordinateDirection.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\CoordinateDirection.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\Game.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\Game.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\GfxCommonInterfacesImp.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\GraphicsSyncRB.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\MainActor.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\MainActor.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjBase.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjBase.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjDynamic.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjDynamic.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjECModel.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjECModel.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjForceField.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjForceField.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjMgr.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjMgr.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjParticle.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjParticle.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjSkinModel.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjSkinModel.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjStatic.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\PhysXObjStatic.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\RayTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\RayTrace.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\Render.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\Render.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ShadowRender.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\ShadowRender.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\SubThread.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\SubThread.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Common\SubThreadInterface.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\res\APhysXLab.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\res\APhysXLab.rc2
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\res\APhysXLabDoc.ico
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXLab\Test\CommonMovement.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\CommonMovement.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\OverlapBounceDamping.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\OverlapBounceDamping.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\TConvexMesh.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\TConvexMesh.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\TScenario.cpp

!IF  "$(CFG)" == "APhysXLab - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Debug ForbidAngelicaNewDelete"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Unicode Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "APhysXLab - Win32 Release_NoDebugRender"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APhysXLab\Test\TScenario.h
# End Source File
# End Group
# Begin Group "BrushCollision"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushcharCtrler.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushcharCtrler.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushesObj.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushesObj.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushesObjMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushesObjMgr.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushObjAttr.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\BrushObjAttr.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\Collision.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\Collision.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\EC_BrushMan.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\EC_BrushMan.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\EC_BrushManUtil.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\ECCDR.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\ECCDR.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\ELBrushBuilding.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\ELBrushBuilding.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\PhysXObjChb.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\PhysXObjChb.h
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\SceneExtraConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\BrushCollision\SceneExtraConfig.h
# End Source File
# End Group
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\LIMITS"
# End Source File
# Begin Source File

SOURCE=..\APhysXLab\AppFrame\AppFrame\ReadMe.txt
# End Source File
# End Target
# End Project
