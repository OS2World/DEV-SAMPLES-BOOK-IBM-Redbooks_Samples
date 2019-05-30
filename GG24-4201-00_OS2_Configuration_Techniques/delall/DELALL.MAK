CPARMS = /Fm+ /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm

all: DELALL.exe

DELALL.exe: DELALL.obj
        LINK386 DELALL,DELALL,DELALL,,DELALL.def;

DELALL.obj: DELALL.c
        ICC $(CPARMS) DELALL.c
