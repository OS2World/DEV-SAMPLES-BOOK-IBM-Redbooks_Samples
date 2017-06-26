/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

'copy pwfolder.dll d:\os2\dll 1>nul: 2>nul:'

if rc then do
   say 'Error DLL could not be updated please re-boot'
   /* Remove bad entry */
   RetCode = SysDeregisterObjectClass( "PWFolder");
   'pause'
   exit(1)
   end

RetCode = SysRegisterObjectClass( "PWFolder", "pwfolder")

if RetCode then
   say 'PWFolder Class registered'
else do
   say 'Error PWFolder Class failed to register'
   /* Remove false entry */
   RetCode = SysDeregisterObjectClass( "PWFolder");
   exit(1)
   end

