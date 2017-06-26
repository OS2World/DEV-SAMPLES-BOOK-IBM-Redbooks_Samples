#===================================================================
#
#   Dmphone Make file
#
#===================================================================

include e:\toolkt20\c\samples\ibmsamp.inc

CC   =        icc /Kb /c /Ti /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__
LFLAGS     = /DE /NOE /NOD /ALIGN:16 /EXEPACK /M

HEADERS = dmphone.h

#-------------------------------------------------------------------
#   A list of all of the object files
#-------------------------------------------------------------------
ALL_OBJ1 = dmphone.obj


all: dmphone.exe


dmphone.l: dmphone.mak
    echo $(ALL_OBJ1)            > dmphone.l
    echo dmphone.exe           >> dmphone.l
    echo dmphone.map           >> dmphone.l
    echo $(MTLIBS)                >> dmphone.l
    echo dmphone.def           >> dmphone.l




dmphone.res: dmphone.rc dmphone.ico dmphone.h

dmphone.obj: dmphone.c $(HEADERS)

dmphone.exe: $(ALL_OBJ1)  dmphone.def dmphone.l dmphone.res
    $(LINK) /C @dmphone.l
    rc dmphone.res dmphone.exe
