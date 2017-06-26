
include ibmsamp.inc

CC = icc /c /Ti /Gd- /Se /Re /ss /Ms /Gm+ /D__MIG_LIB__

all: server.exe

server.l: server.mak
    echo server.obj    > server.l
    echo server.exe   >> server.l
    echo server.map   >> server.l
    echo $(MTLIBS)    >> server.l
    echo server.def   >> server.l


server.res: server.rc find.h

server.obj: server.c

server.exe: server.obj server.def server.l server.res
    $(LINK) /C @server.l
    rc server.res server.exe
