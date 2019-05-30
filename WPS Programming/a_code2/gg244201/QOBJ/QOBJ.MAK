CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm

all: QOBJ.exe

QOBJ.exe: QOBJ.obj
        LINK386 QOBJ,QOBJ,QOBJ,,QOBJ.def;

QOBJ.obj: QOBJ.c
        ICC $(CPARMS) QOBJ.c
