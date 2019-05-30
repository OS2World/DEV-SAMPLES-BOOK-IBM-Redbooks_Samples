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
/*       HINI         Handle of ini file                                      */
/*       PUSHORT      Pointer to USHORT for return code                       */
/*                                                                            */
/* Result:                                                                    */
/*                                                                            */
/*       PSZ          Pointer to allocated string containing the real name    */
/*                    if NULL, the return code is one of the followings:      */
/*                                                                            */
/*                    1 = Handle of desktop not found in INI file             */
/*                    2 = Unable to allocate shared memory                    */
/*                    3 = Cannot create object                                */
/*                    4 = Error in WinSetObjectData                           */
/*                                                                            */
/******************************************************************************/
         PSZ        QueryDesktopDirectory(HINI hini, PUSHORT Retc)
{
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
         PQDESK_PARM qd;
         ULONG      DataLen;
         PSZ        Result = (PSZ)NULL;
         BOOL       fSuccess;
         APIRET     rc;
         HOBJECT    hObject;
         HOBJECT    hObjDesktop;

*Retc = 0;

/* Query handle of the desktop directory */
DataLen = sizeof(hObjDesktop);
fSuccess = PrfQueryProfileData(hini,
                               (PSZ)Application,
                               (PSZ)Keyname,
                               (PSZ)&hObjDesktop,
                               &DataLen);

if ((!fSuccess) || (DataLen < sizeof(hObjDesktop)))
   {
   *Retc = 1;
   return((PSZ)NULL);
   }

hObject = CreateQDObject(&qd, Retc, (PSZ)NULL);

if (!hObject)
   {
   (*Retc)++;
   return((PSZ)NULL);
   }

qd->ObjHandle = hObjDesktop;
qd->Action = ACTION_QUERY_FROM_OBJECTHANDLE;

qd->CallNr = 0;

fSuccess = SetQDObjectData();

if (qd->CallNr < 2)
   *Retc = 4;
else
   Result = strdup(qd->String);

DeleteQDObject();

return(Result);
}
