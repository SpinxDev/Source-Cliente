# Microsoft Developer Studio Project File - Name="AngelicaCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AngelicaCommon - Win32 StdNew Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaCommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaCommon.mak" CFG="AngelicaCommon - Win32 StdNew Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AngelicaCommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 StdNew Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 StdNew Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 StdNew Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 DBG Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaCommon - Win32 StdNew Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/AngelicaCommon", ORAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AngelicaCommon - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Release\AngelicaCommon.lib
TargetName=AngelicaCommon
InputPath=.\Release\AngelicaCommon.lib
SOURCE="$(InputPath)"


# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\AngelicaCommon_d.lib"
# Begin Custom Build
TargetPath=.\Debug\AngelicaCommon_d.lib
TargetName=AngelicaCommon_d
InputPath=.\Debug\AngelicaCommon_d.lib
SOURCE="$(InputPath)"


	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaCommon___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode_Debug"
# PROP Intermediate_Dir "Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaCommon_d.lib"
# ADD LIB32 /nologo /out:"Unicode_Debug\AngelicaCommon_ud.lib"
# Begin Custom Build
TargetPath=.\Unicode_Debug\AngelicaCommon_ud.lib
TargetName=AngelicaCommon_ud
InputPath=.\Unicode_Debug\AngelicaCommon_ud.lib
SOURCE="$(InputPath)"


	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaCommon___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Unicode_Release"
# PROP Intermediate_Dir "Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Unicode_Release\AngelicaCommon_u.lib"
# Begin Custom Build
TargetPath=.\Unicode_Release\AngelicaCommon_u.lib
TargetName=AngelicaCommon_u
InputPath=.\Unicode_Release\AngelicaCommon_u.lib
SOURCE="$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 StdNew Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaCommon___Win32_StdNew_Debug"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_StdNew_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Debug"
# PROP Intermediate_Dir "StdNew_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaCommon_d.lib"
# ADD LIB32 /nologo /out:"StdNew_Debug\AngelicaCommon_sd.lib"
# Begin Custom Build
TargetPath=.\StdNew_Debug\AngelicaCommon_sd.lib
TargetName=AngelicaCommon_sd
InputPath=.\StdNew_Debug\AngelicaCommon_sd.lib
SOURCE="$(InputPath)"


# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 StdNew Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaCommon___Win32_StdNew_Release"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_StdNew_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Release"
# PROP Intermediate_Dir "StdNew_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"StdNew_Release\AngelicaCommon_s.lib"
# Begin Custom Build
TargetPath=.\StdNew_Release\AngelicaCommon_s.lib
TargetName=AngelicaCommon_s
InputPath=.\StdNew_Release\AngelicaCommon_s.lib
SOURCE="$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 StdNew Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaCommon___Win32_StdNew_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_StdNew_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Unicode_Debug"
# PROP Intermediate_Dir "StdNew_Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNew_Debug\AngelicaCommon_sd.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Debug\AngelicaCommon_sud.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Debug\AngelicaCommon_sud.lib
TargetName=AngelicaCommon_sud
InputPath=.\StdNew_Unicode_Debug\AngelicaCommon_sud.lib
SOURCE="$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 DBG Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaCommon___Win32_DBG_Release"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_DBG_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DBGRelease"
# PROP Intermediate_Dir "DBGRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaCommon_su.lib"
# ADD LIB32 /nologo /out:"DBGRelease\AngelicaCommon_dr.lib"
# Begin Custom Build
TargetPath=.\DBGRelease\AngelicaCommon_dr.lib
TargetName=AngelicaCommon_dr
InputPath=.\DBGRelease\AngelicaCommon_dr.lib
SOURCE="$(InputPath)"

	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaCommon - Win32 StdNew Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaCommon___Win32_StdNew_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaCommon___Win32_StdNew_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Unicode_Release"
# PROP Intermediate_Dir "StdNew_Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaCommon_su.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Release\AngelicaCommon_su.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Release\AngelicaCommon_su.lib
TargetName=AngelicaCommon_su
InputPath=.\StdNew_Unicode_Release\AngelicaCommon_su.lib
SOURCE="$(InputPath)"

	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AngelicaCommon - Win32 Release"
# Name "AngelicaCommon - Win32 Debug"
# Name "AngelicaCommon - Win32 Unicode Debug"
# Name "AngelicaCommon - Win32 Unicode Release"
# Name "AngelicaCommon - Win32 StdNew Debug"
# Name "AngelicaCommon - Win32 StdNew Release"
# Name "AngelicaCommon - Win32 StdNew Unicode Debug"
# Name "AngelicaCommon - Win32 DBG Release"
# Name "AngelicaCommon - Win32 StdNew Unicode Release"
# Begin Group "Base Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Header\AArray.h
# End Source File
# Begin Source File

SOURCE=.\Header\ABaseDef.h
# End Source File
# Begin Source File

SOURCE=.\Header\AChar.h
# End Source File
# Begin Source File

SOURCE=.\Source\AList.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\AList.h
# End Source File
# Begin Source File

SOURCE=.\Header\AList2.h
# End Source File
# Begin Source File

SOURCE=.\Source\AMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\AMemory.h
# End Source File
# Begin Source File

SOURCE=.\Header\APoint.h
# End Source File
# Begin Source File

SOURCE=.\Header\ARect.h
# End Source File
# Begin Source File

SOURCE=.\Header\AStack.h
# End Source File
# Begin Source File

SOURCE=.\Source\AString.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\AString.h
# End Source File
# Begin Source File

SOURCE=.\Source\AWString.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\AWString.h
# End Source File
# Begin Source File

SOURCE=.\Header\deque.h
# End Source File
# Begin Source File

SOURCE=.\Header\hashmap.h
# End Source File
# Begin Source File

SOURCE=.\Header\hashtab.h
# End Source File
# Begin Source File

SOURCE=.\Header\vector.h
# End Source File
# End Group
# Begin Group "Assist Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\AAssist.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\AAssist.h
# End Source File
# Begin Source File

SOURCE=.\Source\ACounter.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\ACounter.h
# End Source File
# Begin Source File

SOURCE=.\Header\ACPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Header\ACSWrapper.h
# End Source File
# Begin Source File

SOURCE=.\Source\ALog.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\ALog.h
# End Source File
# Begin Source File

SOURCE=.\Source\APerlinNoise1D.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\APerlinNoise1D.h
# End Source File
# Begin Source File

SOURCE=.\Source\APerlinNoise2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\APerlinNoise2D.h
# End Source File
# Begin Source File

SOURCE=.\Source\APerlinNoise3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\APerlinNoise3D.h
# End Source File
# Begin Source File

SOURCE=.\Source\APerlinNoiseBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\APerlinNoiseBase.h
# End Source File
# Begin Source File

SOURCE=.\Source\ARandomGen.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\ARandomGen.h
# End Source File
# Begin Source File

SOURCE=.\Source\ATime.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\ATime.h
# End Source File
# Begin Source File

SOURCE=.\Source\jexception.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\jexception.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Header\AC.h
# End Source File
# End Target
# End Project
