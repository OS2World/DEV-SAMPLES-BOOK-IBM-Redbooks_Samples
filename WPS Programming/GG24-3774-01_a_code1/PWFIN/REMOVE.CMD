/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysDeregisterObjectClass( "PWFinanceFile");

if RetCode then
    say 'Uninstall successfully completed for PWFinanceFile class'

say 'Re-boot NOW in order to release DLL'
'pause'
