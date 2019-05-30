/*--------------------------------------------------------------------
 *
 * SETVAL.CMD Set the value of the CursorBlinkRate key in the 
 * PM_ControlPanel application to 0.
 *
 *-------------------------------------------------------------------*/

/* initialize variables */

keyname   = 'CursorBlinkRate'     /* key name         */
appname   = 'PM_ControlPanel'     /* application name */
inifile   = 'USER'                /* INI file to use  */
value     = '0'                   /* value to insert  */
NULL      = '00'x                 /* null character   */

/* register SysIni */

if RxFuncQuery('SysIni') = 1 then
  do /* register SysIni */
    rc = RxFuncAdd('SysIni', 'RexxUtil', 'SysIni')
    If rc <> 0 then
      do
        say 'SysIni registration failed'
        exit
      end /* do */
  end /* if */

/* add null character */

value = value || NULL

/* set key value */

result = SysIni(inifile, appname, keyname, value)
if result = 'ERROR:' then
  say 'Error setting INI file information'
else
  say 'Key value successfully set to ' || value

exit

