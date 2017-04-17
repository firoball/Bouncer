# Microsoft Developer Studio Project File - Name="Tutorial_10_CustomJoints" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Tutorial_10_CustomJoints - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tutorial10.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tutorial10.mak" CFG="Tutorial_10_CustomJoints - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tutorial_10_CustomJoints - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Tutorial_10_CustomJoints - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Tutorial_10_CustomJoints - Win32 ReleaseDouble" (based on "Win32 (x86) Console Application")
!MESSAGE "Tutorial_10_CustomJoints - Win32 DebugDouble" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Tutorial_10_CustomJoints - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NEWTON_USE_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\Tutorial_10_CustomJoints.exe" /libpath:"..\gl" /libpath:"..\..\sdk\lib_mt"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "..\gl\*.dll" "..\bin\*.*"  /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Tutorial_10_CustomJoints - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc" /out:"..\bin\Tutorial_10_CustomJoints.exe" /pdbtype:sept /libpath:"..\gl" /libpath:"..\..\sdk\dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "..\gl\*.dll" "..\bin\*.*"  /F /Y	xcopy "..\..\sdk\dll\*.dll" "..\bin\*.*"  /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Tutorial_10_CustomJoints - Win32 ReleaseDouble"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Tutorial_10_CustomJoints___Win32_ReleaseDouble"
# PROP BASE Intermediate_Dir "Tutorial_10_CustomJoints___Win32_ReleaseDouble"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDouble"
# PROP Intermediate_Dir "ReleaseDouble"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NEWTON_USE_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NEWTON_USE_LIB" /D "__USE_DOUBLE_PRECISION__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\Tutorial_10_CustomJoints.exe" /libpath:"..\gl" /libpath:"..\..\sdk\lib_st"
# ADD LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\Tutorial_10_CustomJoints.exe" /libpath:"..\gl" /libpath:"..\..\sdk\lib_mt_double"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "..\gl\*.dll" "..\bin\*.*"  /F /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Tutorial_10_CustomJoints - Win32 DebugDouble"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Tutorial_10_CustomJoints___Win32_DebugDouble"
# PROP BASE Intermediate_Dir "Tutorial_10_CustomJoints___Win32_DebugDouble"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DebugDouble"
# PROP Intermediate_Dir "DebugDouble"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_NEWTON_USE_LIB" /D "__USE_DOUBLE_PRECISION__" /YX /FD /c
# ADD CPP /nologo /MTd /W4 /GX /Zi /Od /I "..\..\sdk\customJoints" /I "..\toolbox" /I "..\common" /I "..\\" /I "..\..\sdk" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__USE_DOUBLE_PRECISION__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\Tutorial_10_CustomJoints.exe" /libpath:"..\gl" /libpath:"..\..\sdk\lib_st_double"
# ADD LINK32 newton.lib GLAUX.LIB glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin\Tutorial_10_CustomJoints.exe" /libpath:"..\gl" /libpath:"..\..\sdk\dll_double"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "..\gl\*.dll" "..\bin\*.*"  /F /Y	xcopy "..\..\sdk\dll_double\*.dll" "..\bin\*.*"  /F /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Tutorial_10_CustomJoints - Win32 Release"
# Name "Tutorial_10_CustomJoints - Win32 Debug"
# Name "Tutorial_10_CustomJoints - Win32 ReleaseDouble"
# Name "Tutorial_10_CustomJoints - Win32 DebugDouble"
# Begin Group "customJoints"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomBallAndSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomBallAndSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomConeLimitedBallAndSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomConeLimitedBallAndSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomCorkScrew.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomCorkScrew.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomDryRollingFriction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomDryRollingFriction.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomGear.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomGear.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomHinge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomHinge.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomPoweredBallAndSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomPoweredBallAndSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomPulley.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomPulley.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomRayCastCar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomRayCastCar.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomSlider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomSlider.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomUniversal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomUniversal.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomUpVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomUpVector.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomWormGear.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\CustomWormGear.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\NewtonCustomJoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\customJoints\NewtonCustomJoint.h
# End Source File
# End Group
# Begin Group "toolbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\toolBox\dBaseHierarchy.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dBaseHierarchy.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dChunkParsel.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dChunkParsel.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dCRC.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dCRC.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dGeometry.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dList.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dMatrix.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dQuaternion.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dTree.cpp
# End Source File
# Begin Source File

SOURCE=..\toolBox\dTree.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dVector.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\stdafx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CharaterControl.cpp
# End Source File
# Begin Source File

SOURCE=.\CharaterControl.h
# End Source File
# Begin Source File

SOURCE=..\toolBox\dGeometry.h
# End Source File
# Begin Source File

SOURCE=..\gl\Glut.h
# End Source File
# Begin Source File

SOURCE=..\common\HiResTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\common\HiResTimer.h
# End Source File
# Begin Source File

SOURCE=.\JointsTutorial.cpp
# End Source File
# Begin Source File

SOURCE=.\JointsTutorial.h
# End Source File
# Begin Source File

SOURCE=..\common\LevelPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LevelPrimitive.h
# End Source File
# Begin Source File

SOURCE=.\Materials.cpp
# End Source File
# Begin Source File

SOURCE=.\Materials.h
# End Source File
# Begin Source File

SOURCE=..\common\MousePick.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MousePick.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\Newton.h
# End Source File
# Begin Source File

SOURCE=..\common\OpenGlUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\OpenGlUtil.h
# End Source File
# Begin Source File

SOURCE=..\common\RenderPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=..\common\RenderPrimitive.h
# End Source File
# Begin Source File

SOURCE=.\tutorial.cpp
# End Source File
# Begin Source File

SOURCE=.\tutorial.h
# End Source File
# End Target
# End Project
