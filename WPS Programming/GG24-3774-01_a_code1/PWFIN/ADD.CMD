/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

say 'Please enter Boot drive letter'
pull boot
dll = substr(boot,1,1) || ':\os2\dll'

'copy pwFin.dll' dll

if rc then do
   say 'Error DLL could not be updated please re-boot'
   /* Remove bad entry */
   RetCode = SysDeregisterObjectClass( "PWFinanceFile");
   'pause'
   exit(1)
   end

RetCode = SysRegisterObjectClass( "PWFinanceFile", "pwFin")

if RetCode then
   say 'PWFinanceFile Class registered'
else do
   say 'Error PWFinanceFile Class failed to register'
   /* Remove false entry */
   RetCode = SysDeregisterObjectClass( "PWFinanceFile");
   exit(1)
   end

