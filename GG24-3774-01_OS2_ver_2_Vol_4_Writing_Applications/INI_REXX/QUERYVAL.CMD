/*--------------------------------------------------------------------
 *
 * QUERYVAL.CMD query the key value for the application
 * PM_ControlPanel and key name CursorBlinkRate.
 *
 *-------------------------------------------------------------------*/

/* initialize variables */

keyname   = 'CursorBlinkRate'     /* key name         */
appname   = 'PM_ControlPanel'     /* application name */
inifile   = 'USER'                /* INI file to use  */

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

/* query key value */

value = SysIni(inifile, appname, keyname)
if value = 'ERROR:' then
  do
    say 'Error obtaining INI file information'
    exit
  end /* if */

/* remove terminating null character */

len = length(value)
value = left(value, len - 1)

/* write out results */

say 'Key value for application ' || appname || ' key name ' || keyname,
     || ' is:'
say value

exit

