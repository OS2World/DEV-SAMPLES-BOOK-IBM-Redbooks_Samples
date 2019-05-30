/*--------------------------------------------------------------------
 *
 * SETCOLOR.CMD
 * ------------
 * This program inserts color information into the OS2.INI file.
 *
 * Input is either via the command line or through an input file.
 *
 * The command line syntax is as follows:
 * SETCOLOR DEFAULT keyname "values" or
 * SETCOLOR keyname rrr ggg bbb
 *
 * The input file syntax is:
 * SETCOLOR DEFAULT /F:filename  or
 * SETCOLOR /F:filename
 *
 * where DEFAULT, if present causes PM_Default_Colors to be updated 
 * otherwise PM_Colors is updated.
 *
 * keyname must be a valid key name for PM_Colors or PM_Default_Colors
 *
 * rrr, ggg and bbb are the red, green and blue values respectively
 * 
 * filename must be the name of a file with lines of the form:
 *
 * keyname "values"
 *
 * with one or more spaces seperating keyname and "values"
 *
 * The output of this program is status and/or error information written
 * to standard output.
 *
 *-------------------------------------------------------------------*/
parse arg args
/*--------------------------------------------------------------------
 * Initialize Variables
 *-------------------------------------------------------------------*/
mode = ''                        /* operation mode                   */
filename = ''                    /* input file name                  */
appname = ''                     /* application to use               */
arglist. = ''                    /* list of arguments                */
numargs = 0                      /* number of arguments              */
keyname = ''                     /* key name to insert into          */
red = ''                         /* red value                        */
green = ''                       /* green value                      */
blue  = ''                       /* blue value                       */
numrows = 0                      /* number of rows of data           */
keynames. = ''                   /* key names                        */
red. = ''                        /* red values                       */
green. = ''                      /* green values                     */
blue.  = ''                      /* blue values                      */

DEFAPP = 'PM_Default_Colors'     /* default colors application       */
CURAPP = 'PM_Colors'             /* current colors apllication       */
/*--------------------------------------------------------------------
 * Validate arguments
 *-------------------------------------------------------------------*/

args = strip(args)
numargs = words(args)
/* note: "nnn nnn nnn" will be counted as 3 arguments */

do i = 1 to numargs
  arglist.i = word(args, i)
end /* do */

if left(arglist.numargs,3) = '/F:' then
  do /* we have a filename */
    select
      when numargs = 1 then
        do
          mode = 'multiple'
          filename = substr(arglist.numargs,4)
          appname = CURAPP
        end /* when */
      when numargs = 2 then
        do
          if arglist.1 = 'DEFAULT' then
            do
              mode = 'multiple'
              filename = substr(arglist.numargs,4)
              appname = DEFAPP
            end /* if */
          else
            do
              say 'ERROR: incorrect option supplied'
              say 'Please specify either DEFAULT or nothing before the filename'
              exit
            end /* else */
        end /* when */
      otherwise
        do
          say 'ERROR: incorrect number of arguments'
          say 'Please specify either DEFAULT or nothing before the filename'
          exit
        end /* otherwise */

    end  /* select */
  end /* if */
else
  do /* we don't have a filename */
    select
      when numargs = 4 then
        do
          mode = 'single'
          appname = CURAPP
          keyname = arglist.1
          red     = arglist.2
          green   = arglist.3
          blue    = arglist.4
        end /* when */
      when numargs = 5 then
        do
          if arglist.1 = 'DEFAULT' then
            do
              mode = 'single'
              appname = DEFAPP
              keyname = arglist.2
              red     = arglist.3
              green   = arglist.4
              blue    = arglist.5
            end /* if */
          else
            do
              say 'ERROR: incorrect option supplied'
              say 'Please specify either DEFAULT or nothing as an option'
              exit
            end /* else */
        end /* when */
      otherwise
        do
          say 'ERROR: incorrect number of arguments'
          if numargs < 4 then
            say 'Filename may have been specified incorrectly'
          else
            say 'Please specify an option (optional), a key name and 3 values'
          exit
        end /* otherwise */
    end  /* select */
  end /* else */

/*--------------------------------------------------------------------
 * Register SysIni
 *-------------------------------------------------------------------*/

if RxFuncQuery('SysIni') = 1 then
  do /* register SysIni */

    rc = RxFuncAdd('SysIni', 'RexxUtil', 'SysIni')

    If rc <> 0 then
      do
        say 'ERROR: SysIni registration failed'
        exit
      end /* do */
  end /* if */

/*--------------------------------------------------------------------
 * Set up Data Stems (read data file if necessary)
 *-------------------------------------------------------------------*/

if mode = 'single' then
  do
    numrows = 1
    keynames.1 = keyname
    red.1 = red
    blue.1 = blue
    green.1 = green
  end /* if */
else
  do
    result = stream(filename,'c','query exists')
    if result = '' then
      do
        say 'ERROR: Filename ' || filename || ' does not exist'
        say 'Please re-enter the command with a correct filename'
        exit
      end /* do */
      i = 0
      do while lines(filename) <> 0
        i = i + 1
        thisline = linein(filename)
        parse var thisline keynames.i red.i green.i blue.i
      end /* do while */
      numrows = i
  end /* else */

/*--------------------------------------------------------------------
 * Write Information to INI file
 *-------------------------------------------------------------------*/

do i = 1 to numrows
  values = build_values(red.i, green.i, blue.i)
  if values = 'ERROR:' then
    do
      say 'ERROR: Invalid RGB values : ' || red.i green.i blue.i
    end /* if */
  else
    do 
      if SysIni('USER',appname, keynames.i) = 'ERROR:' then
        do
          say 'ERROR: keyname ' || keynames.i || ' not found'
        end /* if */
      else
        do
          rcode = SysIni('USER', appname, keynames.i, values)
          if rcode <> '' then
            do
              say 'ERROR: Information not inserted into key ' || keynames.i
              say 'values: "' || values || '"'
            end /* if */
          else
            do
              say 'Information inserted into key ' || keynames.i
            end /* else */
        end /* else */
    end /* else */
end /* do */

exit

build_values: procedure
/*--------------------------------------------------------------------
 * valid_colors: function to build an RGB color string for insertion
 * into the INI files. The return value is either the value string
 * or 'ERROR:' if the input values are invalid
 *-------------------------------------------------------------------*/
arg red , green , blue

valid = 'true'

if ((datatype(red,'W') = 0) |,
    (red > 255) |,
    (red < 0  )) then
  valid = 'false'

if ((datatype(green,'W') = 0) |,
    (green > 255) |,
    (green < 0  )) then
  valid = 'false'

if ((datatype(blue,'W') = 0) |,
    (blue > 255) |,
    (blue < 0  )) then
  valid = 'false'

if valid = 'true' then
  rvalue = left(red,4)   ||,
           left(green,4) ||,
           left(blue,3)  ||,
           '00'x
else
  rvalue = 'ERROR:'

return rvalue


