CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm

all: INI2DESK.exe

INI2DESK.exe: INI2DESK.obj ..\qdesk\qdeskobj.obj
        LINK386 INI2DESK+..\QDESK\QDESKOBJ,INI2DESK,INI2DESK,,INI2DESK.DEF;

INI2DESK.obj: INI2DESK.c
        ICC $(CPARMS) INI2DESK.c
