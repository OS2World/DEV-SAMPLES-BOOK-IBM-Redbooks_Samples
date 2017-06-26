
include ibmsamp.inc

CC = icc /c /Ti /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__

all: dropinfo.exe

dropinfo.l: dropinfo.obj
    echo dropinfo.obj    > dropinfo.l
    echo dropinfo.exe   >> dropinfo.l
    echo dropinfo.map   >> dropinfo.l
    echo $(MTLIBS)    >> dropinfo.l
    echo dropinfo.def   >> dropinfo.l

dropinfo.res: dropinfo.rc

dropinfo.obj: dropinfo.c

dropinfo.exe: dropinfo.obj dropinfo.def dropinfo.l dropinfo.res
    $(LINK) /C @dropinfo.l
    rc dropinfo.res dropinfo.exe
