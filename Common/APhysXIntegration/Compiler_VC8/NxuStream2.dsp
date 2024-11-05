# Microsoft Developer Studio Project File - Name="NxuStream2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NxuStream2 - Win32 Debug MTD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NxuStream2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NxuStream2.mak" CFG="NxuStream2 - Win32 Debug MTD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NxuStream2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "NxuStream2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "NxuStream2 - Win32 Debug MTD" (based on "Win32 (x86) Static Library")
!MESSAGE "NxuStream2 - Win32 Release MTD" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NxuStream2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\NxuStream2\Release"
# PROP Intermediate_Dir "Build\NxuStream2\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\NxuStream2\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\NxuStream2.lib"

!ELSEIF  "$(CFG)" == "NxuStream2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\NxuStream2\Debug"
# PROP Intermediate_Dir "Build\NxuStream2\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\NxuStream2\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\NxuStream2_d.lib"

!ELSEIF  "$(CFG)" == "NxuStream2 - Win32 Debug MTD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NxuStream2___Win32_Debug_MTD"
# PROP BASE Intermediate_Dir "NxuStream2___Win32_Debug_MTD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\NxuStream2\DebugMTD"
# PROP Intermediate_Dir "Build\NxuStream2\DebugMTD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\NxuStream2\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\NxuStream2\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\NxuStream2_d.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\NxuStream2_MTD_d.lib"

!ELSEIF  "$(CFG)" == "NxuStream2 - Win32 Release MTD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NxuStream2___Win32_Release_MTD"
# PROP BASE Intermediate_Dir "NxuStream2___Win32_Release_MTD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\NxuStream2\ReleaseMTD"
# PROP Intermediate_Dir "Build\NxuStream2\ReleaseMTD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\NxuStream2\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\NxuStream2\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\NxuStream2.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\NxuStream2_MTD.lib"

!ENDIF 

# Begin Target

# Name "NxuStream2 - Win32 Release"
# Name "NxuStream2 - Win32 Debug"
# Name "NxuStream2 - Win32 Debug MTD"
# Name "NxuStream2 - Win32 Release MTD"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_Asc2Bin.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_ColladaExport.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_ColladaImport.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_cooking.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_customcopy.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_File.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_Geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_GraphicsMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_helper.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_hull.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_PhysicsExport.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_PhysicsInstantiator.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_ScaledCopy.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_schema.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_SchemaStream.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_Streaming.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_string.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Src\NXU_tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_Asc2Bin.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_ColladaExport.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_ColladaImport.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_cooking.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_customcopy.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_File.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_Geometry.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_GraphicsMesh.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_helper.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_hull.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_PhysicsExport.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_PhysicsInstantiator.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_ScaledCopy.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_schema.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_SchemaStream.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_SchemaTypes.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_Streaming.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_string.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_tinystr.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\NxuStream2\Include\NXU_VC6_Compatible.h
# End Source File
# End Group
# End Target
# End Project
