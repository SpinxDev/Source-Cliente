# Microsoft Developer Studio Project File - Name="AutoMoveImp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AUTOMOVEIMP - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoMoveImp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoMoveImp.mak" CFG="AUTOMOVEIMP - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoMoveImp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AutoMoveImp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AutoMoveImp - Win32 EditerDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "AutoMoveImp - Win32 EditorRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AutoPFImp/AutoMoveImp", XQQAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoMoveImp - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib\AutoMoveImp.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\  /R/Y/Q	xcopy  bitimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  blockimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  PfConstant.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  MoveAgent.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  ..\lib\*.lib  ..\..\AutoMoveImpExport\lib\ /R/Y/Q	echo "copy to XiYou"	xcopy  MoveAgent.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy  PFConstant.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy ..\lib\*.lib   ..\..\..\XYElement\lib\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AutoMoveImp - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\AutoMoveImp_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\  /R/Y/Q	xcopy  bitimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  blockimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  PfConstant.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  MoveAgent.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  ..\lib\*.lib  ..\..\AutoMoveImpExport\lib\ /R/Y/Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AutoMoveImp - Win32 EditerDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AutoMoveImp___Win32_EditerDebug"
# PROP BASE Intermediate_Dir "AutoMoveImp___Win32_EditerDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AutoMoveImp___Win32_EditerDebug"
# PROP Intermediate_Dir "AutoMoveImp___Win32_EditerDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AUTOMOVE_EDITOR" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AUTOMOVE_EDITOR" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\AutoMoveImp_d.lib"
# ADD LIB32 /nologo /out:"..\lib\AutoMoveImpEditor_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\  /R/Y/Q	xcopy  bitimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  blockimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  PfConstant.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  MoveAgent.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  ..\lib\*.lib  ..\..\AutoMoveImpExport\lib\ /R/Y/Q	echo "copy to XiYou"	xcopy  MoveAgent.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy  PFConstant.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy ..\lib\*.lib   ..\..\..\XYElement\lib\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AutoMoveImp - Win32 EditorRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AutoMoveImp___Win32_EditorRelease"
# PROP BASE Intermediate_Dir "AutoMoveImp___Win32_EditorRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AutoMoveImp___Win32_EditorRelease"
# PROP Intermediate_Dir "AutoMoveImp___Win32_EditorRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AUTOMOVE_EDITOR" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib\AutoMoveImpEditor.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\  /R/Y/Q	xcopy  bitimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  blockimage.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  PfConstant.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  MoveAgent.h  ..\..\AutoMoveImpExport\include\ /R/Y/Q	xcopy  ..\lib\*.lib  ..\..\AutoMoveImpExport\lib\ /R/Y/Q	echo "copy to XiYou"	xcopy  MoveAgent.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy  PFConstant.h  ..\..\..\XYElement\include\AutoMove\  /R/Y /Q	xcopy ..\lib\*.lib   ..\..\..\XYElement\lib\ /R /Y /Q
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AutoMoveImp - Win32 Release"
# Name "AutoMoveImp - Win32 Debug"
# Name "AutoMoveImp - Win32 EditerDebug"
# Name "AutoMoveImp - Win32 EditorRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CluCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\CluGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\CluGraphConnector.cpp
# End Source File
# Begin Source File

SOURCE=.\ClusterAbstraction.cpp
# End Source File
# Begin Source File

SOURCE=.\GAStar.cpp
# End Source File
# Begin Source File

SOURCE=.\GHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\Graph.cpp
# End Source File
# Begin Source File

SOURCE=.\Island.cpp
# End Source File
# Begin Source File

SOURCE=.\LayerMap.cpp
# End Source File
# Begin Source File

SOURCE=.\MapGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\MapNode.cpp
# End Source File
# Begin Source File

SOURCE=.\MAStar.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveMap.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiCluGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\OptimizePath.cpp
# End Source File
# Begin Source File

SOURCE=.\PfClu.cpp
# End Source File
# Begin Source File

SOURCE=.\PfConstant.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bitimage.h
# End Source File
# Begin Source File

SOURCE=.\blockimage.h
# End Source File
# Begin Source File

SOURCE=.\CluCommon.h
# End Source File
# Begin Source File

SOURCE=.\CluGraph.h
# End Source File
# Begin Source File

SOURCE=.\CluGraphConnector.h
# End Source File
# Begin Source File

SOURCE=.\ClusterAbstraction.h
# End Source File
# Begin Source File

SOURCE=.\GAStar.h
# End Source File
# Begin Source File

SOURCE=.\GHeap.h
# End Source File
# Begin Source File

SOURCE=.\Graph.h
# End Source File
# Begin Source File

SOURCE=.\Island.h
# End Source File
# Begin Source File

SOURCE=.\JointList.h
# End Source File
# Begin Source File

SOURCE=.\LayerMap.h
# End Source File
# Begin Source File

SOURCE=.\MapGraph.h
# End Source File
# Begin Source File

SOURCE=.\MapNode.h
# End Source File
# Begin Source File

SOURCE=.\MAStar.h
# End Source File
# Begin Source File

SOURCE=.\MoveAgent.h
# End Source File
# Begin Source File

SOURCE=.\MoveMap.h
# End Source File
# Begin Source File

SOURCE=.\MultiCluGraph.h
# End Source File
# Begin Source File

SOURCE=.\OptimizePath.h
# End Source File
# Begin Source File

SOURCE=.\PfClu.h
# End Source File
# Begin Source File

SOURCE=.\PfConstant.h
# End Source File
# End Group
# End Target
# End Project
