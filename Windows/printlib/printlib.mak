# Microsoft Developer Studio Generated NMAKE File, Based on printlib.dsp
!IF "$(CFG)" == ""
CFG=printlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to printlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "printlib - Win32 Release" && "$(CFG)" != "printlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "printlib.mak" CFG="printlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "printlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "printlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "printlib - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

ALL : "$(OUTDIR)\printlib.lib"


CLEAN :
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxmoves.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\fontconfig.obj"
	-@erase "$(INTDIR)\fontserver.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\glwindowdisplay.obj"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\openglfont.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\stix.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\Style.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(OUTDIR)\printlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "NDEBUG" /D "__STDC_HEADERS" /D "STDC_HEADERS" /D "__STDC__" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /Fp"$(INTDIR)\printlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\printlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\printlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxmoves.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\fontconfig.obj" \
	"$(INTDIR)\fontserver.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\glwindowdisplay.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\openglfont.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\stix.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\Style.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj"

"$(OUTDIR)\printlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "printlib - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\ 
# End Custom Macros

ALL : "$(OUTDIR)\printlib.lib"


CLEAN :
	-@erase "$(INTDIR)\absboxes.obj"
	-@erase "$(INTDIR)\abspictures.obj"
	-@erase "$(INTDIR)\actions.obj"
	-@erase "$(INTDIR)\applicationapi.obj"
	-@erase "$(INTDIR)\attributes.obj"
	-@erase "$(INTDIR)\attrpresent.obj"
	-@erase "$(INTDIR)\boxmoves.obj"
	-@erase "$(INTDIR)\boxpositions.obj"
	-@erase "$(INTDIR)\boxrelations.obj"
	-@erase "$(INTDIR)\buildboxes.obj"
	-@erase "$(INTDIR)\buildlines.obj"
	-@erase "$(INTDIR)\changeabsbox.obj"
	-@erase "$(INTDIR)\changepresent.obj"
	-@erase "$(INTDIR)\checkaccess.obj"
	-@erase "$(INTDIR)\content.obj"
	-@erase "$(INTDIR)\createabsbox.obj"
	-@erase "$(INTDIR)\createpages.obj"
	-@erase "$(INTDIR)\dictionary.obj"
	-@erase "$(INTDIR)\displaybox.obj"
	-@erase "$(INTDIR)\displayview.obj"
	-@erase "$(INTDIR)\documentapi.obj"
	-@erase "$(INTDIR)\epshandler.obj"
	-@erase "$(INTDIR)\exceptions.obj"
	-@erase "$(INTDIR)\externalref.obj"
	-@erase "$(INTDIR)\fileaccess.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\fontconfig.obj"
	-@erase "$(INTDIR)\fontserver.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\gifhandler.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\glwindowdisplay.obj"
	-@erase "$(INTDIR)\hyphen.obj"
	-@erase "$(INTDIR)\inites.obj"
	-@erase "$(INTDIR)\jpeghandler.obj"
	-@erase "$(INTDIR)\labelalloc.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\openglfont.obj"
	-@erase "$(INTDIR)\pagecommands.obj"
	-@erase "$(INTDIR)\picture.obj"
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\pnghandler.obj"
	-@erase "$(INTDIR)\presrules.obj"
	-@erase "$(INTDIR)\presvariables.obj"
	-@erase "$(INTDIR)\pschemaapi.obj"
	-@erase "$(INTDIR)\quit.obj"
	-@erase "$(INTDIR)\readpivot.obj"
	-@erase "$(INTDIR)\readprs.obj"
	-@erase "$(INTDIR)\readstr.obj"
	-@erase "$(INTDIR)\readtra.obj"
	-@erase "$(INTDIR)\references.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\schemas.obj"
	-@erase "$(INTDIR)\schtrad.obj"
	-@erase "$(INTDIR)\stix.obj"
	-@erase "$(INTDIR)\structschema.obj"
	-@erase "$(INTDIR)\Style.obj"
	-@erase "$(INTDIR)\tableH.obj"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\uconvert.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\ustring.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\windowdisplay.obj"
	-@erase "$(INTDIR)\xbmhandler.obj"
	-@erase "$(INTDIR)\xpmhandler.obj"
	-@erase "$(OUTDIR)\printlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /ZI /Od /I "..\..\thotlib\include" /I "..\..\thotlib\internals\h" /I "..\..\thotlib\internals\f" /I "..\..\thotlib\internals\var" /I "..\..\libjpeg" /I "..\..\libpng" /I "..\..\libpng\zlib" /D "_DEBUG" /D "_WINDOWS_DLL" /D "STDC_HEADERS" /D "__STDC__" /D "_WIN_PRINT" /D "WIN32" /D "_WINDOWS" /D "_WINGUI" /Fp"$(INTDIR)\printlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\printlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\printlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\absboxes.obj" \
	"$(INTDIR)\abspictures.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\applicationapi.obj" \
	"$(INTDIR)\attributes.obj" \
	"$(INTDIR)\attrpresent.obj" \
	"$(INTDIR)\boxmoves.obj" \
	"$(INTDIR)\boxpositions.obj" \
	"$(INTDIR)\boxrelations.obj" \
	"$(INTDIR)\buildboxes.obj" \
	"$(INTDIR)\buildlines.obj" \
	"$(INTDIR)\changeabsbox.obj" \
	"$(INTDIR)\changepresent.obj" \
	"$(INTDIR)\checkaccess.obj" \
	"$(INTDIR)\content.obj" \
	"$(INTDIR)\createabsbox.obj" \
	"$(INTDIR)\createpages.obj" \
	"$(INTDIR)\dictionary.obj" \
	"$(INTDIR)\displaybox.obj" \
	"$(INTDIR)\displayview.obj" \
	"$(INTDIR)\documentapi.obj" \
	"$(INTDIR)\epshandler.obj" \
	"$(INTDIR)\exceptions.obj" \
	"$(INTDIR)\externalref.obj" \
	"$(INTDIR)\fileaccess.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\fontconfig.obj" \
	"$(INTDIR)\fontserver.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\gifhandler.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\glwindowdisplay.obj" \
	"$(INTDIR)\hyphen.obj" \
	"$(INTDIR)\inites.obj" \
	"$(INTDIR)\jpeghandler.obj" \
	"$(INTDIR)\labelalloc.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\openglfont.obj" \
	"$(INTDIR)\pagecommands.obj" \
	"$(INTDIR)\picture.obj" \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\pnghandler.obj" \
	"$(INTDIR)\presrules.obj" \
	"$(INTDIR)\presvariables.obj" \
	"$(INTDIR)\pschemaapi.obj" \
	"$(INTDIR)\quit.obj" \
	"$(INTDIR)\readpivot.obj" \
	"$(INTDIR)\readprs.obj" \
	"$(INTDIR)\readstr.obj" \
	"$(INTDIR)\readtra.obj" \
	"$(INTDIR)\references.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\schemas.obj" \
	"$(INTDIR)\schtrad.obj" \
	"$(INTDIR)\stix.obj" \
	"$(INTDIR)\structschema.obj" \
	"$(INTDIR)\Style.obj" \
	"$(INTDIR)\tableH.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\uconvert.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\ustring.obj" \
	"$(INTDIR)\windowdisplay.obj" \
	"$(INTDIR)\xbmhandler.obj" \
	"$(INTDIR)\xpmhandler.obj"

"$(OUTDIR)\printlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("printlib.dep")
!INCLUDE "printlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "printlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "printlib - Win32 Release" || "$(CFG)" == "printlib - Win32 Debug"
SOURCE=..\..\thotlib\view\absboxes.c

"$(INTDIR)\absboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\abspictures.c

"$(INTDIR)\abspictures.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\actions.c

"$(INTDIR)\actions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Editing\applicationapi.c

"$(INTDIR)\applicationapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\attributes.c

"$(INTDIR)\attributes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\attrpresent.c

"$(INTDIR)\attrpresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxmoves.c

"$(INTDIR)\boxmoves.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxpositions.c

"$(INTDIR)\boxpositions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\boxrelations.c

"$(INTDIR)\boxrelations.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\buildboxes.c

"$(INTDIR)\buildboxes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\buildlines.c

"$(INTDIR)\buildlines.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\changeabsbox.c

"$(INTDIR)\changeabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\changepresent.c

"$(INTDIR)\changepresent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\checkaccess.c

"$(INTDIR)\checkaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\content.c

"$(INTDIR)\content.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\createabsbox.c

"$(INTDIR)\createabsbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\createpages.c

"$(INTDIR)\createpages.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\content\dictionary.c

"$(INTDIR)\dictionary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\displaybox.c

"$(INTDIR)\displaybox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\View\displayview.c

"$(INTDIR)\displayview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\Document\documentapi.c

"$(INTDIR)\documentapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\epshandler.c

"$(INTDIR)\epshandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\exceptions.c

"$(INTDIR)\exceptions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\externalref.c

"$(INTDIR)\externalref.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\fileaccess.c

"$(INTDIR)\fileaccess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\font.c

"$(INTDIR)\font.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\fontconfig.c

"$(INTDIR)\fontconfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\fontserver.c

"$(INTDIR)\fontserver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\frame.c

"$(INTDIR)\frame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\gifhandler.c

"$(INTDIR)\gifhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\global.c

"$(INTDIR)\global.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\glwindowdisplay.c

"$(INTDIR)\glwindowdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\hyphen.c

"$(INTDIR)\hyphen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\inites.c

"$(INTDIR)\inites.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\jpeghandler.c

"$(INTDIR)\jpeghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\labelalloc.c

"$(INTDIR)\labelalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\language.c

"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\memory.c

"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\message.c

"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\openglfont.c

"$(INTDIR)\openglfont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\pagecommands.c

"$(INTDIR)\pagecommands.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\picture.c

"$(INTDIR)\picture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\platform.c

"$(INTDIR)\platform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\pnghandler.c

"$(INTDIR)\pnghandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presrules.c

"$(INTDIR)\presrules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\presvariables.c

"$(INTDIR)\presvariables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\presentation\pschemaapi.c

"$(INTDIR)\pschemaapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\quit.c

"$(INTDIR)\quit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readpivot.c

"$(INTDIR)\readpivot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readprs.c

"$(INTDIR)\readprs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readstr.c

"$(INTDIR)\readstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\readtra.c

"$(INTDIR)\readtra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\references.c

"$(INTDIR)\references.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\registry.c

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schemas.c

"$(INTDIR)\schemas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\document\schtrad.c

"$(INTDIR)\schtrad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\dialogue\stix.c

"$(INTDIR)\stix.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\structschema.c

"$(INTDIR)\structschema.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Thotlib\presentation\Style.c

"$(INTDIR)\Style.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\tableH.c

"$(INTDIR)\tableH.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\tree\tree.c

"$(INTDIR)\tree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\uconvert.c

"$(INTDIR)\uconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\base\units.c

"$(INTDIR)\units.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\unicode\ustring.c

"$(INTDIR)\ustring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\view\windowdisplay.c

"$(INTDIR)\windowdisplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\xbmhandler.c

"$(INTDIR)\xbmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\thotlib\image\xpmhandler.c

"$(INTDIR)\xpmhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

