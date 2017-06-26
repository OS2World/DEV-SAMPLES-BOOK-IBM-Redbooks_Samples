
#
# Include the class definition file for the parent class
#
include <wptrans.sc>

#
# Define the new class
#
class: Record,
       file stem = record,
       external prefix = record_,
       class prefix = recordcls_,
       major version = 1,
       minor version = 1,
       local;

#
# Specify the parent class
#
parent: WPTransient;

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
PSZ pRecordData;
-- This is the pointer the the text buffer
-- that contains the record data
#
# Define some new methods
#
methods:

BOOL SetRecordInfo( PSZ szData );
--
-- METHOD:   SetRecordInfo                               PUBLIC
--
-- PURPOSE:  Copy the szData string in to the
--           Record objects text buffer
--

BOOL GetRecordInfo( PSZ szData );
--
-- METHOD:   GetRecordInfo                               PUBLIC
--
-- PURPOSE:  Copy the  Record objects text buffer
--           in to the szData string
--

BOOL clsQueryDatabase( PSZ szSearch, WPObject *Folder), classmethod;
--
-- METHOD:   clsQueryDatabase                            PUBLIC
--
-- PURPOSE:  Search the record database for all entries
--           that match the patern in the search string.
--           This results in the creation of new objects
--           in the Folder specified
--

BOOL clsRestartServer(), classmethod;
--
-- METHOD:   clsRestartServer                            PUBLIC
--
-- PURPOSE:  Restarts the server program.
--

#
# Specify instance methods being overridden
#

override wpInitData;
--
-- METHOD:   wpInitData                                  PUBLIC
--
-- PURPOSE:  Initialise the object memory for
--           the record text buffer
--

override wpUnInitData;
--
-- METHOD:   wpInitData                                  PUBLIC
--
-- PURPOSE:  Free the object memory for
--           the record text buffer
--

override wpModifyPopupMenu;
--
-- METHOD:   wpModifyPopupMenu                           PUBLIC
--
-- PURPOSE:  Add any new options for the records
--           context menu
--

override wpMenuItemSelected;
--
-- METHOD:   wpMenuItemSelected                          PUBLIC
--
-- PURPOSE:  Processing for record objects menu
--

override wpOpen;
--
-- METHOD:   wpOpen                                      PUBLIC
--
-- PURPOSE:  Open a view of the object ( see also
--           wpclsQueryDefaultView )
--

override wpSetup;
--
-- METHOD:   wpSetup                                     PUBLIC
--
-- PURPOSE:  Process the records Setup strings
--

#
# Specify class methods being overridden
#

override wpclsInitData, classmethod;
--
-- METHOD:   wpclsInitData                               PUBLIC
--
-- PURPOSE:  Initialises the Class
--           This involves :
--              Initialising class variables
--              Loading class resources
--              Starting the IPC requester thread
--              Registering the Viewing window PM Class
--

override wpclsUnInitData, classmethod;
--
-- METHOD:   wpclsUnInitData                             PUBLIC
--
-- PURPOSE:  Free Class resources
--

override wpclsQueryTitle, classmethod;
--
-- METHOD:   wpclsQueryTitle                             PUBLIC
--
-- PURPOSE:  Return the string "Record" as the default
--           title for new record objects
--

override wpclsQueryIcon, classmethod;
--
-- METHOD:   wpclsQueryIcon                              PUBLIC
--
-- PURPOSE:  Returns the default record icon pointer.
--

override wpclsQueryDefaultView, classmethod;
--
-- METHOD:   wpclsQueryDefaultView                       PUBLIC
--
-- PURPOSE:  Returns the default view for a
--           new instance of the record object.
--

override wpclsFindObjectFirst, classmethod;
--
-- METHOD:   wpclsFindObjects                            PUBLIC
--
-- PURPOSE:  Shell entry to finding record objects
--
