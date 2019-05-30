##############################################################
#                                                            #
#  pwFin.CSC          (c) IBM Corporation 1992               #
#                                                            #
#  This class derives from WPDataFile, and is used to        #
#  represent a FinanceFile which is protected by a password. #
#                                                            #
##############################################################

#
# Include the class definition file for the parent class
#
include <wpdataf.sc>
#
#   Define the new class
#
class: PWFinanceFile,
       file stem = pwFin,
       external prefix = pwFinanceFile_,
       class prefix = pwFinanceFilecls_,
       major version = 1,
       minor version = 1,
       local;

-- PWFinanceFile is a Password protected FinanceFile.
-- It is derived as follows:
--        SOMOject
--          - WPObject
--             - WPFileSystem
--                - WPDataFile
--                   - PWFinanceFile

#
# Specify the parent class
#
parent: WPDataFile;

#
# Specify the release order of new methods
#
release order: LockFinanceFile;

#
# Passthru a debug message box to the .ih file
# (for inclusion in the .c file)
#
passthru: C.ih, after;
#define INCL_PM
#define INCL_BSE
#define INCL_DOSERRORS
#include <wppgm.h>
#include <wppgmf.h>
#include <stdio.h>
#include <os2.h>

// force SOM to output all debug information to the Communications Port 1

int myReplacementForSOMOutChar (char c)
{
static FILE *fdebug = NULL;

if (!fdebug) {
  fdebug = fopen("COM1","w");

  if (!fdebug) return 0;     /* failed to open COM1: */
  }
fputc(c,fdebug);
fflush(fdebug);
/*if (c=='\n') fflush(fdebug);*/
return 1;
}

/*
 *   The following structures will be used to store window specific data
 *   and a pointer to the object that created the window/dialog.
 *
 *   They're allocated when the Client window is created.  This
 *   allows us to pass the *somSelf pointer and use it in our
 *   window and dialog procedures (the system only passes this
 *   pointer to methods).
 */
   typedef struct _WINDOWDATA
   {
      USHORT   cb;            /* size of this structure */
PWFinanceFile *somSelf;       /* pointer to this instance */
      USEITEM  UseItem;       /* global class usage information */
      VIEWITEM ViewItem;      /* global class view information */
      LONG     x;             /* x position of car in open view */
      LONG     y;             /* y position of car in open view */
      LONG     xDir;          /* x direction */
      LONG     yDir;          /* y direction */
   } WINDOWDATA;
   typedef WINDOWDATA *PWINDOWDATA;

endpassthru;

#
# Passthru private definitions to the .ph file
# (for inclusion in the .c file)
#
passthru: C.ph;

typedef struct _PWF_INFO {                       /* Define password structure */
  CHAR   szPassword[20];                         /* FinanceFile current password   */
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
--  This is the password which locks the FinanceFile

CHAR szCurrentPassword[20];
--  This is the password the user has typed in to be
--  checked against the lock password

CHAR szUserid[20];
--  Userid


#
# Define new methods
#
methods:

BOOL QueryInfo(PPWF_INFO pPWFinanceFileInfo), private;
--
--  METHOD:   QueryInfo                                   PRIVATE
--
--  PURPOSE:  Copies instance data into the PWF_INFO structure.
--
--  INVOKED:  From PasswordDlgProc
--

BOOL SetInfo(PPWF_INFO pPWFinanceFileInfo), private;
--
--  METHOD:   SetInfo                                     PRIVATE
--
--  PURPOSE:  Sets instance data from the PWF_INFO structure.
--
--  INVOKED:  From PasswordDlgProc
--

BOOL LockFinanceFile();
--
--  METHOD:   LockFinanceFile                                  PUBLIC
--
--  PURPOSE:  Invalidates the current password, thereby locking the FinanceFile.
--
--  INVOKED:  From _wpMenuItemSelected
--

#
# Specify methods being overridden
#
override wpFilterPopupMenu;
--
--  METHOD:   wpFilterPopupMenu                           PUBLIC
--
--  PURPOSE:  This class method is called when the user asks for the context
--            (popup) menu.
--
--  INVOKED:  -
--

override wpPrintObject;
--
--  METHOD:   wpPrintObject                               PUBLIC
--
--  PURPOSE:  This class method allows this object to format it's output for
--            printing.
--
--  INVOKED:  -
--

override wpDraggedOverObject;
--
--  METHOD:   wpDraggedOverObject                         PUBLIC
--
--  PURPOSE:  Checks to see that the file is unlocked if user wants to
--            drop the object on a program.
--
--  INVOKED:  -
--

override wpDragOver;
--
--  METHOD:   wpDragOver                                  PUBLIC
--
--  PURPOSE:  Checks to see that the object being dragged over me is also
--            either of my class or derived from me.
--
--  INVOKED:  By Workplace Shell, when an object is being dragged over this
--            object.
--

override wpDrop;
--
--  METHOD:   wpDrop                                      PUBLIC
--
--  PURPOSE:  To receive a dropped object.
--
--  INVOKED:  By Workplace Shell, when another object has been dropped on
--            this object.
--

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
--  PURPOSE:  Only allows a FinanceFile to be opened if the FinanceFile is unlocked, or
--            if the user supplies the correct password in response to the
--            dialog.
--
--  INVOKED:  By Workplace Shell, from the parent wpViewObject method
--

override wpSetTitle;
--
--  METHOD:   wpSetTitle                                  PUBLIC
--
--  PURPOSE:  Sets the FinanceFile's title (icon text) to have the phrase <Locked>
--            as a suffix if the FinanceFile is locked, or removes this suffix if
--            the FinanceFile is unlocked.
--
--  INVOKED:  By wpOpen to set the unlocked state, and by LockFinanceFile to set
--            the locked state.
--

override wpSetup;
--
--  METHOD:   wpSetup                                     PUBLIC
--
--  PURPOSE:  Sets FinanceFile properties based upon a setup string passed by the
--            object's creator as part of the WinCreateObject() call.  The
--            overridden method simply processes the PASSWORD keyword to set
--            the FinanceFile's password immediately upon instantiation, before
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

override wpAddFileTypePage;
--
--  METHOD:   wpAddFileTypePage                           PUBLIC
--
--  PURPOSE:  This class method replaces the type page with the it's own one
--            that only allows the FinanceFile Types.
--
--  INVOKED:  By the Workplace Shell, upon unloading the class DLL.
--

override wpclsQueryTitle, classmethod;
--
--  METHOD:   wpclsQueryTitle                             PUBLIC
--
--  PURPOSE:  This class method returns the default FinanceFile title for any
--            instance of the password protected FinanceFile class.  This title
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