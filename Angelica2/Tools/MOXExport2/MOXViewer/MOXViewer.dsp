# Microsoft Developer Studio Project File - Name="MOXViewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MOXViewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MOXViewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MOXViewer.mak" CFG="MOXViewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MOXViewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MOXViewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/MAX PlugIn/MOXViewer", XFAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MOXViewer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MOXViewer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MOXViewer - Win32 Release"
# Name "MOXViewer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\A3DBox.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DFile.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DFont.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DModel.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DOBBFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DObject.cpp
# End Source File
# Begin Source File

SOURCE=..\A3DTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\AList.cpp
# End Source File
# Begin Source File

SOURCE=..\D3DRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\D3DWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MOXViewer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\A3DFile.h
# End Source File
# Begin Source File

SOURCE=..\A3DFont.h
# End Source File
# Begin Source File

SOURCE=..\A3DFrame.h
# End Source File
# Begin Source File

SOURCE=..\A3DFuncs.h
# End Source File
# Begin Source File

SOURCE=..\A3DMacros.h
# End Source File
# Begin Source File

SOURCE=..\A3DMaterial.h
# End Source File
# Begin Source File

SOURCE=..\A3DMesh.h
# End Source File
# Begin Source File

SOURCE=..\A3DModel.h
# End Source File
# Begin Source File

SOURCE=..\A3DObject.h
# End Source File
# Begin Source File

SOURCE=..\A3DTexture.h
# End Source File
# Begin Source File

SOURCE=..\A3DTypes.h
# End Source File
# Begin Source File

SOURCE=..\A3DVertex.h
# End Source File
# Begin Source File

SOURCE=..\AList.h
# End Source File
# Begin Source File

SOURCE=..\D3DRenderer.h
# End Source File
# Begin Source File

SOURCE=..\D3DWindow.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ConsoleFont.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\MOXViewer.ico
# End Source File
# Begin Source File

SOURCE=.\MOXViewer.rc

!IF  "$(CFG)" == "MOXViewer - Win32 Release"

!ELSEIF  "$(CFG)" == "MOXViewer - Win32 Debug"

# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804 /fo"./MOXViewer.res"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
