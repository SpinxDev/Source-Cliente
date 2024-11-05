# Microsoft Developer Studio Project File - Name="GfxCommon_DX9" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GfxCommon_DX9 - Win32 EditorDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GfxCommon_DX9.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GfxCommon_DX9.mak" CFG="GfxCommon_DX9 - Win32 EditorDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GfxCommon_DX9 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon_DX9 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon_DX9 - Win32 DbgRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "GfxCommon_DX9 - Win32 EditorDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GfxCommon", UINAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GfxCommon_DX9 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GfxCommon_DX9___Win32_Release"
# PROP BASE Intermediate_Dir "GfxCommon_DX9___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_dx9"
# PROP Intermediate_Dir "Release_dx9"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /X /I "E:\OnlineGm\XAJHElement\Include\stlport" /I "C:\PROGRAM FILES\MICROSOFT SDK\INCLUDE" /I "C:\PROGRAM FILES\MICROSOFT DIRECTX SDK (AUGUST 2008)\INCLUDE" /I "C:\DX8SDK\include" /I "C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSICS\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\FOUNDATION\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\COOKING\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\NXCHARACTER\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSXLOADER\INCLUDE" /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\A3DSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\ACSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AFSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AMSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\3RDSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\APSDK\INCLUDE" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /D "_DX9" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GfxCommon_DX9 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon_DX9___Win32_Debug"
# PROP BASE Intermediate_Dir "GfxCommon_DX9___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_dx9"
# PROP Intermediate_Dir "Debug_dx9"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "E:\OnlineGm\XAJHElement\Include\stlport" /I "C:\PROGRAM FILES\MICROSOFT SDK\INCLUDE" /I "C:\PROGRAM FILES\MICROSOFT DIRECTX SDK (AUGUST 2008)\INCLUDE" /I "C:\DX8SDK\include" /I "C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSICS\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\FOUNDATION\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\COOKING\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\NXCHARACTER\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSXLOADER\INCLUDE" /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\A3DSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\ACSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AFSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AMSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\3RDSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\APSDK\INCLUDE" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /D "_DX9" /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug_dx9\GfxCommon_d_dx9.lib"

!ELSEIF  "$(CFG)" == "GfxCommon_DX9 - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DbgRelease"
# PROP BASE Intermediate_Dir "DbgRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease_dx9"
# PROP Intermediate_Dir "DbgRelease_dx9"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /X /I "E:\OnlineGm\XAJHElement\Include\stlport" /I "C:\PROGRAM FILES\MICROSOFT SDK\INCLUDE" /I "C:\PROGRAM FILES\MICROSOFT DIRECTX SDK (AUGUST 2008)\INCLUDE" /I "C:\DX8SDK\include" /I "C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSICS\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\FOUNDATION\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\COOKING\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\NXCHARACTER\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSXLOADER\INCLUDE" /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\A3DSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\ACSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AFSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AMSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\3RDSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\APSDK\INCLUDE" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /D "_DX9" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"DbgRelease_dx9\GfxCommon_s_dx9.lib"

!ELSEIF  "$(CFG)" == "GfxCommon_DX9 - Win32 EditorDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GfxCommon_DX9___Win32_EditorDebug"
# PROP BASE Intermediate_Dir "GfxCommon_DX9___Win32_EditorDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "EditorDebug_dx9"
# PROP Intermediate_Dir "EditorDebug_dx9"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "E:\OnlineGm\XAJHElement\Include\stlport" /I "C:\PROGRAM FILES\MICROSOFT SDK\INCLUDE" /I "C:\PROGRAM FILES\MICROSOFT DIRECTX SDK (AUGUST 2008)\INCLUDE" /I "C:\DX8SDK\include" /I "C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSICS\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\FOUNDATION\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\COOKING\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\NXCHARACTER\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSXLOADER\INCLUDE" /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\A3DSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\ACSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AFSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AMSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\3RDSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\APSDK\INCLUDE" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "UNICODE" /D "A3D_PHYSX" /D "_DX9" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "E:\OnlineGm\XAJHElement\Include\stlport" /I "C:\PROGRAM FILES\MICROSOFT SDK\INCLUDE" /I "C:\PROGRAM FILES\MICROSOFT DIRECTX SDK (AUGUST 2008)\INCLUDE" /I "C:\DX8SDK\include" /I "C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE" /I "C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSICS\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\FOUNDATION\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\COOKING\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\NXCHARACTER\INCLUDE" /I "C:\PROGRAM FILES\NVIDIA CORPORATION\NVIDIA PHYSX SDK\V2.8.1\SDKS\PHYSXLOADER\INCLUDE" /I "..\APhysXIntegration\APhysXEngine\Include" /I "..\LuaWrapper\Include" /I "..\LuaWrapperExport\Include" /I ".\Include" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\A3DSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\ACSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AFSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2DX9\OUTPUT\AMSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\3RDSDK\INCLUDE" /I "E:\ONLINEGM\ANGELICA2\OUTPUT\APSDK\INCLUDE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "GFX_EDITOR" /D "_SKILLGFXCOMPOSER" /D "A3D_PHYSX" /D "_DX9" /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug_dx9\GfxCommon_d_dx9.lib"
# ADD LIB32 /nologo /out:"EditorDebug_dx9\GfxEditor_d_dx9.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy EditorDebug_dx9\*.lib Output\Lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "GfxCommon_DX9 - Win32 Release"
# Name "GfxCommon_DX9 - Win32 Debug"
# Name "GfxCommon_DX9 - Win32 DbgRelease"
# Name "GfxCommon_DX9 - Win32 EditorDebug"
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

SOURCE=.\A3DGFXCachedItemMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DGFXContainer.h
# End Source File
# Begin Source File

SOURCE=.\A3DGfxECMInterface.h
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
