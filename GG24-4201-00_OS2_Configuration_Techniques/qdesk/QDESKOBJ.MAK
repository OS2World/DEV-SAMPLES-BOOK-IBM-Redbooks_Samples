CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Ti+
CPARMS = /N10 /W3 /C+ /Ka+ /Kb+ /Kc+ /Kg+ /Kp+ /Gm- /Sm


QDESKOBJ.obj: QDESKOBJ.c QDPARM.h
        ICC $(CPARMS) QDESKOBJ.c
