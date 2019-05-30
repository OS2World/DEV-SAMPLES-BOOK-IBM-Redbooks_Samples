#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define INCL_DOSFILEMGR
#define INCL_WINSHELLDATA
#include <os2.h>

#include "chgdt.h"

/******************************************************************************/
/*   Check INI file                                                           */
/*                                                                            */
/*   Parameter:                                                               */
/*                                                                            */
/*        PSZ         Name of ini file                                        */
/*        HAB         HAB                                                     */
/*                                                                            */
/*   Result:                                                                  */
/*                                                                            */
/*        USHORT      0         Inifile ok, already in use                    */
/*                    1         Inifile ok, new                               */
/*                    2         Inifile not found                             */
/*                    3         no valid inifile or in use                    */
/*                    4         malloc error                                  */
/*                    5         Handle of desktop not found in INI file       */
/*                    6         Unable to allocate shared memory              */
/*                    7         Object cannot be created                      */
/*                    8         Desktop not found                             */
/******************************************************************************/
         USHORT     CheckIniFile(PSZ FileName, HAB hab, PSZ *DirName)
{
static   CHAR       Application1[] = "PM_Workplace:Location";
static   CHAR       Application2[] = "PM_InstallObject";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
static   CHAR       Semicolon[] = ";";
         PSZ        DTDir;
         PSZ        s1;
         PSZ        s2;
         PSZ        DataArea;
         PSZ        DataPtr;
         HINI       hini;
         HFILE      Handle;
         ULONG      DataLen;
         ULONG      ActionTaken;
         BOOL       fSuccess;
         USHORT     Retc;
         USHORT     QDTRetc;
         HOBJECT    hObject;

if (DirName != (PSZ *)NULL)
   *DirName = (PSZ)NULL;

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

DataLen = sizeof(hObject);
fSuccess = PrfQueryProfileData(hini,
                               (PSZ)Application1,
                               (PSZ)Keyname,
                               (PSZ)&hObject,
                               &DataLen);

Retc = 0;

if ((!fSuccess) || (DataLen < sizeof(hObject)))
   {
   fSuccess = PrfQueryProfileSize(hini,
                                  Application2,
                                  (PSZ)NULL,
                                  &DataLen);
   if (fSuccess && DataLen)
      {
      if ((DataArea = malloc(DataLen)) == (PSZ)NULL)
         {
         Retc = 4;
         }
      else
         {
         Retc = 3;
         PrfQueryProfileData(hini,
                             Application2,
                             (PSZ)NULL,
                             (PVOID)DataArea,
                             &DataLen);
         for (DataPtr = DataArea; DataPtr[0];
              DataPtr = &DataPtr[strlen(DataPtr) + 1])
            {
            s1 = strdup(DataPtr);
            if (s1)
               {
               s2 = strtok(s1, Semicolon);
               if (s2)
                  {
                  s2 = strtok(NULL, Semicolon);
                  if (s2)
                     {
                     if (!strcmp(s2, "WPDesktop"))
                        {
                        Retc = 1;
                        free(s1);
                        break;
                        }
                     }
                  }
               free(s1);
               }
            }
         free(DataArea);
         }
      }
   else
      Retc = 3;
   }
else
   {
   /* Application/Key found */
   DTDir = QueryDesktopDirectory(hini, &QDTRetc);
   if (DTDir)
      {
      if (DirName != (PSZ *)NULL)
         *DirName = DTDir;
      else
         free(DTDir);
      }
   else
      Retc = 4 + QDTRetc;
   }

PrfCloseProfile(hini);

return(Retc);
}
