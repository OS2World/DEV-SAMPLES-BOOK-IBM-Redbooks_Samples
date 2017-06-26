/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

'copy names.dat c:\names.dat 1>nul: 2>nul:'
'copy server.exe \os2 1>nul: 2>nul:'
'copy record.dll \os2\dll 1>nul: 2>nul:'
'copy recflder.dll \os2\dll 1>nul: 2>nul:'

RetCode = SysRegisterObjectClass( "Record", "record")

if RetCode then
   say 'Record Class registered'
else do
   say 'Error Record Class failed to register'
   /* Remove false entry */
   RetCode = SysDeregisterObjectClass( "Record");
   exit(1)
   end

RetCode = SysRegisterObjectClass( "RecordFolder", "recflder")

if RetCode then
   say 'RecordFolder Class registered'
else do
   say 'Error RecordFolder Class failed to register'
   /* Remove false entry */
   RetCode = SysDeregisterObjectClass( "RecordFolder");
   exit(1)
   end
