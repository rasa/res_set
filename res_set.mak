# Microsoft Developer Studio Generated NMAKE File, Based on res_set.dsp
!IF "$(CFG)" == ""
CFG=res_set - Win32 Debug
!MESSAGE No configuration specified. Defaulting to res_set - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "res_set - Win32 Release" && "$(CFG)" != "res_set - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "res_set.mak" CFG="res_set - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "res_set - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "res_set - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "res_set - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\res_set.exe"


CLEAN :
	-@erase "$(INTDIR)\res_set.obj"
	-@erase "$(INTDIR)\res_set.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\res_set.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\res_set.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\res_set.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\res_set.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shared.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\res_set.pdb" /machine:I386 /out:"$(OUTDIR)\res_set.exe" 
LINK32_OBJS= \
	"$(INTDIR)\res_set.obj" \
	"$(INTDIR)\res_set.res"

"$(OUTDIR)\res_set.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_set - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\res_set.exe" "$(OUTDIR)\res_set.bsc"


CLEAN :
	-@erase "$(INTDIR)\res_set.obj"
	-@erase "$(INTDIR)\res_set.res"
	-@erase "$(INTDIR)\res_set.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\res_set.bsc"
	-@erase "$(OUTDIR)\res_set.exe"
	-@erase "$(OUTDIR)\res_set.ilk"
	-@erase "$(OUTDIR)\res_set.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\res_set.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\res_set.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\res_set.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\res_set.sbr"

"$(OUTDIR)\res_set.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=sharedd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\res_set.pdb" /debug /machine:I386 /out:"$(OUTDIR)\res_set.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\res_set.obj" \
	"$(INTDIR)\res_set.res"

"$(OUTDIR)\res_set.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("res_set.dep")
!INCLUDE "res_set.dep"
!ELSE 
!MESSAGE Warning: cannot find "res_set.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "res_set - Win32 Release" || "$(CFG)" == "res_set - Win32 Debug"
SOURCE=.\res_set.cpp

!IF  "$(CFG)" == "res_set - Win32 Release"


"$(INTDIR)\res_set.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "res_set - Win32 Debug"


"$(INTDIR)\res_set.obj"	"$(INTDIR)\res_set.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\res_set.rc

"$(INTDIR)\res_set.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

