#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_WINSHELLDATA
#define INCL_WINWINDOWMGR
#include <os2.h>

#define DEL_FILE    0
#define DEL_DIR     1

typedef  struct     _FILE_DEL_INFO
{
         struct _FILE_DEL_INFO *next;
         PCHAR      Name;
         SHORT      Flag;
}                   FILE_DEL_INFO, * PFILE_DEL_INFO;

static   PFILE_DEL_INFO DelList = (PFILE_DEL_INFO)NULL;

         VOID       Delete(PCHAR FileName, PCHAR PathName);
         VOID       Add2List(PCHAR Name, SHORT Flag);

         int        main(int argc, char *argv[])
{
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
         PCHAR      FileName;
         PCHAR      PathName;
         PCHAR      s1;
         PFILE_DEL_INFO ActDel;
         PFILE_DEL_INFO TempDel;
         HOBJECT    hObjectDesktop;
         ULONG      DataLen;
         BOOL       fSuccess;
         USHORT     FilesCt;
         USHORT     DirCt;

if (argc < 2)
   {
   printf("No filename/directory name specified\n");
   exit(1);
   }

/* Separate path / filename */
s1 = strrchr(argv[1], '\\');

if (s1)
   {
   PathName = strdup(argv[1]);
   (strrchr(PathName, '\\'))[1] = '\0';
   FileName = strdup(&s1[1]);
   }
else
   {
   s1 = strchr(argv[1], ':');
   if (s1)
      {
      PathName = strdup(argv[1]);
      (strchr(PathName, ':'))[1] = '\0';
      FileName = strdup(&s1[1]);
      }
   else
      {
      FileName = strdup(argv[1]);
      PathName = strdup("");
      }
   }

DataLen = sizeof(hObjectDesktop);

fSuccess = PrfQueryProfileData(HINI_USERPROFILE,
                               (PSZ)Application,
                               (PSZ)Keyname,
                               (PSZ)&hObjectDesktop,
                               &DataLen);

if ((!fSuccess) || (DataLen < sizeof(hObjectDesktop)))
   hObjectDesktop = (HOBJECT)0;

Delete(FileName, PathName);

free(PathName);
free(FileName);

FilesCt = DirCt = 0;

for (ActDel = DelList; ActDel != (PFILE_DEL_INFO)NULL;)
   {
   if (ActDel->Flag == DEL_FILE)
      {
      if (DosDelete(ActDel->Name))
         printf("Cannot delete file \"%s\"\n", ActDel->Name);
      else
         FilesCt++;
      }
   else
      {
      if (DosDeleteDir(ActDel->Name))
         printf("Cannot remove directory \"%s\".\n", ActDel->Name);
      else
         DirCt++;
      }
   TempDel = ActDel;
   ActDel = ActDel->next;
   free((PCHAR)TempDel);
   }

if (!FilesCt && !DirCt)
   printf("DELALL: Nothing found to delete.\n");
else
   printf("Files deleted: %d\nDirectories removed: %d\n", FilesCt, DirCt);

if (hObjectDesktop)
   {
   DosSleep(5000L);
   PrfWriteProfileData(HINI_USERPROFILE,
                       (PSZ)Application,
                       (PSZ)Keyname,
                       (PVOID)&hObjectDesktop,
                       sizeof(HOBJECT));
   }

return(0);
}

         VOID       Delete(PCHAR FileName, PCHAR PathName)
{
         APIRET     rc;
         HDIR       DirHandle = 0xFFFFFFFF;
         FILEFINDBUF3 Buffer;
         ULONG      Count = 1L;
         PCHAR      ActualSearchDir;
         PCHAR      NewSearchDir;
         PCHAR      ActualFile;

ActualSearchDir = malloc(strlen(FileName) + strlen(PathName) + 1);

if (!ActualSearchDir)
   return;

ActualFile = malloc(strlen(PathName) + 260);
if (!ActualFile)
   {
   free(ActualSearchDir);
   return;
   }

strcpy(ActualSearchDir, PathName);
strcat(ActualSearchDir, FileName);

rc = DosFindFirst(ActualSearchDir,
                  &DirHandle,
                  FILE_ARCHIVED | FILE_DIRECTORY | FILE_SYSTEM | FILE_HIDDEN |
                  FILE_READONLY,
                  (PVOID)&Buffer,
                  sizeof(Buffer),
                  &Count,
                  FIL_STANDARD);

while (!rc)
   {
   if (strcmp(Buffer.achName, ".") && strcmp(Buffer.achName, ".."))
      {
      strcpy(ActualFile, PathName);
      strcat(ActualFile, Buffer.achName);
      if (Buffer.attrFile & FILE_DIRECTORY)
         {
         NewSearchDir = malloc(strlen(PathName) + strlen(Buffer.achName) + 2);
         if (NewSearchDir)
            {
            strcpy(NewSearchDir, PathName);
            strcat(NewSearchDir, Buffer.achName);
            strcat(NewSearchDir, "\\");

            Delete("*.*", NewSearchDir);

            Add2List(ActualFile, DEL_DIR);
/*            if (DosDeleteDir(ActualFile))
               printf("Cannot remove directory \"%s\".\n", ActualFile);
            else
               {
               FileCount += (LONG)0x10000;
               } */

            free(NewSearchDir);
            }
         }
      else
         {
         Add2List(ActualFile, DEL_FILE);
/*         if (DosDelete(ActualFile))
            printf("Cannot delete file \"%s\"\n", ActualFile);
         else
            {
            FileCount++;
            } */
         }
      }
   Count = 1l;
   rc = DosFindNext(DirHandle,
                    (PVOID)&Buffer,
                    sizeof(Buffer),
                    &Count);
   }

DosFindClose(DirHandle);
free(ActualSearchDir);
free(ActualFile);

return;
}

/******************************************************************************/
/* Add file or directory to chained list                                      */
/******************************************************************************/
         VOID       Add2List(PCHAR Name, SHORT Flag)
{
         PFILE_DEL_INFO Ptr;
         PFILE_DEL_INFO ActPtr;
         ULONG      i1;

i1 = sizeof(FILE_DEL_INFO) + strlen(Name) + 1;
if ((Ptr = (PFILE_DEL_INFO)malloc(i1)) == (PFILE_DEL_INFO)NULL)
   {
   printf("Not enough memory to delete all specified files/directories.\n");
   exit(1);
   }
memset((PCHAR)Ptr, '\0', i1);

Ptr->Name = (PCHAR)&Ptr[1];
strcpy(Ptr->Name, Name);
Ptr->Flag = Flag;

if (DelList == (PFILE_DEL_INFO)NULL)
   DelList = Ptr;
else
   {
   for (ActPtr = DelList; ; ActPtr = ActPtr->next)
      if (ActPtr->next == (PFILE_DEL_INFO)NULL)
         {
         ActPtr->next = Ptr;
         break;
         }
   }
}
