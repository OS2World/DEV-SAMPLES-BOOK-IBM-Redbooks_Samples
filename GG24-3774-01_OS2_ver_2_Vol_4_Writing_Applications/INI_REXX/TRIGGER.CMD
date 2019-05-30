/*--------------------------------------------------------------------
 * TRIGGER.CMD - program to change mouse trigger information in the
 * user INI file. The particular mouse trigger to update and the values
 * to use are hard coded in the initialize variables section. The
 * keyboard values should be set to INP_NONE if not used
 *-------------------------------------------------------------------*/
/*--------------------------------------------------------------------
 * Assign Constants
 *-------------------------------------------------------------------*/
/* mouse values */
WM_BUTTON1DOWN   = '0071'x
WM_BUTTON1UP     = '0072'x
WM_BUTTON1DBLCLK = '0073'x
WM_BUTTON2DOWN   = '0074'x
WM_BUTTON2UP     = '0075'x
WM_BUTTON2DBLCLK = '0076'x
WM_CHORD         = '0410'x

/* keyboard values */
INP_NONE         = '0000'x
INP_SHIFT        = '0008'x
INP_CTRL         = '0010'x
INP_ALT          = '0020'x
INP_IGNORE       = 'FFFF'x

/*--------------------------------------------------------------------
 * Initialize variables
 *-------------------------------------------------------------------*/

mouse_value  = WM_BUTTON2DOWN        /* mouse action causing trigger */
keyboard1    = INP_NONE              /* keyboard modifier 1          */
keyboard2    = INP_NONE              /* keyboard modifier 2          */
keyboard3    = INP_NONE              /* keyboard modifier 3          */
trigger_name = 'ContextMenuMouse'    /* key name for trigger         */
appname      = 'PM_ControlPanel'     /* application name             */
inifile      = 'USER'                /* INI file to use              */

/*--------------------------------------------------------------------
 * Register SysIni
 *-------------------------------------------------------------------*/

if RxFuncQuery('SysIni') = 1 then
  do /* register SysIni */
    rc = RxFuncAdd('SysIni', 'RexxUtil', 'SysIni')
    If rc <> 0 then
      do
        say 'SysIni registration failed'
        exit
      end /* do */
  end /* if */

/*--------------------------------------------------------------------
 * Construct value to put in INI
 *-------------------------------------------------------------------*/
keyboard = bitor(keyboard1,keyboard2)
keyboard = bitor(keyboard, keyboard3)
keyboard = c2x(keyboard)
mouse    = c2x(mouse_value)
value = keyboard || mouse
value = reverse(value)
value = x2c(value)

/*--------------------------------------------------------------------
 * Insert value in INI file
 *-------------------------------------------------------------------*/
result = SysIni(inifile, appname, trigger_name, value)
if result = 'ERROR:' then
  say 'Error setting INI file information'
else
  say 'Trigger value successfully set'

exit

reverse: procedure
/*--------------------------------------------------------------------
 * reverse: REXX function to reverse the byte order in a hexadecimal
 *          string
 *
 * input  : a hexadecimal string with no blanks
 * output : a hexadecimal string giving the bytes in the input in
 *          reverse order. If the input string contains an odd number
 *          of digits a leading zero is inserted before conversion.
 *          If the input string is not a valid hexadecimal string
 *          then a null string is returned.
 *-------------------------------------------------------------------*/
arg input_string .

result = ''

if datatype(input_string, 'X') = 1 then /* we have a valid hex string */
  do
    len = length(input_string)
    if (len // 2) <> 0 then /* odd number of digits */
      do
        input_string = '0' || input_string
        len = len + 1
      end /* if */
    if len < 4 then
      do
        result = input_string
      end /* if */
    else
      do i = 1 to len - 1 by 2
        result = substr(input_string, i, 2) || result
      end /* else */
  end /* if */

return result
