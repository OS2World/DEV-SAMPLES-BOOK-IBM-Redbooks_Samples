#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INCL_DOSERRORS
#define INCL_DOSMEMMGR
#define INCL_WINSHELLDATA
#define INCL_WPERRORS
#include <os2.h>

#include <wpfolder.h>

#include "..\qdesk\qdparm.h"
#include "chgdt.h"

/******************************************************************************/
/* Query Desktop directory                                                    */
/*                                                                            */
/* Parameters:                                                                */
/*                                                                            */
/*       PSZ          File name of new ini file                               */
/*       HAB          Anchor block handle                                     */
/*       PSZ          Name of new desktop directory structure                 */
/*                                                                            */
/* Result:                                                                    */
/*                                                                            */
/*       USHORT       Return code:                                            */
/*                                                                            */
/*                    0 = ok, INI file updated with new desktop handle        */
/*                    2 = INI file not found                                  */
/*                    3 = No valid INI file or in use                         */
/*                    4 = Memory allocation error                             */
/*                    6 = Unable to allocate shared memory                    */
/*                    7 = Cannot create object MyGEQueryObject                */
/*                    9 = Cannot locate desktop directory                     */
/*                   10 = Cannot update INI file                              */
/*                                                                            */
/******************************************************************************/
         USHORT     SetDesktopDirectory(PSZ FileName, HAB hab, PSZ DirName)
{
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
         BOOL       fSuccess;
         HFILE      Handle;
         ULONG      ActionTaken;
         ULONG      DataLen;
         HINI       hini;
         HOBJECT    hObject;
         HOBJECT    hObjDesktop;
         PQDESK_PARM qd;
         APIRET     rc;
         USHORT     Retc = 0;

/* Check, if file exists */
if (DosOpen(FileName,
            &Handle,
            &ActionTaken,
            0L,
            0L,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
            (PEAOP2)NULL))
   return(2);

DosClose(Handle);

hini = PrfOpenProfile(hab, FileName);

/* No valid ini file if error occured on PrfOpenProfile */
if (!hini)
   return(3);

hObject = CreateQDObject(&qd, &Retc, (PSZ)NULL);
if (!hObject)
   Retc += 5;
else
   {
   strcpy(qd->String, DirName);
   qd->Action = ACTION_QUERY_FROM_OBJECTID;
   fSuccess = SetQDObjectData();
   if (fSuccess)
      {
      if (!PrfWriteProfileData(hini,
                               (PSZ)Application,
                               (PSZ)Keyname,
                               (PVOID)&qd->ObjHandle,
                               sizeof(HOBJECT)))
         Retc = 10;
      }
   else
      {
      Retc = 9;
      }
   DeleteQDObject();
   }

PrfCloseProfile(hini);

return(Retc);
}
