/*************************************************************************/
/* Functions to create and delete object MyGEQueryObject                 */
/*************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#define INCL_DOSERRORS
#define INCL_DOSMEMMGR
#define INCL_WINSHELLDATA
#define INCL_WPERRORS
#include <os2.h>

#include <wpfolder.h>

#include "qdparm.h"

static   PSZ        pszRealName = (PSZ)NULL;
static   HOBJECT    hObject = (HOBJECT)NULL;

/******************************************************************************/
/* Create Object for querying desktop                                         */
/******************************************************************************/
         HOBJECT    CreateQDObject(PQDESK_PARM * qd, PUSHORT Retc, PSZ * Msg)
{
static   CHAR       SharedMemName[] = "\\SHAREMEM\\TESTQRY.MEM";
static   CHAR       Msg1[] = "Cannot create or access shared memory.";
static   CHAR       Msg2[] = "Cannot create object MyGEQueryDesktop in"
                             " class GEQueryObject.";
static   CHAR       ObjectClassName[] = "GEQueryDesktop";
static   CHAR       ObjectName[] = "MyGEQueryDesktop";
static   USHORT     FirstCall = 1;
         APIRET     rc;


rc = DosAllocSharedMem((PVOID *)&pszRealName, SharedMemName,
                       sizeof(QDESK_PARM), PAG_COMMIT | PAG_WRITE);

if (rc == ERROR_ALREADY_EXISTS)
   {
   rc = DosGetNamedSharedMem((PVOID *)&pszRealName, SharedMemName,
                             PAG_WRITE | PAG_READ);

   if (rc)
      {
      *Retc = ERROR_CANNOT_CREATE_SHAREDMEM;
      if (Msg)
         *Msg = Msg1;
      return((HOBJECT)NULL);
      }
   }
else
   if (rc)
      {
      *Retc = ERROR_CANNOT_CREATE_SHAREDMEM;
      if (Msg)
         *Msg = Msg1;
      return((HOBJECT)NULL);
      }

hObject = WinCreateObject(ObjectClassName, ObjectName,
                          "NOTVISIBLE=YES", "<WP_DESKTOP>",
                          CO_UPDATEIFEXISTS);

if (!hObject)
   {
   *Retc = ERROR_CANNOT_CREATE_OBJECT;
   if (Msg)
      *Msg = Msg2;
   DosFreeMem(pszRealName);
   return((HOBJECT)NULL);
   }

*qd = (PQDESK_PARM)pszRealName;

if (FirstCall)
   {
   atexit(DeleteQDObject);
   FirstCall = 0;
   }

return(hObject);
}

/******************************************************************************/
/* Destroy object and free memory                                             */
/******************************************************************************/
         VOID       DeleteQDObject(VOID)
{
if (hObject)
   {
   WinDestroyObject(hObject);
   hObject = (HOBJECT)NULL;
   }

if (pszRealName)
   {
   DosFreeMem(pszRealName);
   pszRealName = (PSZ)NULL;
   }
}

/******************************************************************************/
/* Set object data                                                            */
/******************************************************************************/
         BOOL       SetQDObjectData(VOID)
{
return(WinSetObjectData(hObject, "SHAREMEM=\\SHAREMEM\\TESTQRY.MEM;"));
}
