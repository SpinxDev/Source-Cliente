# Microsoft Developer Studio Project File - Name="Angelica3D" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Angelica3D - Win32 StdNew Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Angelica3D.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Angelica3D.mak" CFG="Angelica3D - Win32 StdNew Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Angelica3D - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 StdNew Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 StdNew Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 StdNew Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 StdNew Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Angelica3D - Win32 DBG Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Angelica2/Angelica3D", BIDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Angelica3D - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=.\Release\Angelica3D.lib
TargetName=Angelica3D
InputPath=.\Release\Angelica3D.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\Angelica3D_d.lib"
# Begin Custom Build
TargetPath=.\Debug\Angelica3D_d.lib
TargetName=Angelica3D_d
InputPath=.\Debug\Angelica3D_d.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Angelica3D___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode_Debug"
# PROP Intermediate_Dir "Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\Angelica3D_d.lib"
# ADD LIB32 /nologo /out:"Unicode_Debug\Angelica3D_ud.lib"
# Begin Custom Build
TargetPath=.\Unicode_Debug\Angelica3D_ud.lib
TargetName=Angelica3D_ud
InputPath=.\Unicode_Debug\Angelica3D_ud.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Angelica3D___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Unicode_Release"
# PROP Intermediate_Dir "Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Unicode_Release\Angelica3D_u.lib"
# Begin Custom Build
TargetPath=.\Unicode_Release\Angelica3D_u.lib
TargetName=Angelica3D_u
InputPath=.\Unicode_Release\Angelica3D_u.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 StdNew Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Angelica3D___Win32_StdNew_Debug"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_StdNew_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Debug"
# PROP Intermediate_Dir "StdNew_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\Angelica3D_d.lib"
# ADD LIB32 /nologo /out:"StdNew_Debug\Angelica3D_sd.lib"
# Begin Custom Build
TargetPath=.\StdNew_Debug\Angelica3D_sd.lib
TargetName=Angelica3D_sd
InputPath=.\StdNew_Debug\Angelica3D_sd.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 StdNew Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Angelica3D___Win32_StdNew_Release"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_StdNew_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Release"
# PROP Intermediate_Dir "StdNew_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"StdNew_Release\Angelica3D_s.lib"
# Begin Custom Build
TargetPath=.\StdNew_Release\Angelica3D_s.lib
TargetName=Angelica3D_s
InputPath=.\StdNew_Release\Angelica3D_s.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 StdNew Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Angelica3D___Win32_StdNew_Unicode_Debug"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_StdNew_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StdNew_Unicode_Debug"
# PROP Intermediate_Dir "StdNew_Unicode_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_A_FORBID_NEWDELETE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNew_Debug\Angelica3D_sd.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Debug\Angelica3D_sud.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Debug\Angelica3D_sud.lib
TargetName=Angelica3D_sud
InputPath=.\StdNew_Unicode_Debug\Angelica3D_sud.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 StdNew Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Angelica3D___Win32_StdNew_Unicode_Release"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_StdNew_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StdNew_Unicode_Release"
# PROP Intermediate_Dir "StdNew_Unicode_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\Angelica3D_su.lib"
# ADD LIB32 /nologo /out:"StdNew_Unicode_Release\Angelica3D_su.lib"
# Begin Custom Build
TargetPath=.\StdNew_Unicode_Release\Angelica3D_su.lib
TargetName=Angelica3D_su
InputPath=.\StdNew_Unicode_Release\Angelica3D_su.lib
SOURCE="$(InputPath)"

"..\Output\a3dsdk\lib\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy libary into sdk folder 
	copy $(TargetPath) ..\Output\a3dsdk\lib\$(TargetName).lib 
	echo copy headers into sdk folder 
	copy .\header\*.h ..\Output\a3dsdk\include\*.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Angelica3D - Win32 DBG Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Angelica3D___Win32_DBG_Release"
# PROP BASE Intermediate_Dir "Angelica3D___Win32_DBG_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DBGRelease"
# PROP Intermediate_Dir "DBGRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\header" /I "..\AngelicaMedia\Header" /I "..\Output\3rdsdk\include" /I "..\AngelicaFile\Header" /I "..\AngelicaCommon\Header" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"StdNewUnicodeDBGRelease\Angelica3D_su.lib"
# ADD LIB32 /nologo /out:"DBGRelease\Angelica3D_dr.lib"

!ENDIF 

# Begin Target

# Name "Angelica3D - Win32 Release"
# Name "Angelica3D - Win32 Debug"
# Name "Angelica3D - Win32 Unicode Debug"
# Name "Angelica3D - Win32 Unicode Release"
# Name "Angelica3D - Win32 StdNew Debug"
# Name "Angelica3D - Win32 StdNew Release"
# Name "Angelica3D - Win32 StdNew Unicode Debug"
# Name "Angelica3D - Win32 StdNew Unicode Release"
# Name "Angelica3D - Win32 DBG Release"
# Begin Group "Effect Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DBillBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBillBoard.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DDecal.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFlash.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFlash.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DGFXCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DGFXCollector.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DGFXMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DGFXMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DGraphicsFX.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DGraphicsFX.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLamp.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLamp.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLensFlare.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLightning.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLightning.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPArray.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DRain.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DRain.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSnow.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSnow.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DStars.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DStars.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSuperDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSuperDecal.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSuperSpray.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSuperSpray.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrainMark.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrainMark.h
# End Source File
# End Group
# Begin Group "2D Graphics Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A2DSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A2DSprite.h
# End Source File
# Begin Source File

SOURCE=.\Source\A2DSpriteBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A2DSpriteBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\A2DSpriteItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A2DSpriteItem.h
# End Source File
# Begin Source File

SOURCE=.\Source\A2DSpriteTexMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A2DSpriteTexMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A2DSpriteTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A2DSpriteTextures.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCursor.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFont.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFontMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFontMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DGDI.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DGDI.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DString.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DString.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DStringSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DStringSet.h
# End Source File
# End Group
# Begin Group "Model Files"

# PROP Default_Filter ""
# Begin Group "Skin Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DAbsTrackMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DAbsTrackMaker.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DBone.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBone.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DBoneController.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBoneController.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DClothMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DClothMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DController.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DController.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DIKData.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DIKData.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DIKSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DIKSolver.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DJoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DJoint.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMeshBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMeshBase.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMorphMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMorphMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMorphRigidMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMorphRigidMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMorphSkinMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMorphSkinMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMuscleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMuscleMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMuscleOrgan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMuscleOrgan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DRigidMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DRigidMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkeleton.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkeleton.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkin.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkin.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinMeshMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinMeshMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinModel.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinModelAct.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinModelAct.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinModelAux.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinModelAux.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinModelMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinModelMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkinRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkinRender.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSuppleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSuppleMesh.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTrackData.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTrackMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTrackMan.h
# End Source File
# End Group
# Begin Group "MOX Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFrame.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DIBLMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DIBLMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLitModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLitModel.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLitModelRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLitModelRender.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMesh.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMeshCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMeshCollector.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMeshMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMeshMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMeshSorter.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMeshSorter.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DModel.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DModelCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DModelCollector.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DModelMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DModelMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMoxMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMoxMan.h
# End Source File
# End Group
# Begin Group "Image Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DImgModel.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DImgModel.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DImgModelMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DImgModelMan.h
# End Source File
# End Group
# End Group
# Begin Group "Light Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLight.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLightingEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLightingEngine.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DLightMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DLightMan.h
# End Source File
# End Group
# Begin Group "+ Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DCoordinate.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFlatCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPI.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPlants.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DRenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DShadowMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSky.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkyGradient.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSkySphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSurfaceMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DVectorComp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DVertexCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DWireCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DWorldCDS.cpp
# End Source File
# End Group
# Begin Group "+ Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Header\A3D.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCDS.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DClassID.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DConfig.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DControl.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCoordinate.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DDevice.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DDevObject.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DEngine.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFlatCollector.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFuncs.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DGeometry.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMacros.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DObject.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPI.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPlants.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DRenderTarget.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DShadowMap.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSky.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkyBox.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkyGradient.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSkySphere.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DStream.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSurface.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSurfaceMan.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTypes.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVector.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVertex.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVertexCollector.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DWireCollector.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DWorld.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DWorldCDS.h
# End Source File
# End Group
# Begin Group "Scene And Terrain Files"

# PROP Default_Filter ""
# Begin Group "Collision"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DESP.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DESP.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DESPFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DESPMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DKDTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DKDTree.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DKDTreeFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DSceneHull.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSceneHull.h
# End Source File
# End Group
# Begin Group "Visible"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DBSP.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBSP.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBSPFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DHorizonCull.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DHorizonCull.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPortal.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPortalFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DRouteVSS.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DRouteVSS.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DSceneVisible.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVSSCommon.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DVSSRouteLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVSSRouteLoader.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\A3DIBLScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DIBLScene.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DScene.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2Blk.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2Blk.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2Cull.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2CullHC.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2CullHC.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2Env.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2Env.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2File.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2Loader.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2LoaderA.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2LoaderA.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2LoaderB.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2LoaderB.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2LOD.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2LOD.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrain2Render.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2RenderAux.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrain2Trace.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrainOutline.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrainOutline.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrainWater.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrainWater.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrainWaterRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrainWaterRender.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTerrainWaterSimpleAlphaRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTerrainWaterSimpleAlphaRender.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DWaterArea.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DWaterArea.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DWaterWave.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DWaterWave.h
# End Source File
# End Group
# Begin Group "Texture And Shader Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DCubeTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCubeTexture.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPerlinNormalMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPerlinNormalMap.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DPixelShader.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DPixelShader.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DShader.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DShader.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DShaderMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DShaderMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTexture.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTextureMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTextureMan.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DVertexShader.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVertexShader.h
# End Source File
# Begin Source File

SOURCE=.\Header\A3DVSDef.h
# End Source File
# End Group
# Begin Group "Assist Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DBezierPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBezierPoint.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DBezierSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DBezierSegment.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCollision.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DHermiteSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DHermiteSpline.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DMathUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DMathUtility.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DTrace.h
# End Source File
# End Group
# Begin Group "Camera And Viewport"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\A3DAdditionalView.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DAdditionalView.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCamera.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DCameraBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DCameraBase.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DFrustum.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DOrthoCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DOrthoCamera.h
# End Source File
# Begin Source File

SOURCE=.\Source\A3DViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\Header\A3DViewport.h
# End Source File
# End Group
# End Target
# End Project
