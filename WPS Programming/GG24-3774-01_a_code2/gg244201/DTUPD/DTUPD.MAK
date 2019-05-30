CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm

all: DTUPD.exe

DTUPD.exe: DTUPD.obj
        LINK386 DTUPD,DTUPD,DTUPD,,DTUPD.def;

DTUPD.obj: DTUPD.c
        ICC $(CPARMS) DTUPD.c
