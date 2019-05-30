/* */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

Object = "GEQueryDesktop"

os2dll = FileSpec('Drive', Value('SYSTEM_INI',,'OS2ENVIRONMENT')) || ,
         '\OS2\DLL'
'@copy qdesk.dll' os2dll '1>nul: 2>nul:'


if 1 = 2 then
if rc then do
   say Object 'class already registered.'
   /* Remove bad entries */
/*   RetCode = SysDeregisterObjectClass(Object); */
   '@pause'
   exit(1)
   end

RetCode = SysRegisterObjectClass( Object, os2dll"\QDESK.DLL")

if RetCode then
   say Object 'Class registered'
else do
   say 'Error' Object 'Class failed to register'
   /* Remove false entry */
/*   RetCode = SysDeregisterObjectClass(Object); */
   exit(1)
   end
