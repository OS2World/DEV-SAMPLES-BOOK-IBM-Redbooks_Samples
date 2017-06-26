##########################################################
#                                                        #
#  RECFLDER.CSC          (c) IBM Corporation 1992        #
#                                                        #
#  A WPS Example : This class derives from WPFolder      #
#  and is to be used to illustrate WPS programming       #
#  for the ITSC Redbook.                                 #
#                                                        #
##########################################################

#
# Include the class definition file for the parent class
#
include <wpfolder.sc>

#
#   Define the new class
#
class: RecordFolder,
       file stem = recflder,
       external prefix = recflder_,
       class prefix = recfldercls_,
       major version = 1,
       minor version = 1,
       local;

-- RecordFolder is a folder in which Record objects can be
-- instantiated.
-- Its derived as follows:
--        SOMOject
--          - WPObject
--             - WPFileSystem
--                - WPFolder
--                   - RecordFolder



#
# Specify the parent class
#
parent: WPFolder;

#
# Passthru a debug message box to the .ih file
# (for inclusion in the .c file)
#
passthru: C.h, after;

#define DebugBox(title, text)  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP, \
                               (PSZ) text , (PSZ) title, 0, \
                               MB_OK | MB_INFORMATION )

endpassthru;

#
# Define some instance data for the class
#
data:
CHAR szLastQuery[200];
--  This is the last query made from this folder
--  to get some records instaciated from the Record
--  class

#
# Define some new methods
#
methods:

BOOL UpdateLastQuery(PSZ szQuery);
--
-- METHOD:   UpdateLastQuery                             PRIVATE
--
-- PURPOSE:  Saves the last query that return a result
--           from the Record class
--

#
# Specify methods being overridden
#
override wpInitData;
--
-- OVERRIDE: wpInitData                                  PUBLIC
--
-- PURPOSE:  Initialize our instance variables.
--

override wpModifyPopupMenu;
--
-- OVERRIDE: wpModifyPopupMenu                           PUBLIC
--
-- PURPOSE:  Add the extra find option to
--           the context menu.
--

override wpMenuItemSelected;
--
-- OVERRIDE: wpMenuItemSelected                          PUBLIC
--
-- PURPOSE:  Process input from the extra
--           menu option that we have added.
--

override wpSaveState;
--
-- OVERRIDE: wpSaveState                                 PUBLIC
--
-- PURPOSE:  Here we can store our persistent instance
--           data so that it can be restored from
--           wpRestoreState
--

override wpRestoreState;
--
-- OVERRIDE: wpRestoreState                              PUBLIC
--
-- PURPOSE:  Allow us to resend the last query to
--           the Record class
--

override wpclsQueryTitle, classmethod;
--
-- METHOD: wpclsQueryTitle                               PUBLIC
--
-- PURPOSE:
--   Return the string "Record Folder"
--

override wpclsInitData, classmethod;
--
-- METHOD: wpclsInitData                                 PUBLIC
--
-- PURPOSE:
--   We can do class specific initialisation here
--

override wpclsUnInitData, classmethod;
--
-- METHOD: wpclsUnInitData                               PUBLIC
--
-- PURPOSE:
--   Here we release any class resources
--
