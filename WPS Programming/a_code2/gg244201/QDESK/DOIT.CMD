setlocal
set SMINCLUDE=.;\toolkt21\sc
set SMTMP=h:\
set SMEMIT=ih;h;sc;c;def
goto lbl%1
:lbl
:lbl1
rem sc -v -r qdesk.csc
sc qdesk.csc
if errorlevel 1 goto endall
:lbl2
REM icc /c /Gd- /Se /Re /ss /Ms /Gm+ /Kb+ /N100 /Ge- qdesk.c
icc /c /Ge- /Gd- /Se /Re /ss /Ms /Gm+ qdesk.c
if errorlevel 1 goto endall
:lbl3
link386 /noi /map /nol /nod /exepack /packcode /packdata /align:16 qdesk.obj,qdesk.dll,,DDE4MBS + os2386 som.lib,qdesk.def
REM link386 /noi /map /nol /nod /exepack /packcode /packdata /align:16 qdesk.obj,qdesk.dll,,som + os2386 + dde4sbs,qdesk.def
:endall
endlocal
