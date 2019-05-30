/* Rexx program to insert ini data */

/* register REXX utility functions */
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

/* Change system colors */

call SysIni 'USER' , 'PM_Colors' , 'Background' , '100 95  120' || '00'x

exit
