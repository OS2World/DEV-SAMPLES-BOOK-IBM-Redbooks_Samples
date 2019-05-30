/* */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

Object = "GEQueryDesktop"

os2dll = FileSpec('Drive', Value('SYSTEM_INI',,'OS2ENVIRONMENT')) || ,
         '\OS2\DLL'
'@copy qdesk.dll' os2dll '1>nul: 2>nul:'

RetCode = SysRegisterObjectClass( Object, os2dll"\QDESK.DLL")

if RetCode then
   say Object 'Class registered'
else do
   say 'Error' Object 'Class already registered'
   exit(1)
   end

exit(0)
