
include e:\toolkt20\c\samples\ibmsamp.inc

CC = icc /c /Ti /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__

all: dmorder.exe

dmorder.l: dmorder.mak
    echo dmorder.obj     > dmorder.l
    echo dmorder.exe    >> dmorder.l
    echo dmorder.map    >> dmorder.l
    echo $(MTLIBS)  >> dmorder.l
    echo dmorder.def    >> dmorder.l

dmorder.res: dmorder.rc dmorder.h

dmorder.obj: dmorder.c dmorder.h

dmorder.exe: dmorder.obj dmorder.def dmorder.l dmorder.res
    $(LINK) /C @dmorder.l
    rc dmorder.res dmorder.exe

