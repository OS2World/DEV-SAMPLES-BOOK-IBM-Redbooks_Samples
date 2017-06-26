#===================================================================
#
#   Dmcust Make file
#
#===================================================================

include e:\toolkt20\c\samples\ibmsamp.inc

CC   =        icc /c /Ti /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__
LFLAGS     = /DE /NOE /NOD /ALIGN:16 /EXEPACK /M

HEADERS = dmcust.h

#-------------------------------------------------------------------
#   A list of all of the object files
#-------------------------------------------------------------------
ALL_OBJ1 = dmcust.obj


all: dmcust.exe


dmcust.l: dmcust.mak
    echo $(ALL_OBJ1)            > dmcust.l
    echo dmcust.exe           >> dmcust.l
    echo dmcust.map           >> dmcust.l
    echo $(MTLIBS)                >> dmcust.l
    echo dmcust.def           >> dmcust.l




dmcust.res: dmcust.rc dmcust.ico dmcust.h

dmcust.obj: dmcust.c $(HEADERS)

dmcust.exe: $(ALL_OBJ1)  dmcust.def dmcust.l dmcust.res
    $(LINK) /C @dmcust.l
    rc dmcust.res dmcust.exe
