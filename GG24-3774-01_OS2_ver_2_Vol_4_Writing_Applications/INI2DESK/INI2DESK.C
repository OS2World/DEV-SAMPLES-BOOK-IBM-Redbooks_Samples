#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_WINSHELLDATA
#define INCL_WINWORKPLACE
#include <os2.h>

#include <wpfolder.h>

#include "..\qdesk\qdparm.h"

#define ININAME_LENGTH   200

         PCHAR      GetRealName(USHORT OjectID);

         int        main(int argc, char *argv[])
{
static   CHAR       ObjectClassName[] = "GEQueryDesktop";
static   CHAR       ObjectName[] = "MyGEQueryDesktop";
static   CHAR       SharedMemName[] = "\\SHAREMEM\\TESTQRY.MEM";
         HAB        hab;
         HINI       hini;
         BOOL       fSuccess;
         HFILE      Handle;
         ULONG      ActionTaken;
         ULONG      DataLen;
         HOBJECT    ObjectHandle;
         HOBJECT    hObject;
         PQDESK_PARM qd;
         int        retc = 0;
         PSZ        DataArea;
         PSZ        DataPtr;
         PSZ        s1;
         PSZ        s2;
         PCHAR      Msg;
         USHORT     RetCode;
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       Application2[] = "PM_InstallObject";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
static   CHAR       Semicolon[] = ";";

if (argc < 2)
   {
   printf("No inifile specified\n");
   return(3);
   }

hab = WinInitialize(0);

if (!hab)
   {
   printf("Unable to initialize application to PM.\n");
   return(4);
   }

if (DosOpen(argv[1],
            &Handle,
            &ActionTaken,
            0L,
            0L,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
            (PEAOP2)NULL))
   {
   printf("Cannot open profile \"%s\".\n", strupr(argv[1]));
   WinTerminate(hab);
   return(5);
   }

DosClose(Handle);

hini = PrfOpenProfile(hab, argv[1]);

if (hini == NULLHANDLE)
   {
   printf("Cannot open profile \"%s\".\n", strupr(argv[1]));
   WinTerminate(hab);
   return(3);
   }

DataLen = sizeof(ObjectHandle);

fSuccess = PrfQueryProfileData(hini,
                               (PSZ)Application,
                               (PSZ)Keyname,
                               (PSZ)&ObjectHandle,
                               &DataLen);


if ((!fSuccess) || (DataLen < sizeof(ObjectHandle)))
   {
   fSuccess = PrfQueryProfileSize(hini,
                                  Application2,
                                  (PSZ)NULL,
                                  &DataLen);

   if (fSuccess && DataLen)
      {
      if ((DataArea = malloc(DataLen)) == (PSZ)NULL)
         {
         PrfCloseProfile(hini);
         printf("Cannot allocate memory\n");
         return(4);
         }
      else
         {
         retc = 2;
         PrfQueryProfileData(hini,
                             Application2,
                             (PSZ)NULL,
                             (PVOID)DataArea,
                             &DataLen);
         PrfCloseProfile(hini);
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
                        retc = 1;
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
      if (retc == 1)
         printf("User .INI file was just created by MAKEINI.\n");
      else
         printf("No valid user .INI file\n");
      return(retc);
      }
   else
      {
      PrfCloseProfile(hini);
      printf("No valid user .INI file\n");
      return(2);
      }
   }

PrfCloseProfile(hini);

hObject = CreateQDObject(&qd, &RetCode, &Msg);

if (!hObject)
   {
   printf("%s\n", Msg);
   if (RetCode == ERROR_CANNOT_CREATE_SHAREDMEM)
      retc = 6;
   else
      {
      printf("Maybe QDESK.DLL is not installed or registered.\n");
      retc = 5;
      }
   }
else
   {
   qd->ObjHandle = ObjectHandle;
   qd->Action = ACTION_QUERY_FROM_OBJECTHANDLE;
   qd->CallNr = 0;
   fSuccess = SetQDObjectData();
   if (qd->CallNr >= 2)
      {
      printf("%s\n", qd->String);
      }
   else
      {
      printf("Desktop directory associated with %s not found\n", argv[1]);
      retc = 4;
      }
   DeleteQDObject();
   }

WinTerminate(hab);

return(retc);
}
