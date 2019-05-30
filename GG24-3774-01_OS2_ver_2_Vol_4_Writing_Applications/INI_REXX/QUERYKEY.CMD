/*--------------------------------------------------------------------
 *
 * QUERYKEY.CMD List all key names for the application PM_ControlPanel
 * and write them to the screen.
 *
 *-------------------------------------------------------------------*/

/* initialize variables */

keynames. = ''                    /* stem to hold key names */
appname   = 'PM_ControlPanel'     /* application name       */
inifile   = 'USER'                /* INI file to use        */

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
/* query key names */

result = SysIni(inifile, appname, 'ALL:', 'keynames')
if result = 'ERROR:' then
  do
    say 'Error obtaining INI file information'
    exit
  end /* if */

/* write out results */

say 'Key names for application ' || appname || ' are:'
do i = 1 to keynames.0
  say keynames.i
end /* do */

exit

