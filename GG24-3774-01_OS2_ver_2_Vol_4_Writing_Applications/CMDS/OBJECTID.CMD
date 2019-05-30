/* OBJECTID.CMD: Sample code using REXXUTIL's SysIni function.          */
/* Routine will display the OBJECTIDs known to the WorkPlace Shell      */



'@ECHO OFF'
/* Load REXXUTIL */
call rxfuncadd sysloadfuncs, rexxutil, sysloadfuncs
call sysloadfuncs

/* List ObjectIds */
App='PM_Workplace:Location'

/* call SysIni 'USER', App, 'All:', 'Keys' */
call SysIni 'BOTH', App, 'All:', 'Keys'
if Result \= 'ERROR:' then do
   Call SysCls
   Say '';Say '';
   Say 'Listing ObjectId information (value enclosed in quotes)'; Say '';
   parse value SysTextScreenSize() with row col
   j=row-10
   Do i=1 to Keys.0
      If trunc(i/j)==i/j Then Do
         Say '';Say 'Press any key to show next screen...'
         key=SysGetKey()
         Call SysCls
         Say '';Say '';
         Say 'Listing ObjectId information (value enclosed in quotes)'; Say '';
      End
      Say '"'Keys.i'"'
   End
End
Else Say 'Error querying for' App
Call SysDropFuncs
Exit


