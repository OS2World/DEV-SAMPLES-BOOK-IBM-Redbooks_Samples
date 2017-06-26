#===================================================================
#
#   Hello Make file
#
#===================================================================

#===================================================================
#
#   Sample application makefile,common definitions for the IBM C
#   compiler environment
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#
# Compile switchs  that are enabled
# /c      compile don't link
# /Gm+    use the multi-threaded libraries
# /ss     allow  "//" for comment lines
# /Ms     use the system calling convention and not optilink as the default
# /Gd-    Disable optimization
# /Se     allow cset  extensions
#
#Note: /D__MIG_LIB__ will be coming out after LA and code should be changed
#      accordingly.
#

CC         = icc /c /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__ /Ti


AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
LFLAGS   = /NOE /NOD /ALIGN:16 /EXEPACK /M /De
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386
STLIBS  = DDE4SBS + OS2386
MTLIBS  = DDE4MBS + DDE4MBM  + os2386
DLLLIBS = DDE4NBS + os2386
VLIBS   = DDE4SBS + vdh + os2386

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    $(CC) -Fo$*.obj $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf /W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r -p -x $*.rc

CC         = icc /c /Ge /Gd- /Se /Re /ss /Gm+ /D__MIG_LIB__ /Ti

HEADERS = swapsize.h

#-------------------------------------------------------------------
#   A list of all of the object files
#-------------------------------------------------------------------
ALL_OBJ1 = swapsize.obj


all: swapsize.exe


swapsize.l: swapsize.mak
    echo $(ALL_OBJ1)            > swapsize.l
    echo swapsize.exe           >> swapsize.l
    echo swapsize.map           >> swapsize.l
    echo $(LIBS)                >> swapsize.l
    echo swapsize.def           >> swapsize.l




swapsize.res: swapsize.rc swapsize.ico swapsize.h

swapsize.obj: swapsize.c $(HEADERS)

swapsize.exe: $(ALL_OBJ1)  swapsize.def swapsize.l swapsize.res
    $(LINK) @swapsize.l
    rc swapsize.res swapsize.exe
