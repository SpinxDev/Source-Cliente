# Microsoft Developer Studio Project File - Name="ShaderEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ShaderEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ShaderEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShaderEditor.mak" CFG="ShaderEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShaderEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ShaderEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica/ShaderEditor", LXDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ShaderEditor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 vorbis_static.lib vorbisfile_static.lib ogg_static.lib zliblib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaFile.lib AngelicaMedia.lib dsound.lib dxguid.lib D3dx8.lib d3d8.lib Angelica3d.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"Bin\ShaderEditor.exe" /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"

!ELSEIF  "$(CFG)" == "ShaderEditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dbghlp.lib zliblib.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib dsound.lib Angelica3d_d.lib D3dx8.lib d3d8.lib dxguid.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin\ShaderEditor_d.exe" /pdbtype:sept /libpath:"..\..\Output\A3DSDK\Lib" /libpath:"..\..\Output\AFSDK\Lib" /libpath:"..\..\Output\ACSDK\Lib" /libpath:"..\..\Output\AMSDK\Lib" /libpath:"..\..\Output\3rdSDK\Lib"

!ENDIF 

# Begin Target

# Name "ShaderEditor - Win32 Release"
# Name "ShaderEditor - Win32 Debug"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\CommonDef.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonDef.h
# End Source File
# Begin Source File

SOURCE=.\Decal.cpp
# End Source File
# Begin Source File

SOURCE=.\Decal.h
# End Source File
# Begin Source File

SOURCE=.\MainForm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainForm.h
# End Source File
# Begin Source File

SOURCE=.\MenuUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyForm.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyForm.h
# End Source File
# Begin Source File

SOURCE=.\PropFormCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ShaderEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderEditor.h
# End Source File
# Begin Source File

SOURCE=.\ShaderForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderForm.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\Res\General.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Line.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Selected.bmp
# End Source File
# Begin Source File

SOURCE=.\ShaderEditor.rc
# End Source File
# Begin Source File

SOURCE=.\Res\tcmods.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Unselected.bmp
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AlphaCompDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AlphaCompDlg.h
# End Source File
# Begin Source File

SOURCE=.\BumpMatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BumpMatDlg.h
# End Source File
# Begin Source File

SOURCE=.\ComboListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboListDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgColorValue.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgColorValue.h
# End Source File
# Begin Source File

SOURCE=.\DlgComboVal.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgComboVal.h
# End Source File
# Begin Source File

SOURCE=.\DlgD3DTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgD3DTextures.h
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.h
# End Source File
# Begin Source File

SOURCE=.\DlgTexCoord.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTexCoord.h
# End Source File
# Begin Source File

SOURCE=.\DlgTexTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTexTransform.h
# End Source File
# Begin Source File

SOURCE=.\GenBlendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GenBlendDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewShaderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewShaderDlg.h
# End Source File
# Begin Source File

SOURCE=.\PixelShaderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PixelShaderDlg.h
# End Source File
# Begin Source File

SOURCE=.\SaveAskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveAskDlg.h
# End Source File
# Begin Source File

SOURCE=.\SingleValDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleValDlg.h
# End Source File
# Begin Source File

SOURCE=.\TexOPDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TexOPDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureDlg.h
# End Source File
# Begin Source File

SOURCE=.\TwoValDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TwoValDlg.h
# End Source File
# Begin Source File

SOURCE=.\WaveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveDlg.h
# End Source File
# End Group
# End Target
# End Project
