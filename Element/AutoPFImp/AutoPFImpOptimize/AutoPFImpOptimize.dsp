# Microsoft Developer Studio Project File - Name="AutoPFImpOptimize" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AutoPFImpOptimize - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoPFImpOptimize.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoPFImpOptimize.mak" CFG="AutoPFImpOptimize - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoPFImpOptimize - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AutoPFImpOptimize - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "AutoPFImpOptimize - Win32 DbgRelease" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/AutoPFImp/AutoPFImpFindPara", JIDBAAAA"
# PROP Scc_LocalPath "..\autopfimpfindpara"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoPFImpOptimize - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../bin/AutoPFImpOptimize.exe" /libpath:"..\lib" /libpath:"..\..\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase"

!ELSEIF  "$(CFG)" == "AutoPFImpOptimize - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AutoMoveImp_d.lib version.lib psapi.lib zlibwapi.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"MSVCRTD" /out:"../Bin/AutoPFImpOptimize_d.exe" /pdbtype:sept /libpath:"..\lib" /libpath:"..\..\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase"

!ELSEIF  "$(CFG)" == "AutoPFImpOptimize - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AutoPFImpOptimize___Win32_DbgRelease0"
# PROP BASE Intermediate_Dir "AutoPFImpOptimize___Win32_DbgRelease0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 AutoMoveImp_su.lib version.lib psapi.lib zlibwapi.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /subsystem:console /debug /machine:I386 /out:"../Bin/AutoPFImpOptimize.exe" /libpath:"..\lib" /libpath:"..\..\Lib" /libpath:"..\..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase"

!ENDIF 

# Begin Target

# Name "AutoPFImpOptimize - Win32 Release"
# Name "AutoPFImpOptimize - Win32 Debug"
# Name "AutoPFImpOptimize - Win32 DbgRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoPFImpOptimize.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\EC_NativeAPI.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
