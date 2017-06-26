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

CC         = icc /c /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__


AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
LFLAGS   = /NOE /NOD /ALIGN:16 /EXEPACK /M
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
