#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#define INCL_DOSFILEMGR
#define INCL_WINSHELLDATA
#define INCL_DOSERRORS
#define INCL_WINWORKPLACE
#include <os2.h>

#include <wpfolder.h>

#include "..\qdesk\qdparm.h"

#define HFILE_STDOUT     1
#define HFILE_STDERR     2
#define QUERY_OBJECTS    1
#define UPDATE_INI       2

typedef  struct     _OBJ_DATA
{
         struct _OBJ_DATA *next;
         HOBJECT    ObjHandleOld;
         HOBJECT    ObjHandleNew;
         PSZ        ObjectName;
         PSZ        RealName;
         PSZ        FldrContent;
         ULONG      Length;
}                   OBJ_DATA, * POBJ_DATA;

/******************************************************************************/
/* This program can be used to update all file system object information      */
/* for a desktop that was created on another machine.                         */
/*                                                                            */
/* 1. Step:  Query all file system objects on the machine where the desktop   */
/*           was created and write the information to a control file.         */
/* 2. Step:  Move desktop to another machine by copying the directory         */
/*           structure and INI file.                                          */
/* 3. Step:  Update INI file on new machine with the new file system object   */
/*           handles, the control file created in step 1 is used to locate    */
/*           the directory structure.                                         */
/* 4. Step:  Switch to new INI file with CHGDT                                */
/******************************************************************************/

         VOID       Syntax(VOID);
         VOID       Message(PSZ Msg, ...);
         VOID       Write2Handle(HFILE Handle, PSZ Msg, ...);
         int        QueryObjects(PSZ IniFile, PSZ ControlFile);
         int        UpdateIni(PSZ IniFile, PSZ ControlFile, PSZ Drive,
                              PSZ Path);
         POBJ_DATA  ReadControlFile(PSZ ControlFile, PSZ IniFile, PSZ Drive,
                                    PSZ Path);
         int        GetNewHandles(POBJ_DATA ObjectData);
         int        ChangeIni(HAB hab, PSZ IniFile, POBJ_DATA ObjectData);

         int        main(int argc, char *argv[])
{
         int        i1;
         int        rc = 0;
         ULONG      Flag = 0L;
         PSZ        IniFile = (PSZ)NULL;
         PSZ        ControlFile = (PSZ)NULL;
         PSZ        Drive = (PSZ)NULL;
         PSZ        Path = (PSZ)NULL;
         PSZ        AddParm = (PSZ)NULL;
static   CHAR       Msg1[] = "The parameters /Q and /U cannot be used"
                             " together.\x0d\x0a";
static   CHAR       Msg2[] = "Additional parameter %s ignored.\x0d\x0a";

for (i1 = 1; i1 < argc; i1++)
   {
   if (!strcmp(argv[i1], "?") || !strcmp(argv[i1], "/?"))
      {
      Syntax();
      return(1);
      }
   if (argv[i1][0] == '/')
      {
      if (!stricmp(argv[i1], "/Q"))
         {
         if (Flag == UPDATE_INI)
            {
            Message(Msg1);
            return(2);
            }
         Flag = QUERY_OBJECTS;
         continue;
         }
      if (!stricmp(argv[i1], "/U"))
         {
         if (Flag == QUERY_OBJECTS)
            {
            Message(Msg1);
            return(2);
            }
         Flag = UPDATE_INI;
         continue;
         }
      if (strlen(argv[i1]) > 3)
         {
         if (!memicmp(argv[i1], "/I:", 3))
            {
            if (IniFile)
               {
               Message("Parameter %s ignored, INI file already defined.\x0d\x0a",
                       argv[i1]);
               continue;
               }
            IniFile = &argv[i1][3];
            continue;
            }
         if (!memicmp(argv[i1], "/C:", 3))
            {
            if (ControlFile)
               {
               Message("Parameter %s ignored, control file already defined.\x0d\x0a",
                       argv[i1]);
               continue;
               }
            ControlFile = &argv[i1][3];
            continue;
            }
         if (!memicmp(argv[i1], "/D:", 3))
            {
            if (Drive)
               {
               Message("Parameter %s ignored, drive already defined.\x0d\x0a",
                       argv[i1]);
               continue;
               }
            Drive = &argv[i1][3];
            continue;
            }
         if (!memicmp(argv[i1], "/P:", 3))
            {
            if (Path)
               {
               Message("Parameter %s ignored, path already defined.\x0d\x0a",
                       argv[i1]);
               continue;
               }
            Path = &argv[i1][3];

            if (Path[0] == '\\')
               Path++;

            if (strlen(Path))
               if (Path[strlen(Path) - 1] == '\\')
                  Path[strlen(Path) - 1] = '\0';

            if (!strlen(Path))
               Path = (PSZ)NULL;
            continue;
            }
         }
      Message("Invalid or incomplete parameter %s ignored.\x0d\x0a",
              argv[i1]);
      }
   else
      {
      if (AddParm)
         Message(Msg2, argv[i1]);
      else
         AddParm = argv[i1];
      }
   }
if (!Flag)
   {
   Message("Either parameter /Q or /U must be specified.\x0d\x0a");
   return(2);
   }

if (Flag == UPDATE_INI)
   {
   if (ControlFile)
      {
      if (AddParm)
         Message(Msg2, AddParm);
      }
   else
      if (AddParm)
         ControlFile = AddParm;
   if (!ControlFile)
      {
      Message("No control file name for updating user INI defined.\x0d\x0a");
      return(2);
      }
   if (!IniFile)
      {
      Message("No INI file name for updating defined.\x0d\x0a");
      return(2);
      }
   rc = UpdateIni(IniFile, ControlFile, Drive, Path);
   }
else
   rc = QueryObjects(IniFile, ControlFile);

return(rc);
}

/******************************************************************************/
/* Display Syntax of program call                                             */
/******************************************************************************/
         VOID       Syntax(VOID)
{
         ULONG      BytesWritten;
static   CHAR       SyntaxText[] =
"DTUPD - Query file system objects and update user INI file\x0D\x0A"
"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\x0D\x0A\x0D\x0A"
"To query actual file system objects:\x0D\x0A\x0D\x0A"
"   DTUPD ÄÄÄÄÄÄÄ /Q ÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄ´\x0D\x0A"
"                         ÀÄ /I:ini file ÄÙ   ÀÄ /C:control file ÄÙ\x0D\x0A\x0D\x0A"
"     ini file            Name of ini file to query. If not defined,\x0D\x0A"
"                         actual user INI will be queried.\x0D\x0A"
"     control file        Name of control file that will be created.\x0D\x0A"
"                         If not defined, output will be written to\x0D\x0A"
"                         standard output.\x0D\x0A\x0D\x0A"
"To update user INI file with new handles of file system objects:\x0D\x0A\x0D\x0A"
"   DTUPD ÄÄÄ /U ÄÂÄÄ ctlfile ÄÄÄÂÄ /I:inifile ÄÂÄÄÄÄÄÄÄÄÄÄÂÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄ´\x0D\x0A"
"                 ÀÄ /C:ctlfile ÄÙ              ÀÄ /D:drv ÄÙ ÀÄ /P:path ÄÙ\x0D\x0A\x0D\x0A"
"     ctlfile             Name of control file that was created with\x0D\x0A"
"                         DTUPD /Q ...\x0D\x0A"
"     inifile             Location and name of INI file.\x0D\x0A"
"     drv                 If desktop directory structure is located on\x0D\x0A"
"                         another drive as on the machine, where the control\x0D\x0A"
"                         file was created, the driveletter can be overwritten.\x0D\x0A"
"     path                Path, if copied desktop directory is renamed.\x0D\x0A";
DosWrite(HFILE_STDERR,
         SyntaxText,
         sizeof(SyntaxText) - 1,
         &BytesWritten);
}

/******************************************************************************/
/* Write message to STDERR                                                    */
/******************************************************************************/
         VOID       Message(PSZ Msg, ...)
{
         ULONG      BytesWritten;
         va_list    arg_ptr;
         CHAR       MSG[500];

va_start(arg_ptr, Msg);

vsprintf(MSG, Msg, arg_ptr);

DosWrite(HFILE_STDERR,
         MSG,
         strlen(MSG),
         &BytesWritten);

va_end(arg_ptr);
}

/******************************************************************************/
/* Query Objects                                                              */
/******************************************************************************/
         int        QueryObjects(PSZ IniFile, PSZ ControlFile)
{
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       ObjectClassName[] = "GEQueryDesktop";
static   CHAR       ObjectName[] = "MyGEQueryDesktop";
static   CHAR       SharedMemName[] = "\\SHAREMEM\\TESTQRY.MEM";
         HINI       hini = (HINI)NULL;
         HAB        hab;
         HOBJECT    hObject;
         HFILE      Handle;
         ULONG      ActionTaken;
         ULONG      DataLen;
         BOOL       fSuccess;
         APIRET     rc;
         PSZ        pszRealName;
         PSZ        Buffer;
         PSZ        BufferPtr;
         PQDESK_PARM qd;
         USHORT     ObjectCount = 0;

hab = WinInitialize(0);

if (!hab)
   {
   Message("Unable to initialize application to PM.\x0D\x0A");
   return(2);
   }

if (IniFile)
   {
   /* Check, if file exists */
   if (DosOpen(IniFile,
               &Handle,
               &ActionTaken,
               0L,
               0L,
               OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
               OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
               (PEAOP2)NULL))
      Message("Ini file %s does not exist.\x0d\x0a", IniFile);
   else
      {
      DosClose(Handle);
      hini = PrfOpenProfile(hab, IniFile);
      if (!hini)
         Message("Cannot open Ini file %s\x0D\x0A", IniFile);
      }
   }
else
   hini = HINI_USERPROFILE;

if (hini)
   {
   if (ControlFile)
      {
      if (DosOpen(ControlFile,
                  &Handle,
                  &ActionTaken,
                  0L,
                  FILE_ARCHIVED,
                  OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_WRITEONLY,
                  (PEAOP2)NULL))
         {
         Message("Cannot create control file %s\n", ControlFile);
         Handle = (HFILE)NULL;
         }
      }
   else
      Handle = HFILE_STDOUT;
   if (Handle)
      {
      fSuccess = PrfQueryProfileSize(hini,
                                     (PSZ)Application,
                                     (PSZ)NULL,
                                     &DataLen);
      if (!fSuccess || !DataLen)
         Message("Cannot find any file system objects in %s\x0D\x0A", IniFile);
      else
         {
         if ((Buffer = malloc(DataLen)) == (PSZ)NULL)
            Message("Cannot allocate memory\x0D\x0A");
         else
            {
            rc = DosAllocSharedMem((PVOID *)&pszRealName, SharedMemName,
                                   sizeof(QDESK_PARM), PAG_COMMIT | PAG_WRITE);

            if (rc == ERROR_ALREADY_EXISTS)
               {
               rc = DosGetNamedSharedMem((PVOID *)&pszRealName, SharedMemName,
                                         PAG_WRITE | PAG_READ);
               }
            if (rc)
               Message("Cannot access or create shared memory, rc = %ld\x0D\x0A",
                       (LONG)rc);
            else
               {
               qd = (PQDESK_PARM)pszRealName;
               hObject = WinCreateObject(ObjectClassName, ObjectName,
                                         "NOTVISIBLE=YES", "<WP_DESKTOP>",
                                         CO_UPDATEIFEXISTS);

               if (hObject)
                  {
                  PrfQueryProfileString(hini,
                                        (PSZ)Application,
                                        (PSZ)NULL,
                                        (PSZ)NULL,
                                        Buffer,
                                        DataLen);
                  for (BufferPtr = Buffer; BufferPtr[0];
                       BufferPtr = &BufferPtr[strlen(BufferPtr) + 1])
                     {
                     DataLen = sizeof(qd->ObjHandle);
                     PrfQueryProfileData(hini,
                                         (PSZ)Application,
                                         BufferPtr,
                                         (PVOID)&qd->ObjHandle,
                                         &DataLen);

                     if ((DataLen == sizeof(qd->ObjHandle)) &&
                         ((qd->ObjHandle & 0xFFFF0000) == 0x00030000))
                        {
                        qd->Action = ACTION_QUERY_FROM_OBJECTHANDLE;
                        fSuccess = WinSetObjectData(hObject,
                                     "SHAREMEM=\\SHAREMEM\\TESTQRY.MEM;");
                        if (fSuccess && (qd->CallNr >= 2))
                           {
                           if (!ObjectCount)
                              {
                              if (IniFile)
                                 {
                                 Write2Handle(Handle,
                                              "INIFILE:%s\x0D\x0A",
                                              IniFile);
                                 }
                              }
                           Write2Handle(Handle,
                                        "Object:%s,%s,%lX\x0D\x0A",
                                        BufferPtr, qd->String,
                                        qd->ObjHandle);

                           ObjectCount++;
                           }
                        }
                     }
                  WinDestroyObject(hObject);
                  }
               else
                  Message("Object %s in class %s couldn't be created, maybe\x0D"
                          "\x0A QDESK.DLL is not installed or registered\x0D\x0A",
                          ObjectName, ObjectClassName);
               DosFreeMem(pszRealName);
               }
            }
         free(Buffer);
         }
      if (Handle != HFILE_STDOUT)
         DosClose(Handle);
      }
   if (hini != HINI_USERPROFILE)
      PrfCloseProfile(hini);
   }

WinTerminate(hab);

if (ObjectCount)
   {
   Message("%s successfully created, %ld file system objects found.\x0D\x0A",
           ControlFile, (ULONG)ObjectCount);
   }
else
   {
   Message("No file system objects found\x0D\x0A");
   return(2);
   }

return(0);
}

/******************************************************************************/
/* Write message to STDERR                                                    */
/******************************************************************************/
         VOID       Write2Handle(HFILE Handle, PSZ Msg, ...)
{
         ULONG      BytesWritten;
         va_list    arg_ptr;
         CHAR       MSG[500];

va_start(arg_ptr, Msg);

vsprintf(MSG, Msg, arg_ptr);

DosWrite(Handle,
         MSG,
         strlen(MSG),
         &BytesWritten);

va_end(arg_ptr);
}

/******************************************************************************/
/* Read control file                                                          */
/******************************************************************************/
         POBJ_DATA  ReadControlFile(PSZ ControlFile, PSZ IniFile, PSZ Drive,
                                    PSZ Path)
{
static   CHAR       Delimiter[] = ",";
         FILE      *Handle;
         CHAR       Buffer[300];
         CHAR       RealName[256];
         ULONG      ObjectHandle;
         POBJ_DATA  Chain = (POBJ_DATA)NULL;
         POBJ_DATA  Element;
         POBJ_DATA  Ptr;
         PSZ        s1;
         PSZ        s2;
         PSZ        s3;
         PSZ        s4;
         int        i1;

if ((Handle = fopen(ControlFile, "r")) == (FILE *)NULL)
   {
   Message("Cannot open control file %s\x0D\x0A", ControlFile);
   return((POBJ_DATA)NULL);
   }

while (fgets(Buffer, sizeof(Buffer), Handle))
   {
   i1 = strlen(Buffer) - 1;
   if (Buffer[i1] == '\n')
      Buffer[i1] = '\0';
   s1 = strtok(Buffer, Delimiter);
   if (s1)
      {
      if (!memicmp(s1, "Object:", 7))
         {
         s2 = strtok(NULL, Delimiter);
         if (s2)
            {
            s3 = strtok(NULL, Delimiter);
            if (s3)
               {
               for (ObjectHandle = 0; s3[0]; s3++)
                  {
                  ObjectHandle <<= 4;
                  if ((s3[0] >= '0') && (s3[0] <= '9'))
                     {
                     ObjectHandle += (ULONG)(USHORT)(s3[0] - '0');
                     }
                  else
                  if ((s3[0] >= 'a') && (s3[0] <= 'f'))
                     {
                     ObjectHandle += (10 + (ULONG)(USHORT)(s3[0] - 'a'));
                     }
                  else
                  if ((s3[0] >= 'A') && (s3[0] <= 'F'))
                     {
                     ObjectHandle += (10 + (ULONG)(USHORT)(s3[0] - 'A'));
                     }
                  }
               if (Path)
                  {
                  if (!memcmp(&s2[1], ":\\", 2))
                     {
                     s4 = strchr(&s2[3], '\\');
                     if (s4)
                        {
                        sprintf(RealName, "%c:\\%s%s", s2[0], Path, s4);
                        s2 = RealName;
                        }
                     }
                  }
               i1 = sizeof(OBJ_DATA) + 2 + strlen(&s1[7]) + strlen(s2);
               if ((Element = (POBJ_DATA)malloc(i1)) == (POBJ_DATA)NULL)
                  Message("Cannot allocate memory for Object %s\x0D\x0A",
                          &s1[7]);
               else
                  {
                  memset((char *)Element, '\0', i1);
                  Element->ObjectName = (PSZ)&Element[1];
                  strcpy(Element->ObjectName, &s1[7]);
                  Element->RealName = &Element->ObjectName[strlen(&s1[7]) + 1];
                  strcpy(Element->RealName, s2);
                  if (Drive)
                     Element->RealName[0] = Drive[0];
                  Element->ObjHandleOld = (HOBJECT)ObjectHandle;
                  if (Chain)
                     {
                     for (Ptr = Chain; Ptr->next; Ptr = Ptr->next);
                     Ptr->next = Element;
                     }
                  else
                     Chain = Element;
                  }
               }
            }
         }
      else
      if (!memicmp(s1, "INIFILE:", 8))
         {
         strcpy(IniFile, &s1[8]);
         }

      }
   }

fclose(Handle);
return(Chain);
}

/******************************************************************************/
/* Get new handles from ini file                                              */
/******************************************************************************/
         int        GetNewHandles(POBJ_DATA ObjectData)
{
         POBJ_DATA  Ptr;

for (Ptr = ObjectData; Ptr; Ptr = Ptr->next)
   {
   Ptr->ObjHandleNew = WinQueryObject(Ptr->RealName);
   if (!Ptr->ObjHandleNew)
      Message("Cannot find object %s\x0D\x0A", Ptr->RealName);
   }

return(0);
}

/******************************************************************************/
/* Make the changes in the INI file                                           */
/******************************************************************************/
         int        ChangeIni(HAB hab, PSZ IniFile, POBJ_DATA ObjectData)
{
static   CHAR       Application1[] = "PM_Abstract:FldrContent";
static   CHAR       Application2[] = "PM_Workplace:Location";
         HINI       hini;
         HFILE      Handle;
         ULONG      ActionTaken;
         ULONG      DataLen;
         ULONG      KeysChanged = 0;
         BOOL       fSuccess;
         POBJ_DATA  Ptr;
         CHAR       Key[10];

if (DosOpen(IniFile,
            &Handle,
            &ActionTaken,
            0L,
            0L,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
            (PEAOP2)NULL))
   {
   Message("Ini file %s does not exist.\x0d\x0a", IniFile);
   return(2);
   }

DosClose(Handle);
hini = PrfOpenProfile(hab, IniFile);
if (!hini)
   {
   Message("Cannot open Ini file %s\x0D\x0A", IniFile);
   return(2);
   }

/* Get contents of ini file first */
for (Ptr = ObjectData; Ptr; Ptr = Ptr->next)
   {
   if (!Ptr->ObjHandleNew)
      continue;
   sprintf(Key, "%lX", (LONG)(Ptr->ObjHandleOld & 0x0000FFFF));
   fSuccess = PrfQueryProfileSize(hini,
                                  (PSZ)Application1,
                                  (PSZ)Key,
                                  &DataLen);
   if (fSuccess && DataLen)
      {
      Ptr->FldrContent = malloc(DataLen);
      if (Ptr->FldrContent)
         {
         PrfQueryProfileData(hini,
                             (PSZ)Application1,
                             (PSZ)Key,
                             (PSZ)Ptr->FldrContent,
                             &DataLen);
         Ptr->Length = DataLen;
         /* Delete keys */
         PrfWriteProfileData(hini,
                             (PSZ)Application1,
                             (PSZ)Key,
                             (PSZ)NULL,
                             0);
         KeysChanged++;
         }
      }
   }

/* Add new keys */
for (Ptr = ObjectData; Ptr; Ptr = Ptr->next)
   {
   if (Ptr->ObjHandleNew)
      {
      /* Delete key first */
      PrfWriteProfileData(hini,
                          (PSZ)Application2,
                          (PSZ)Ptr->ObjectName,
                          (PSZ)NULL,
                          0);

      PrfWriteProfileData(hini,
                          (PSZ)Application2,
                          (PSZ)Ptr->ObjectName,
                          (PSZ)&Ptr->ObjHandleNew,
                          sizeof(ULONG));
      KeysChanged++;
      }
   if (Ptr->ObjHandleNew && Ptr->Length)
      {
      sprintf(Key, "%lX", (LONG)(Ptr->ObjHandleNew & 0x0000FFFF));
      PrfWriteProfileData(hini,
                          (PSZ)Application1,
                          (PSZ)Key,
                          (PSZ)Ptr->FldrContent,
                          Ptr->Length);
      KeysChanged++;
      }
   }

PrfCloseProfile(hini);
if (KeysChanged)
   {
   Message("%ld keys updated in user INI file.\x0D\x0A", KeysChanged);
   }

return(0);
}

/******************************************************************************/
/* Update INI file                                                            */
/******************************************************************************/
         int        UpdateIni(PSZ IniFile, PSZ ControlFile, PSZ Drive,
                              PSZ Path)
{
         HAB        hab;
         int        rc = 0;
         POBJ_DATA  ObjDataChain;
         CHAR       IniFile2[200];

hab = WinInitialize(0);

if (!hab)
   {
   Message("Unable to initialize application to PM.\x0D\x0A");
   return(2);
   }

if ((ObjDataChain = ReadControlFile(ControlFile, IniFile2, Drive, Path)) ==
    (POBJ_DATA)NULL)
   rc = 2;
else
   {
   rc = GetNewHandles(ObjDataChain);
   if (!rc)
      rc = ChangeIni(hab, IniFile, ObjDataChain);
   }

WinTerminate(hab);

return(rc);
}
