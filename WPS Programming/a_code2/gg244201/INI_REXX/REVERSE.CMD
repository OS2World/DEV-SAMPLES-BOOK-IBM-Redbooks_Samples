/* rexx */

finished = 'false'

do until finished = 'true'

  say 'Type in a hex string'
  parse pull answer

  if answer = '' then finished = 'true'

  say 'The reverse was ' || reverse(answer)

end /* do until */

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