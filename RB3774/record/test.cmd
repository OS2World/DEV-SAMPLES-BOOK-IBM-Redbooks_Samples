/*  */
Call RxFuncadd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

'@echo off'

RetCode = SysCreateObject( "RecordFolder", "People Folder", "<WP_DESKTOP>", " ")

if RetCode then
   say 'Folder Object created'
else do
   say 'Error creating object'
   exit(1)
   end

/* Required for GA problem */
RetCode = SysCreateObject( "Record", "DUMMY", "<WP_NOWHERE>", "OBJECTID=<RECORD_0>")
RetCode = SysCreateObject( "RecordFolder", "DUMMY", "<WP_NOWHERE>", "OBJECTID=<RECFLDER_0>")

say 'A Record Folder has been created, select the context menu'
say '<Telephone List - Find> and test it.'

