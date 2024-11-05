# Microsoft Developer Studio Project File - Name="GfxCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GfxCommon - Win32 PhyDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GfxCommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GfxCommon.mak" CFG="GfxCommon - Win32 PhyDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GfxCommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 DbgRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 Mem_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 EditorDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 EditorRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 EditorDebugForbidNewDel" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 EditorReleaseForbidNewDel" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 UnicodeEditorDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 UnicodeEditorRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 LanEn_EditorDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 LanEn_EditorRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 PhyDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 PhyRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon - Win32 PhyDebugRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GfxCommon", XADAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GfxCommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "F:\SDK\LuaWrapper\Include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\*.lib Output\Lib	xcopy /y *.h Output\Include
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\GfxCommon_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_DbgRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_ELEMENTCLIENT" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"DbgRelease\GfxCommon_su.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy DbgRelease\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 Mem_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_Mem_Debug"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_Mem_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Mem_Debug"
# PROP Intermediate_Dir "Mem_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_ELEMENTCLIENT" /D "UNICODE" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_ELEMENTCLIENT" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\GfxCommon_d.lib"
# ADD LIB32 /nologo /out:"Mem_Debug\GfxCommon_smd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy             mem_debug\*.lib             ..\lib	copy             mem_debug\*.lib              Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 EditorDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_EditorDebug"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_EditorDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EditorDebug"
# PROP Intermediate_Dir "EditorDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_ELEMENTCLIENT" /D "UNICODE" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\luawrapperexport\include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "A3D_PHYSX" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\GfxCommon_d.lib"
# ADD LIB32 /nologo /out:"EditorDebug\GfxEditor_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy EditorDebug\*.lib Output\Lib	copy *.h ..\APhysXIntegration\Extra_Include	copy EditorDebug\*.lib ..\APhysXIntegration\Extra_Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 EditorRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_EditorRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_EditorRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EditorRelease"
# PROP Intermediate_Dir "EditorRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_ELEMENTCLIENT" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\luawrapperexport\include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "A3D_PHYSX" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"EditorRelease\GfxEditor.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy EditorRelease\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 EditorDebugForbidNewDel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_EditorDebugForbidNewDel"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_EditorDebugForbidNewDel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EditorDebugForbidNewDel"
# PROP Intermediate_Dir "EditorDebugForbidNewDel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "A3D_PHYSX" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorDebug\GfxEditor_d.lib"
# ADD LIB32 /nologo /out:"EditorDebugForbidNewDel\GfxEditor_sd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy EditorDebugForbidNewDel\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 EditorReleaseForbidNewDel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_EditorReleaseForbidNewDel"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_EditorReleaseForbidNewDel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "EditorReleaseForbidNewDel"
# PROP Intermediate_Dir "EditorReleaseForbidNewDel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "A3D_PHYSX" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorRelease\GfxEditor.lib"
# ADD LIB32 /nologo /out:"EditorReleaseForbidNewDel\GfxEditor_s.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy EditorReleaseForbidNewDel\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 UnicodeEditorDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_UnicodeEditorDebug"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_UnicodeEditorDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UnicodeEditorDebug"
# PROP Intermediate_Dir "UnicodeEditorDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "UNICODE" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorDebug\GfxEditor_d.lib"
# ADD LIB32 /nologo /out:"UnicodeEditorDebug\GfxEditor_ud.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy UnicodeEditorDebug\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 UnicodeEditorRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_UnicodeEditorRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_UnicodeEditorRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UnicodeEditorRelease"
# PROP Intermediate_Dir "UnicodeEditorRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "UNICODE" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorRelease\GfxEditor.lib"
# ADD LIB32 /nologo /out:"UnicodeEditorRelease\GfxEditor_u.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy UnicodeEditorRelease\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 LanEn_EditorDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_LanEn_EditorDebug"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_LanEn_EditorDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LanEn_EditorDebug"
# PROP Intermediate_Dir "LanEn_EditorDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LuaWrapper\Include" /I ".\Include" /I "..\luawrapperexport\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\luawrapperexport\include" /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "_DEBUG" /D "_GFX_LANG_EN" /D "WIN32" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "A3D_PHYSX" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorDebug\GfxEditor_d.lib"
# ADD LIB32 /nologo /out:"LanEn_EditorDebug\GfxEditor_dEN.lib"

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 LanEn_EditorRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_LanEn_EditorRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_LanEn_EditorRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LanEn_EditorRelease"
# PROP Intermediate_Dir "LanEn_EditorRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\LuaWrapper\Include" /I ".\Include" /I "..\LuaWrapperExport\Include" /D "NDEBUG" /D "_GFX_LANG_EN" /D "WIN32" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "A3D_PHYSX" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"EditorRelease\GfxEditor.lib"
# ADD LIB32 /nologo /out:"LanEn_EditorRelease\GfxEditorEN.lib"

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 PhyDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon___Win32_PhyDebug"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_PhyDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PhyDebug"
# PROP Intermediate_Dir "PhyDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\GfxCommon_d.lib"
# ADD LIB32 /nologo /out:"PhyDebug\GfxCommonPhy_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy PhyDebug\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 PhyRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_PhyRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_PhyRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PhyRelease"
# PROP Intermediate_Dir "PhyRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\LuaWrapper\Include" /I "F:\SDK\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PhyRelease\GfxCommonPhy.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy PhyRelease\*.lib Output\Lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GfxCommon - Win32 PhyDebugRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon___Win32_PhyDebugRelease"
# PROP BASE Intermediate_Dir "GfxCommon___Win32_PhyDebugRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PhyDebugRelease"
# PROP Intermediate_Dir "PhyDebugRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\LuaWrapper\Include" /I "F:\SDK\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /I ".\Include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"PhyRelease\GfxCommonPhy.lib"
# ADD LIB32 /nologo /out:"PhyDebugRelease\GfxCommonPhy_s.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy PhyDebugRelease\*.lib Output\Lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "GfxCommon - Win32 Release"
# Name "GfxCommon - Win32 Debug"
# Name "GfxCommon - Win32 DbgRelease"
# Name "GfxCommon - Win32 Mem_Debug"
# Name "GfxCommon - Win32 EditorDebug"
# Name "GfxCommon - Win32 EditorRelease"
# Name "GfxCommon - Win32 EditorDebugForbidNewDel"
# Name "GfxCommon - Win32 EditorReleaseForbidNewDel"
# Name "GfxCommon - Win32 UnicodeEditorDebug"
# Name "GfxCommon - Win32 UnicodeEditorRelease"
# Name "GfxCommon - Win32 LanEn_EditorDebug"
# Name "GfxCommon - Win32 LanEn_EditorRelease"
# Name "GfxCommon - Win32 PhyDebug"
# Name "GfxCommon - Win32 PhyRelease"
# Name "GfxCommon - Win32 PhyDebugRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\A3DAreaEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DBoxEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DCombActMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DCombinedAction.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DCurveEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DCylinderEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DDecalEx.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DEllipsoidEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXECMLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXECModel.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXEditorInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXElement.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXEx.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXExMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXKeyPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLight.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLightning.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLightningEx.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXModel.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXModelMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGfxPhysDataMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXPIFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXRenderSlot.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXRing.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXShakeCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSound.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSoundImp.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXStreamMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSurfaceRender.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXTexMan.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGridDecalEx.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DLtnBolt.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DLtnTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysics.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysSync.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysSyncData.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DMultiPlaneEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DParaboloid.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DParticleEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DParticleSystemEx.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysActor.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysElement.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysFluidEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysFluidSyncData.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysPointEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPhysRBCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DPointEmitter.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DRibbon.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DSkillGfxComposer.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DSkillGfxEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DSkinPhysSync.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DSkinPhysSyncData.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CameraKeyPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ModelHook.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ModelLua.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ModelMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ModelShell.cpp
# End Source File
# Begin Source File

SOURCE=.\FX_BINDING.CPP
# End Source File
# Begin Source File

SOURCE=.\GfxEditorUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\RandStringContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\TrailList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\A3DAreaEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DBoxEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DCombActMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DCombinedAction.h
# End Source File
# Begin Source File

SOURCE=.\A3DCurveEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DCylinderEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DDecalEx.h
# End Source File
# Begin Source File

SOURCE=.\A3DEllipsoidEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXAnimable.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXAnimable.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXCachedItemMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXContainer.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXECMInterface.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXECMLoader.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXECModel.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXEditorInterface.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXElement.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXEx.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXExMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXFloatValueTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXFloatValueTrans.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXFuncs.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXGridAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DGFXGridAnimation.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXInterface.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXKeyPoint.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLight.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLightning.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXLightningEx.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXModel.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXModelMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXPhysDataMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXPIFuncs.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXRenderSlot.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXRing.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXShakeCamera.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSound.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSoundImp.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXStreamMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXSurfaceRender.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXTexMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGridDecalEx.h
# End Source File
# Begin Source File

SOURCE=.\A3DLtnBolt.h
# End Source File
# Begin Source File

SOURCE=.\A3DLtnTrail.h
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysics.h
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysSync.h
# End Source File
# Begin Source File

SOURCE=.\A3DModelPhysSyncData.h
# End Source File
# Begin Source File

SOURCE=.\A3DMultiPlaneEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DParaboloid.h
# End Source File
# Begin Source File

SOURCE=.\A3DParticle.h
# End Source File
# Begin Source File

SOURCE=.\A3DParticleEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DParticleSystemEx.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysActor.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysElement.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysFluidEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysFluidSyncData.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysPointEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DPhysRBCreator.h
# End Source File
# Begin Source File

SOURCE=.\A3DPointEmitter.h
# End Source File
# Begin Source File

SOURCE=.\A3DRibbon.h
# End Source File
# Begin Source File

SOURCE=.\A3DSkillGfxComposer.h
# End Source File
# Begin Source File

SOURCE=.\A3DSkillGfxEvent.h
# End Source File
# Begin Source File

SOURCE=.\A3DSkinPhysSync.h
# End Source File
# Begin Source File

SOURCE=.\A3DSkinPhysSyncData.h
# End Source File
# Begin Source File

SOURCE=.\A3DTrail.h
# End Source File
# Begin Source File

SOURCE=.\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\EC_CameraKeyPoint.h
# End Source File
# Begin Source File

SOURCE=.\EC_Model.h
# End Source File
# Begin Source File

SOURCE=.\EC_ModelHook.h
# End Source File
# Begin Source File

SOURCE=.\EC_ModelLua.h
# End Source File
# Begin Source File

SOURCE=.\EC_ModelMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_ModelShell.h
# End Source File
# Begin Source File

SOURCE=.\FX_BINDING.H
# End Source File
# Begin Source File

SOURCE=.\GfxCommonHeaders.h
# End Source File
# Begin Source File

SOURCE=.\GfxCommonTypes.h
# End Source File
# Begin Source File

SOURCE=.\GfxEditorUtil.h
# End Source File
# Begin Source File

SOURCE=.\RandStringContainer.h
# End Source File
# Begin Source File

SOURCE=.\RotList.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TrailList.h
# End Source File
# End Group
# End Target
# End Project
