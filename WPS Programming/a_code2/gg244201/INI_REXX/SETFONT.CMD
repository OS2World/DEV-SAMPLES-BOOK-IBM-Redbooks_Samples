/*--------------------------------------------------------------------
 *
 * SETFONT.CMD Set the default font for all system menus to 14 point
 * Times New Roman.
 *
 *-------------------------------------------------------------------*/

/* initialize variables */

keyname   = 'Menus'               /* key name         */
appname   = 'PM_SystemFonts'      /* application name */
inifile   = 'USER'                /* INI file to use  */
value     = '14.Times New Roman'  /* value to insert  */
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
  say 'Value for key ' || keyname || ' successfully set to ' || value

exit

