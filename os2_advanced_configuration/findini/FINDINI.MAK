.SUFFIXES: .rc
.SUFFIXES: .def
.SUFFIXES: .lib

all: findini.exe


exeobj=qdeskdir.obj qdeskobj.obj

CC= ICC /Ss /Q /G3 /C /Gm+

.c.obj:
    $(CC)  $*.c > $*.err



findini.exe:findini.obj  $(exeobj)
 LINK386 /NOE /PM:VIO /ST:10000 findini.obj $(exeobj)   ,findini.exe , findini, ,findini.def; >> findini.err

findini.obj: findini.c
 ICC /Ss /Q /G3 /C /Gm+ /Wuse+ $*.c > findini.err

QDESKDIR.obj: QDESKDIR.c
 ICC /Ss /Q /G3 /C /Gm+ /Wuse+ QDESKDIR.c > findini.err

QDESKOBJ.obj: QDESKOBJ.c
 ICC /Ss /Q /G3 /C /Gm+ /Wuse+ QDESKOBJ.c > findini.err
