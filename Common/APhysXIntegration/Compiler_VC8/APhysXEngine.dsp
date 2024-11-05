# Microsoft Developer Studio Project File - Name="APhysXEngine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APhysXEngine - Win32 DebugStaticLib ForbidAngelicaNewDelete
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APhysXEngine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APhysXEngine.mak" CFG="APhysXEngine - Win32 DebugStaticLib ForbidAngelicaNewDelete"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APhysXEngine - Win32 ReleaseStaticLib" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 DebugStaticLib" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 Unicode ReleaseStaticLib" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 Unicode DebugStaticLib" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 Unicod DebugReleaseStaticLib" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 DebugStaticLib ForbidAngelicaNewDelete" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 ReleaseStaticLib ForbidAngelicaNewDelete" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 ReleaseStaticLib DebugRender" (based on "Win32 (x86) Static Library")
!MESSAGE "APhysXEngine - Win32 Unicode ReleaseStaticLib DebugRender" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/APhysXIntegration/Compiler_VC6", ATPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APhysXEngine - Win32 ReleaseStaticLib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\ReleaseStaticLib"
# PROP Intermediate_Dir "Build\APhysXEngine\ReleaseStaticLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 DebugStaticLib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\APhysXEngine\DebugStaticLib"
# PROP Intermediate_Dir "Build\APhysXEngine\DebugStaticLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /Yu"APhysXStdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_d.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 Unicode ReleaseStaticLib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXEngine___Win32_Unicode_ReleaseStaticLib"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_Unicode_ReleaseStaticLib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\UnicodeReleaseStaticLib"
# PROP Intermediate_Dir "Build\APhysXEngine\UnicodeReleaseStaticLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_u.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 Unicode DebugStaticLib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APhysXEngine___Win32_Unicode_DebugStaticLib"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_Unicode_DebugStaticLib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\APhysXEngine\UnicodeDebugStaticLib"
# PROP Intermediate_Dir "Build\APhysXEngine\UnicodeDebugStaticLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /Yu"APhysXStdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_d.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_ud.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 Unicod DebugReleaseStaticLib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXEngine___Win32_Unicod_DebugReleaseStaticLib"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_Unicod_DebugReleaseStaticLib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\UnicodeDebugReleaseStaticLib"
# PROP Intermediate_Dir "Build\APhysXEngine\UnicodeDebugReleaseStaticLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /Yu"APhysXStdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_u.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_su.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 DebugStaticLib ForbidAngelicaNewDelete"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APhysXEngine___Win32_DebugStaticLib_ForbidAngelicaNewDelete"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_DebugStaticLib_ForbidAngelicaNewDelete"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\APhysXEngine\DebugStaticLib_ForbidAngelicaNewDelete"
# PROP Intermediate_Dir "Build\APhysXEngine\DebugStaticLib_ForbidAngelicaNewDelete"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /Yu"APhysXStdAfx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\APhysXEngine\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "_A_FORBID_MALLOC" /D "_A_FORBID_NEWDELETE" /Yu"APhysXStdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_d.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_sd.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 ReleaseStaticLib ForbidAngelicaNewDelete"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXEngine___Win32_ReleaseStaticLib_ForbidAngelicaNewDelete"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_ReleaseStaticLib_ForbidAngelicaNewDelete"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\ReleaseStaticLib_ForbidAngelicaNewDelete"
# PROP Intermediate_Dir "Build\APhysXEngine\ReleaseStaticLib_ForbidAngelicaNewDelete"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /Yu"APhysXStdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "_A_FORBID_MALLOC" /D "_A_FORBID_NEWDELETE" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_s.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 ReleaseStaticLib DebugRender"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXEngine___Win32_ReleaseStaticLib_DebugRender"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_ReleaseStaticLib_DebugRender"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\ReleaseStaticLib_DebugRender"
# PROP Intermediate_Dir "Build\APhysXEngine\ReleaseStaticLib_DebugRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /Yu"APhysXStdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "APHYSX_DEBUG_RENDER" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_v.lib"

!ELSEIF  "$(CFG)" == "APhysXEngine - Win32 Unicode ReleaseStaticLib DebugRender"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APhysXEngine___Win32_Unicode_ReleaseStaticLib_DebugRender"
# PROP BASE Intermediate_Dir "APhysXEngine___Win32_Unicode_ReleaseStaticLib_DebugRender"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\APhysXEngine\UnicodeReleaseStaticLib_DebugRender"
# PROP Intermediate_Dir "Build\APhysXEngine\UnicodeReleaseStaticLib_DebugRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /Yu"APhysXStdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\APhysXEngine\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "APHYSX_BUILD_ENGINE" /D "UNICODE" /D "APHYSX_DEBUG_RENDER" /Yu"APhysXStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_u.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\APhysXEngineStatic_uv.lib"

!ENDIF 

# Begin Target

# Name "APhysXEngine - Win32 ReleaseStaticLib"
# Name "APhysXEngine - Win32 DebugStaticLib"
# Name "APhysXEngine - Win32 Unicode ReleaseStaticLib"
# Name "APhysXEngine - Win32 Unicode DebugStaticLib"
# Name "APhysXEngine - Win32 Unicod DebugReleaseStaticLib"
# Name "APhysXEngine - Win32 DebugStaticLib ForbidAngelicaNewDelete"
# Name "APhysXEngine - Win32 ReleaseStaticLib ForbidAngelicaNewDelete"
# Name "APhysXEngine - Win32 ReleaseStaticLib DebugRender"
# Name "APhysXEngine - Win32 Unicode ReleaseStaticLib DebugRender"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXActorDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXAttacher.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXAttacherDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXBoxShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCapsuleShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothAttacher.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothAttacherDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothAttachRBConnector.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothAttachRBConnectorDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXClothObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCollisionChannelManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCompoundObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCompoundObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXConvexShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXD6JointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXDebugRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXDistanceJointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidEmitterAttacher.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidEmitterAttacherDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidEmitterAttachRBConnector.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidEmitterAttachRBConnectorDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXFluidObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXForceFieldObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXForceFieldObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXHeightFieldShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXJoint.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXJointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXMaterialDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXMeshCooker.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXMeshDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXObjectConnector.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXObjectConnectorDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXObjectDescManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXPerfStats.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXPicker.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXPlaneShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXPulleyJointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXRBAttacherDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXRegistryHack.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXRevoluteJointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXRigidBodyObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXRigidBodyObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXScene.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXSerializerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXShapeDescManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXSkeletonRBObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXSphereShapeDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXSphericalJointDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXStdAfx.cpp
# ADD CPP /Yc"APhysXStdAfx.h"
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXStream.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXStringManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXTerrain.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXTerrainBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXTerrainPatchRBObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXTerrainQuadManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXUserCallBack.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXUtilLib.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\OverlapDetector.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysX.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXActorDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXAllocator.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXAttacher.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXAttacherDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXBoxShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCapsuleShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothAttacher.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothAttacherDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothAttachRBConnector.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothAttachRBConnectorDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXClothObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCollisionChannelManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCompoundObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCompoundObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXConverter.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXConvexShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXD6JointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXDebugRenderer.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXDistanceJointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXDummyRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXDynamicRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXEngine.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidEmitterAttacher.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidEmitterAttacherDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidEmitterAttachRBConnector.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidEmitterAttachRBConnectorDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXFluidObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXForceFieldObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXForceFieldObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXGraph.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXHeightFieldShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXJoint.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXJointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXMaterialDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXMeshCooker.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXMeshDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXMutex.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXObjectConnector.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXObjectConnectorDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXObjectDescManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXPerfStats.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXPicker.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXPlaneShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXPrerequisites.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXPulleyJointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRBAttacher.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRBAttacherDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRefCountable.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRegistryHack.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRevoluteJointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRigidBodyObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXRigidBodyObjectDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXScene.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXSerializerBase.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXShapeDescManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXSingleton.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXSkeletonRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXSphereShapeDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXSphericalJointDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXStaticRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXStdAfx.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXStream.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXStringManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXTerrain.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXTerrainBlock.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXTerrainPatchRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXTerrainQuadManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXUserCallBack.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXUserGraphicsSync.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXUtilLib.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\OverlapDetector.h
# End Source File
# End Group
# Begin Group "PhysXBenchmark"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXEngine\Src\PhysXBenchmark\PhysXBenchmark.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\PhysXBenchmark\PhysXBenchmark.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\PhysXBenchmark\Stream.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\PhysXBenchmark\Stream.h
# End Source File
# End Group
# Begin Group "Breakable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXBreakable.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXBreakableRBObject.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXBreakableRBObject.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXBreakableRBObjectDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXBreakableRBObjectDesc.h
# End Source File
# End Group
# Begin Group "CharacterControl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrler.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrler.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerAssistance.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerAssistance.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerBase.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerCallBack.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerCallBackAgent.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerCallBackAgent.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerDesc.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerDynamic.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerDynamic.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerDynConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerDynConfig.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerImp.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerLightWeight.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerLightWeight.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerManager.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerManager.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerMgrImp.h
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Src\APhysXCharCtrlerNx.cpp
# End Source File
# Begin Source File

SOURCE=..\APhysXEngine\Include\APhysXCharCtrlerNx.h
# End Source File
# End Group
# End Target
# End Project
