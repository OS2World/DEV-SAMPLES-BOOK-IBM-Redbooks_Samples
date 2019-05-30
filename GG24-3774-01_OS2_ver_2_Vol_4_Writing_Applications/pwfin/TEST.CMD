/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysCreateObject( "PWFinanceFile", "FinanceFile", "<WP_DESKTOP>", "PASSWORD=wps;OBJECTID=<MyFinanceFile>")

if RetCode then
   say 'PWFinanceFile Object created'
else do
   say 'Error creating object'
   exit(1)
   end

RetCode = SysCreateObject( "WPProgram", "Editor", "<MyFinanceFile>", "PROGTYPE=PM;EXENAME=\OS2\APPS\EPM.EXE;")

say 'A FinanceFile has been created with the password: wps'
say 'select the context menu option LOCK to lock it'
say 'Switch to PM to test object.'

