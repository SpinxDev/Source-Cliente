# Microsoft Developer Studio Project File - Name="SkeletonExportor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SkeletonExportor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SkeletonExportor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SkeletonExportor.mak" CFG="SkeletonExportor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SkeletonExportor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SkeletonExportor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Tools/SkeletonExportor", KLCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SkeletonExportor - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Morpher.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_s.lib AngelicaFile_s.lib AngelicaMedia_s.lib Angelica3D_s.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Bin\SkeletonExportor.dle" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# Begin Custom Build
TargetPath=.\Bin\SkeletonExportor.dle
TargetName=SkeletonExportor
InputPath=.\Bin\SkeletonExportor.dle
SOURCE="$(InputPath)"

"E:\Graphics\3dsmax5.1\plugins\$(TargetName).dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy plugin to max folder 
	copy $(TargetPath) E:\Graphics\3dsmax5.1\plugins\$(TargetName).dle 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "SkeletonExportor - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Morpher.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib zliblib.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_sd.lib AngelicaFile_sd.lib AngelicaMedia_sd.lib Angelica3D_sd.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Bin\SkeletonExportor.dle" /pdbtype:sept /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# Begin Custom Build
TargetPath=.\Bin\SkeletonExportor.dle
TargetName=SkeletonExportor
InputPath=.\Bin\SkeletonExportor.dle
SOURCE="$(InputPath)"

"E:\Graphics\3dsmax5.1\plugins\$(TargetName).dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy plugin to max folder 
	copy $(TargetPath) E:\Graphics\3dsmax5.1\plugins\$(TargetName).dle 
	copy .\Bin\*.ilk E:\Graphics\3dsmax5.1\plugins 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "SkeletonExportor - Win32 Release"
# Name "SkeletonExportor - Win32 Debug"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonExportor.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonExportor.def
# End Source File
# Begin Source File

SOURCE=.\SkeletonExportor.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonExportor.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Export Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BoneCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\BoneCandidate.h
# End Source File
# Begin Source File

SOURCE=.\Exportor.cpp
# End Source File
# Begin Source File

SOURCE=.\Exportor.h
# End Source File
# Begin Source File

SOURCE=.\MAXInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\MAXInterface.h
# End Source File
# Begin Source File

SOURCE=.\MAXMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\MAXMesh.h
# End Source File
# Begin Source File

SOURCE=.\MAXNode.cpp
# End Source File
# Begin Source File

SOURCE=.\MAXNode.h
# End Source File
# Begin Source File

SOURCE=.\MAXNullView.h
# End Source File
# Begin Source File

SOURCE=.\MeshCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshCandidate.h
# End Source File
# Begin Source File

SOURCE=.\MorphChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\MorphChannel.h
# End Source File
# Begin Source File

SOURCE=.\PhysiqueCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysiqueCandidate.h
# End Source File
# Begin Source File

SOURCE=.\PhysiqueExport.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysiqueExport.h
# End Source File
# Begin Source File

SOURCE=.\PhysiqueExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonCandidate.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonExport.cpp
# End Source File
# Begin Source File

SOURCE=.\SkeletonExport.h
# End Source File
# Begin Source File

SOURCE=.\SkeletonExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\SkinCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinCandidate.h
# End Source File
# Begin Source File

SOURCE=.\SkinMeshExport.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinMeshExport.h
# End Source File
# Begin Source File

SOURCE=.\SkinMeshExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\SkinModelExport.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinModelExport.h
# End Source File
# Begin Source File

SOURCE=.\SkinModelExportDesc.h
# End Source File
# Begin Source File

SOURCE=.\VertexCandidate.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexCandidate.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bone.ico
# End Source File
# Begin Source File

SOURCE=.\Dummy.ico
# End Source File
# Begin Source File

SOURCE=.\FakeBone.ico
# End Source File
# Begin Source File

SOURCE=.\Hook.ico
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Other.ico
# End Source File
# Begin Source File

SOURCE=.\res\SkeletonExportor.rc2
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PageAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\PageAnimation.h
# End Source File
# Begin Source File

SOURCE=.\PageMaterialTree.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMaterialTree.h
# End Source File
# Begin Source File

SOURCE=.\PageMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMesh.h
# End Source File
# Begin Source File

SOURCE=.\PageSkeletonFile.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSkeletonFile.h
# End Source File
# Begin Source File

SOURCE=.\PageSkeletonTree.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSkeletonTree.h
# End Source File
# Begin Source File

SOURCE=.\SheetPhysique.cpp
# End Source File
# Begin Source File

SOURCE=.\SheetPhysique.h
# End Source File
# Begin Source File

SOURCE=.\SheetSkeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\SheetSkeleton.h
# End Source File
# Begin Source File

SOURCE=.\SheetSkinMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\SheetSkinMesh.h
# End Source File
# Begin Source File

SOURCE=.\SheetSkinModel.cpp
# End Source File
# Begin Source File

SOURCE=.\SheetSkinModel.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Note.txt
# End Source File
# End Target
# End Project
