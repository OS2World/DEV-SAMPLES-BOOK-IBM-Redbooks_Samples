/* Delete inifile and desktop directory structure */
parse arg inifile dirname
if dirname <> ''
then do
     '@DELALL "'dirname'" > DELMSG.$$$'
     '@DEL' inifile
     '@ECHO Inifile' inifile 'deleted. >> DELMSG.$$$'
     end
else do
     '@DEL' inifile
     '@ECHO Inifile' inifile 'deleted. > DELMSG.$$$'
     end
