CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm

all: QFOLDER.exe

QFOLDER.exe: QFOLDER.obj
        LINK386 QFOLDER,QFOLDER,QFOLDER,,QFOLDER.def;

QFOLDER.obj: QFOLDER.c
        ICC $(CPARMS) QFOLDER.c
