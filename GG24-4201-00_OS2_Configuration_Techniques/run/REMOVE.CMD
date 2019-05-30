/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

Object = "GEQueryDesktop"

RetCode = SysDeregisterObjectClass( Object);
if RetCode then
    say 'Uninstall successfully completed for' Object 'class'

say 'Re-boot NOW in order to release DLLs'
'pause'
