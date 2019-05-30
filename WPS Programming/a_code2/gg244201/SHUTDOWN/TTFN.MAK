.SUFFIXES: .rc
.SUFFIXES: .def
.SUFFIXES: .lib

all: TTFN.exe

reses: TTFN.res

exeobj=

CC= ICC /Ss /Q /G3 /C /Gm+

.c.obj:
    $(CC)  $*.c > $*.err


TTFN.res: TTFN.rc TTFN.dlg TTFN.h
   rc -r TTFN.rc >> TTFN.err

TTFN.exe:TTFN.obj TTFN.res
 LINK386 /PM:PM  /ST:10000 TTFN.obj, TTFN.exe, TTFN, ,TTFN.def; >> TTFN.err
    rc TTFN.res TTFN.exe >> TTFN.err


TTFN.obj: TTFN.c
 ICC /Ss /Q /G3 /C /Gm+ $*.c > TTFN.err

