##########################################################
#                                                        #
#  PWFOLDER.CSC          (c) IBM Corporation 1992        #
#                                                        #
#  This class derives from WPFolder, and is used to      #
#  represent a folder which is protected by a password.  #
#                                                        #
##########################################################

#
# Include the class definition file for the parent class
#
include <wpfolder.sc>

#
#   Define the new class
#
class: PWFolder,
       file stem = pwfolder,
       external prefix = pwfolder_,
       class prefix = pwfoldercls_,
       major version = 1,
       minor version = 1,
       local;

-- PWFolder is a Password protected folder.
-- It is derived as follows:
--        SOMOject
--          - WPObject
--             - WPFileSystem
--                - WPFolder
--                   - PWFolder

#
# Specify the parent class
#
parent: WPFolder;

#
# Specify the release order of new methods
#
release order: LockFolder;

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
# Passthru private definitions to the .ph file
# (for inclusion in the .c file)
#
passthru: C.ph;

typedef struct _PWF_INFO {                       /* Define password structure */
  CHAR   szPassword[20];                         /* Folder current password   */
  CHAR   szCurrentPassword[20];                  /* User-entered password     */
  CHAR   szUserid[20];                           /* Userid                    */
} PWF_INFO;
typedef PWF_INFO *PPWF_INFO;                     /* Define pointer type       */

endpassthru;

#
# Define instance data for the class
#
data:
CHAR szPassword[20];
--  This is the password which locks the folder

CHAR szCurrentPassword[20];
--  This is the password the user has typed in to be
--  checked against the lock password

CHAR szUserid[20];
--  Userid

#
# Define new methods
#
methods:

BOOL QueryInfo(PPWF_INFO pPWFolderInfo), private;
--
--  METHOD:   QueryInfo                                   PRIVATE
--
--  PURPOSE:  Copies instance data into the PWF_INFO structure.
--
--  INVOKED:  From PasswordDlgProc
--

BOOL SetInfo(PPWF_INFO pPWFolderInfo), private;
--
--  METHOD:   SetInfo                                     PRIVATE
--
--  PURPOSE:  Sets instance data from the PWF_INFO structure.
--
--  INVOKED:  From PasswordDlgProc
--

BOOL LockFolder();
--
--  METHOD:   LockFolder                                  PUBLIC
--
--  PURPOSE:  Invalidates the current password, thereby locking the folder.
--
--  INVOKED:  From _wpMenuItemSelected
--

#
# Specify methods being overridden
#
override wpInitData;
--
--  METHOD:   wpInitData                                  PUBLIC
--
--  PURPOSE:  Initializes instance data
--
--  INVOKED:  By Workplace Shell, upon instantiation of the object instance.
--

override wpModifyPopupMenu;
--
--  METHOD:   wpModifyPopupMenu                           PUBLIC
--
--  PURPOSE:  Adds an additional "Lock" item to the object's context menu.
--
--  INVOKED:  By Workplace Shell, upon instantiation of the object instance.
--

override wpMenuItemSelected;
--
--  METHOD:   wpMenuItemSelected                          PUBLIC
--
--  PURPOSE:  Processes the user's selections from the context menu.  The
--            overridden method processes only the added "Lock" item, before
--            invoking the parent's default processing to handle other items.
--
--  INVOKED:  By Workplace Shell, upon selection of a menu item by the user.
--

override wpOpen;
--
--  METHOD:   wpOpen                                      PUBLIC
--
--  PURPOSE:  Only allows a folder to be opened if the folder is unlocked, or
--            if the user supplies the correct password in response to the
--            dialog.
--
--  INVOKED:  By Workplace Shell, upon selection of the "Open" menu item by
--            the user.
--

override wpSetTitle;
--
--  METHOD:   wpSetTitle                                  PUBLIC
--
--  PURPOSE:  Sets the folder's title (icon text) to have the phrase <Locked>
--            as a suffix if the folder is locked, or removes this suffix if
--            the folder is unlocked.
--
--  INVOKED:  By wpOpen to set the unlocked state, and by LockFolder to set
--            the locked state.
--

override wpSetup;
--
--  METHOD:   wpSetup                                     PUBLIC
--
--  PURPOSE:  Sets folder properties based upon a setup string passed by the
--            object's creator as part of the WinCreateObject() call.  The
--            overridden method simply processes the PASSWORD keyword to set
--            the folder's password immediately upon instantiation, before
--            invoking the parent's default processing to handle all other
--            keywords.
--
--  INVOKED:  By the Workplace Shell, upon instantiation of the object
--            instance.
--

override wpSaveState;
--
--  METHOD:   wpSaveState                                 PUBLIC
--
--  PURPOSE:  Saves the object instance's persistent state data.  The
--            overridden method simply saves the password data, then invokes
--            the parent's default processing to handle any other instance
--            data defined by ancestor classes.
--
--  INVOKED:  By the Workplace Shell, when the object becomes dormant.
--

override wpRestoreState;
--
--  METHOD:   wpRestoreState                              PUBLIC
--
--  PURPOSE:  Restores the object instance's persistent state data.  The
--            overridden method simply restores the password data, then
--            invokes the parent's default processing to handle any other
--            instance data defined by ancestor classes.
--
--  INVOKED:  By the Workplace Shell, when the object becomes awake.
--

override wpSetIcon;
--
--  METHOD:   wpSetIcon                                   PUBLIC
--
--  PURPOSE:  This class method returns the handle to the correct icon for
--            the object.
--
--  INVOKED:  -
--

override wpclsQueryTitle, classmethod;
--
--  METHOD:   wpclsQueryTitle                             PUBLIC
--
--  PURPOSE:  This class method returns the default folder title for any
--            instance of the password protected folder class.  This title
--            is used if a title is not supplied in the WinCreateObject()
--            call.
--
--  INVOKED:  By the Workplace Shell, upon instantiation of the object
--            instance.
--

override wpclsInitData, classmethod;
--
--  METHOD:   wpclsInitData                               PUBLIC
--
--  PURPOSE:  This class method allows the initialization of any class data
--            items.  The overridden method simply obtains a module handle
--            to be used when accessing Presentation Manager resources, then
--            invokes the parent's default processing.
--
--  INVOKED:  By the Workplace Shell, upon loading the class DLL.
--

override wpclsQueryIcon, classmethod;
--
--  METHOD:   wpclsQueryIcon                                 PUBLIC
--
--  PURPOSE:  This class method returns the handle to the default icon for
--            the class.  This method is not used in the current version,
--            but could be used if different icons are to be used for the
--            locked and unlocked states.
--
--  INVOKED:  -
--

override wpclsUnInitData, classmethod;
--
--  METHOD:   wpclsUnInitData                             PUBLIC
--
--  PURPOSE:  This class method allows the release of any class data items
--            or resources.  The overridden method releases the module handle
--            obtained by wpclsInitData, then invokes the parent's default
--            processing.
--
--  INVOKED:  By the Workplace Shell, upon unloading the class DLL.
--
