# Microsoft Developer Studio Project File - Name="AngelicaMedia" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AngelicaMedia - Win32 StdNew Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaMedia.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaMedia.mak" CFG="AngelicaMedia - Win32 StdNew Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AngelicaMedia - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 StdNew Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 StdNew Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 StdNew Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 DBG Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaMedia - Win32 StdNew Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/AngelicaMedia", PUDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AngelicaMedia - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Release\AngelicaMedia.lib
TargetName=AngelicaMedia
InputPath=.\Release\AngelicaMedia.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 Debug"

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
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\AngelicaMedia_d.lib"
# Begin Custom Build
TargetPath=.\Debug\AngelicaMedia_d.lib
TargetName=AngelicaMedia_d
InputPath=.\Debug\AngelicaMedia_d.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaMedia___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode_Debug"
# PROP Intermediate_Dir "Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaMedia_d.lib"
# ADD LIB32 /nologo /out:"Unicode_Debug\AngelicaMedia_ud.lib"
# Begin Custom Build
TargetPath=.\Unicode_Debug\AngelicaMedia_ud.lib
TargetName=AngelicaMedia_ud
InputPath=.\Unicode_Debug\AngelicaMedia_ud.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaMedia___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Unicode_Release"
# PROP Intermediate_Dir "Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Unicode_Release\AngelicaMedia_u.lib"
# Begin Custom Build
TargetPath=.\Unicode_Release\AngelicaMedia_u.lib
TargetName=AngelicaMedia_u
InputPath=.\Unicode_Release\AngelicaMedia_u.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 StdNew Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaMedia___Win32_StdNew_Debug"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_StdNew_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Debug"
# PROP Intermediate_Dir "StdNew_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaMedia_d.lib"
# ADD LIB32 /nologo /out:"StdNew_Debug\AngelicaMedia_sd.lib"
# Begin Custom Build
TargetPath=.\StdNew_Debug\AngelicaMedia_sd.lib
TargetName=AngelicaMedia_sd
InputPath=.\StdNew_Debug\AngelicaMedia_sd.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 StdNew Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaMedia___Win32_StdNew_Release"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_StdNew_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Release"
# PROP Intermediate_Dir "StdNew_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"StdNew_Release\AngelicaMedia_s.lib"
# Begin Custom Build
TargetPath=.\StdNew_Release\AngelicaMedia_s.lib
TargetName=AngelicaMedia_s
InputPath=.\StdNew_Release\AngelicaMedia_s.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 StdNew Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaMedia___Win32_StdNew_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_StdNew_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Unicode_Debug"
# PROP Intermediate_Dir "StdNew_Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNew_Debug\AngelicaMedia_sd.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Debug\AngelicaMedia_sud.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Debug\AngelicaMedia_sud.lib
TargetName=AngelicaMedia_sud
InputPath=.\StdNew_Unicode_Debug\AngelicaMedia_sud.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 DBG Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaMedia___Win32_DBG_Release"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_DBG_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DBGRelease"
# PROP Intermediate_Dir "DBGRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaMedia_su.lib"
# ADD LIB32 /nologo /out:"DBGRelease\AngelicaMedia_dr.lib"
# Begin Custom Build
TargetPath=.\DBGRelease\AngelicaMedia_dr.lib
TargetName=AngelicaMedia_dr
InputPath=.\DBGRelease\AngelicaMedia_dr.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaMedia - Win32 StdNew Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaMedia___Win32_StdNew_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaMedia___Win32_StdNew_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Unicode_Release"
# PROP Intermediate_Dir "StdNew_Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\angelica3d\header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaMedia_su.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Release\AngelicaMedia_su.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Release\AngelicaMedia_su.lib
TargetName=AngelicaMedia_su
InputPath=.\StdNew_Unicode_Release\AngelicaMedia_su.lib
SOURCE="$(InputPath)"

"..\Output\amsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\amsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\amsdk\include\*.h 
	del ..\Output\amsdk\include\ampi.h 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AngelicaMedia - Win32 Release"
# Name "AngelicaMedia - Win32 Debug"
# Name "AngelicaMedia - Win32 Unicode Debug"
# Name "AngelicaMedia - Win32 Unicode Release"
# Name "AngelicaMedia - Win32 StdNew Debug"
# Name "AngelicaMedia - Win32 StdNew Release"
# Name "AngelicaMedia - Win32 StdNew Unicode Debug"
# Name "AngelicaMedia - Win32 DBG Release"
# Name "AngelicaMedia - Win32 StdNew Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\AM3DSoundBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AM3DSoundDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMMp3File.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMOggFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMPI.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSoundBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSoundBufferMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSoundDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSoundEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSoundStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMSurfaceRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMVideoClip.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMVideoEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMWaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FFT.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Header\AM.h
# End Source File
# Begin Source File

SOURCE=.\Header\AM3DSoundBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Header\AM3DSoundDevice.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMConfig.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMEngine.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMMp3File.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMOggFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMPI.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSoundBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSoundBufferMan.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSoundDevice.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSoundEngine.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSoundStream.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMSurfaceRenderer.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMTypes.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMVideoClip.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMVideoEngine.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMWaveFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\FFT.h
# End Source File
# End Group
# End Target
# End Project
