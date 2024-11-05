# Microsoft Developer Studio Project File - Name="AngelicaFile" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AngelicaFile - Win32 StdNew Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaFile.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AngelicaFile.mak" CFG="AngelicaFile - Win32 StdNew Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AngelicaFile - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 StdNew Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 StdNew Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 StdNew Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 DBG Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 StdNew Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AngelicaFile - Win32 DBG Release SBCS" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/AngelicaFile", XQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AngelicaFile - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Release\AngelicaFile.lib
TargetName=AngelicaFile
InputPath=.\Release\AngelicaFile.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\AngelicaFile_d.lib"
# Begin Custom Build
TargetPath=.\Debug\AngelicaFile_d.lib
TargetName=AngelicaFile_d
InputPath=.\Debug\AngelicaFile_d.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaFile___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode_Debug"
# PROP Intermediate_Dir "Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaFile_d.lib"
# ADD LIB32 /nologo /out:"Unicode_Debug\AngelicaFile_ud.lib"
# Begin Custom Build
TargetPath=.\Unicode_Debug\AngelicaFile_ud.lib
TargetName=AngelicaFile_ud
InputPath=.\Unicode_Debug\AngelicaFile_ud.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaFile___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Unicode_Release"
# PROP Intermediate_Dir "Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Unicode_Release\AngelicaFile_u.lib"
# Begin Custom Build
TargetPath=.\Unicode_Release\AngelicaFile_u.lib
TargetName=AngelicaFile_u
InputPath=.\Unicode_Release\AngelicaFile_u.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 StdNew Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaFile___Win32_StdNew_Debug"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_StdNew_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Debug"
# PROP Intermediate_Dir "StdNew_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\AngelicaFile_d.lib"
# ADD LIB32 /nologo /out:"StdNew_Debug\AngelicaFile_sd.lib"
# Begin Custom Build
TargetPath=.\StdNew_Debug\AngelicaFile_sd.lib
TargetName=AngelicaFile_sd
InputPath=.\StdNew_Debug\AngelicaFile_sd.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 StdNew Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaFile___Win32_StdNew_Release"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_StdNew_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Release"
# PROP Intermediate_Dir "StdNew_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"StdNew_Release\AngelicaFile_s.lib"
# Begin Custom Build
TargetPath=.\StdNew_Release\AngelicaFile_s.lib
TargetName=AngelicaFile_s
InputPath=.\StdNew_Release\AngelicaFile_s.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 StdNew Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AngelicaFile___Win32_StdNew_Unicode_Debug"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_StdNew_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Unicode_Debug"
# PROP Intermediate_Dir "StdNew_Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNew_Debug\AngelicaFile_sd.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Debug\AngelicaFile_sud.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Debug\AngelicaFile_sud.lib
TargetName=AngelicaFile_sud
InputPath=.\StdNew_Unicode_Debug\AngelicaFile_sud.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 DBG Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaFile___Win32_DBG_Release"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_DBG_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DBGRelease"
# PROP Intermediate_Dir "DBGRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaFile_su.lib"
# ADD LIB32 /nologo /out:"DBGRelease\AngelicaFile_dr.lib"
# Begin Custom Build
TargetPath=.\DBGRelease\AngelicaFile_dr.lib
TargetName=AngelicaFile_dr
InputPath=.\DBGRelease\AngelicaFile_dr.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 StdNew Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaFile___Win32_StdNew_Unicode_Release"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_StdNew_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Unicode_Release"
# PROP Intermediate_Dir "StdNew_Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\AngelicaFile_su.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Release\AngelicaFile_su.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Release\AngelicaFile_su.lib
TargetName=AngelicaFile_su
InputPath=.\StdNew_Unicode_Release\AngelicaFile_su.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AngelicaFile - Win32 DBG Release SBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AngelicaFile___Win32_DBG_Release_SBCS"
# PROP BASE Intermediate_Dir "AngelicaFile___Win32_DBG_Release_SBCS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AngelicaFile___Win32_DBG_Release_SBCS"
# PROP Intermediate_Dir "AngelicaFile___Win32_DBG_Release_SBCS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"DBGRelease\AngelicaFile_drN.lib"
# ADD LIB32 /nologo /out:"DBGRelease\AngelicaFile_drsb.lib"
# Begin Custom Build
TargetPath=.\DBGRelease\AngelicaFile_drsb.lib
TargetName=AngelicaFile_drsb
InputPath=.\DBGRelease\AngelicaFile_drsb.lib
SOURCE="$(InputPath)"

"..\Output\afsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\afsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\afsdk\include\*.h 
	del ..\Output\afsdk\include\afpi.h 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AngelicaFile - Win32 Release"
# Name "AngelicaFile - Win32 Debug"
# Name "AngelicaFile - Win32 Unicode Debug"
# Name "AngelicaFile - Win32 Unicode Release"
# Name "AngelicaFile - Win32 StdNew Debug"
# Name "AngelicaFile - Win32 StdNew Release"
# Name "AngelicaFile - Win32 StdNew Unicode Debug"
# Name "AngelicaFile - Win32 DBG Release"
# Name "AngelicaFile - Win32 StdNew Unicode Release"
# Name "AngelicaFile - Win32 DBG Release SBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\ADir.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ADirImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AFI.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AFileImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AFilePackage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AFileUnicode.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AIniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AMemFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AScriptFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ATextTabFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AWIniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AWScriptFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Header\ADir.h
# End Source File
# Begin Source File

SOURCE=.\Header\ADirImage.h
# End Source File
# Begin Source File

SOURCE=.\Header\AF.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFI.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFileImage.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFilePackage.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFileUnicode.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFPI.h
# End Source File
# Begin Source File

SOURCE=.\Header\AFPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Header\AIniFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AMemFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AScriptFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\ATextTabFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AWIniFile.h
# End Source File
# Begin Source File

SOURCE=.\Header\AWScriptFile.h
# End Source File
# End Group
# End Target
# End Project
