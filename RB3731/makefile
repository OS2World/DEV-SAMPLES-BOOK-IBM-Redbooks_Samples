all: boota.exe

boota.obj : $*.c
           cl -Ox -c -Zi -W4 -G2s  $*.c

boota.exe : $*.obj
           link /NOD boota,,,os2286 llibcep;

