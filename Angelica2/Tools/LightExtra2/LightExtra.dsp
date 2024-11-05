# Microsoft Developer Studio Project File - Name="LightExtra" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LightExtra - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LightExtra.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LightExtra.mak" CFG="LightExtra - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LightExtra - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LightExtra - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LightExtra - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LightExtra", AIBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LightExtra - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib d3d8.lib d3dx8.lib dxguid.lib dsound.lib angelica3d_s.lib angelicacommon_s.lib angelicafile_s.lib zliblib.lib angelicamedia_s.lib mpg123lib.lib immwrapper.lib /nologo /base:"0x53dd0000" /subsystem:windows /dll /machine:I386 /out:"Bin\LightExtra2.dlu" /libpath:"C:\3dsmax5\maxsdk\lib" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib" /release
# Begin Custom Build
TargetPath=.\Bin\LightExtra2.dlu
TargetName=LightExtra2
InputPath=.\Bin\LightExtra2.dlu
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy plugin to max folder \
	copy $(TargetPath) E:\Graphics\3dsmax4\plugins\$(TargetName).dlu \
	copy $(TargetPath) c:\3dsmax4\plugins\$(TargetName).dlu \
	

"E:\Graphics\3dsmax4\plugins\$(TargetName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"c:\3dsmax4\plugins\$(TargetName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "LightExtra - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib d3d8.lib d3dx8.lib dxguid.lib dsound.lib angelica3d_d.lib angelicacommon_d.lib angelicafile_d.lib zliblib_d.lib angelicamedia_d.lib mpg123lib_d.lib immwrapper_d.lib /nologo /base:"0x53dd0000" /subsystem:windows /dll /debug /machine:I386 /out:"Bin\LightExtra2.dlu" /pdbtype:sept /libpath:"C:\3dsmax5\maxsdk\lib" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# Begin Custom Build
TargetPath=.\Bin\LightExtra2.dlu
TargetName=LightExtra2
InputPath=.\Bin\LightExtra2.dlu
SOURCE="$(InputPath)"

"E:\Graphics\3dsmax4\plugins\$(TargetName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy plugin to max folder 
	copy $(TargetPath) E:\Graphics\3dsmax4\plugins\$(TargetName).dlu 
	copy .\Bin\*.ilk E:\Graphics\3dsmax4\plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LightExtra - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LightExtra___Win32_Hybrid0"
# PROP BASE Intermediate_Dir "LightExtra___Win32_Hybrid0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LightExtra___Win32_Hybrid0"
# PROP Intermediate_Dir "LightExtra___Win32_Hybrid0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /YX /FD /LD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib d3d8.lib d3dx8.lib dxguid.lib dsound.lib angelica3d_d.lib angelicacommon_d.lib angelicafile_d.lib zliblib_d.lib angelicamedia_d.lib mpg123lib_d.lib immwrapper_d.lib /nologo /base:"0x53dd0000" /subsystem:windows /dll /debug /machine:I386 /out:"Bin\LightExtra2.dlu" /pdbtype:sept /libpath:"C:\3dsmax5\maxsdk\lib" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib gup.lib paramblk2.lib d3d8.lib d3dx8.lib dxguid.lib dsound.lib angelica3d_sd.lib angelicacommon_sd.lib angelicafile_sd.lib zliblib_d.lib angelicamedia_sd.lib mpg123lib_d.lib immwrapper_d.lib /nologo /base:"0x53dd0000" /subsystem:windows /dll /debug /machine:I386 /out:"Bin\LightExtra2.dlu" /pdbtype:sept /libpath:"C:\3dsmax5\maxsdk\lib" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"
# Begin Custom Build
TargetPath=.\Bin\LightExtra2.dlu
TargetName=LightExtra2
InputPath=.\Bin\LightExtra2.dlu
SOURCE="$(InputPath)"

"E:\Graphics\3dsmax4\plugins\$(TargetName).dlu" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy plugin to max folder 
	copy $(TargetPath) E:\Graphics\3dsmax4\plugins\$(TargetName).dlu 
	copy .\Bin\*.ilk E:\Graphics\3dsmax4\plugins 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "LightExtra - Win32 Release"
# Name "LightExtra - Win32 Debug"
# Name "LightExtra - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine3DWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Enumeration.cpp
# End Source File
# Begin Source File

SOURCE=.\evalcol.cpp
# End Source File
# Begin Source File

SOURCE=.\LightExtra.cpp
# End Source File
# Begin Source File

SOURCE=.\LightExtra.def
# End Source File
# Begin Source File

SOURCE=.\LightExtra3DWindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Engine3DWindow.h
# End Source File
# Begin Source File

SOURCE=.\Enumeration.h
# End Source File
# Begin Source File

SOURCE=.\evalcol.h
# End Source File
# Begin Source File

SOURCE=.\Instance.h
# End Source File
# Begin Source File

SOURCE=.\LightExtra.h
# End Source File
# Begin Source File

SOURCE=.\LightExtra3DWindow.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\LightExtra.rc
# End Source File
# End Group
# End Target
# End Project
