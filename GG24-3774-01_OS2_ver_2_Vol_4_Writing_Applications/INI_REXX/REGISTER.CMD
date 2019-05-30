/*--------------------------------------------------------------------
 *
 * Code fragment to register SysyIni
 *
 *-------------------------------------------------------------------*/

if RxFuncQuery('SysIni') = 1 then
  do /* register SysIni */
    rc = RxFuncAdd('SysIni', 'RexxUtil', 'SysIni')
    If rc = 0 then
      say 'SysIni registered OK'
    else
      say 'SysIni registration failed'
  end /* if */
else
  say 'SysIni already registered'

exit

