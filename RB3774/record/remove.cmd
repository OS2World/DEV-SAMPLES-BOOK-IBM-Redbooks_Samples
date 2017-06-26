/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysDeregisterObjectClass( "RecordFolder");
if RetCode then
    say 'Uninstall successfully completed for RecordFolder class'

RetCode = SysDeregisterObjectClass( "Record");

if RetCode then
    say 'Uninstall successfully completed for Record class'

say 'Re-boot NOW '
'pause'

