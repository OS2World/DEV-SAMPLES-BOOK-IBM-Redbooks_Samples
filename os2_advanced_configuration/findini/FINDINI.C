/* Find Valid User INI files         */

#define INCL_WIN
#define INCL_DOS
#define INCL_GPI
#define INCL_GPIBITMAPS

#define ININAME_LENGTH 200
#define ALLOC_SIZE 4096


#include <os2.h>                       /* System Include File      */
#include <string.h>                    /* String Functions Include File */
#include <wpfolder.h>

#include "findini.h"                   /* Application Include File */

HAB         hab;
USHORT      ix, iy, flag, newone, first, WhichPtr, ii, found, rrc;
char        INIFILE[20], SmLStr[40];
char        *stringg[128], arrayIN[40][128], arrayNI[40][128];
char        Mess[10][56];
char        fullname[128];
char        *Buff;


main(int argc, char *argv[], char *envp[])
{
   if (argc > 1)
   {
      printf("Find Desktop INI Files, Directory and Desktop Name Program\n");
      printf("No Parameters are required.\n");
      return(0);
   }

hab = WinInitialize((USHORT) NULL);

if (!hab)
   {
   printf("\nError - Unable to WinInitialize.");
   return(4);
   }

   found=0;
   GetPath(stringg);
   GetINIs(stringg, arrayIN);
   CheckprfINI(stringg, arrayIN);
   if(found==0)
      {
   printf("\nError - No Free INI Files found.");
      }

   WinTerminate(hab);
}

GetPath(char *pather)
{
   char *sysinivar;

   sysinivar = getenv("SYSTEM_INI");
   strncpy(pather, sysinivar, 1);
   pather[1]='\0';
   strcat(pather, ":\\OS2\\");
}

GetINIs(char *LookPath, char INIArray[40][128])
{
   int          x, rc;
   char         FindPath[128];
   FILEFINDBUF3 findBuffer;
   ULONG        fileCount;
   HDIR         fileHandle;


    strcpy(FindPath, LookPath);
    strcat(FindPath,"*.INI");

      fileHandle = 0x0001;
      fileCount  = 1;
 if ( !(rc = DosFindFirst(FindPath, &fileHandle,
                          0L, (PVOID)&findBuffer,
                          sizeof(findBuffer), &fileCount,
                          FIL_STANDARD)) )

    {
       x = 0;
    do
      {
           strcpy(INIArray[x],findBuffer.achName);
           x++;
      }
    while ( !(rc = DosFindNext(fileHandle, (PVOID)&findBuffer,
                               sizeof(findBuffer), &fileCount)) );

    }
    DosFindClose(fileHandle);

}

CheckINI(char *sPath, char Fname[40][128])
{
  char       FindMe[128];
  int        x = 0;
  HFILE      Handle;
  ULONG      ActionTaken;


  while (strncmp(Fname[x], "", 1))
  {

  strcpy(FindMe, sPath);
  strcat(FindMe, Fname[x]);

 if (DosOpen(FindMe, &Handle, &ActionTaken, 0L, 0L,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
            (PEAOP2)NULL))
   {
   return(5);
   }
   else
   {
    DosClose(Handle);
   }
   x++;
   }
}

CheckprfINI(char *sPath, char Fname[40][128])
{
   HINI       hini;
   char       FindMe[128];
   int        x = 0, flag;
   BOOL       fSuccess;
   ULONG      DataLen;
   PSZ        DataArea;
   PSZ        DataPtr;
   PSZ        s1;
   PSZ        s2;
   HOBJECT    ObjectHandle;
   HOBJECT    hObject;
   PCHAR      Msg;
   USHORT     retc;
   USHORT     RetCode;
   PQDESK_PARM qd;
static   CHAR Application[] = "PM_Workplace:Location";
static   CHAR Application2[] = "PM_InstallObject";
static   CHAR Keyname[] = "<WP_DESKTOP>";
static   CHAR Semicolon[] = ";";

  while (strncmp(Fname[x], "", 1))
{

  strcpy(FindMe, sPath);
  strcat(FindMe, Fname[x]);
  flag = 0;

hini = PrfOpenProfile(hab, FindMe);

if (hini == NULLHANDLE)
   {
   flag = 1;
   }
DataLen = sizeof(ObjectHandle);

fSuccess = PrfQueryProfileData(hini, (PSZ)Application,
                               (PSZ)Keyname, (PSZ)&ObjectHandle,
                               &DataLen);


if ((!fSuccess) || (DataLen < sizeof(ObjectHandle)))
   {
   fSuccess = PrfQueryProfileSize(hini, Application2,
                                  (PSZ)NULL, &DataLen);

   if (fSuccess && DataLen)
      {
      if ((DataArea = malloc(DataLen)) == (PSZ)NULL)
         {
         PrfCloseProfile(hini);
         return(4);
         }
      else
         {
         retc = 2;
         PrfQueryProfileData(hini, Application2, (PSZ)NULL,
                              (PVOID)DataArea, &DataLen);
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
                        flag = 1;
                        }
                     }
                  }
               free(s1);
               }
            }
         free(DataArea);
         }
      if (retc == 1)
      {
         flag=8;
         }
      else

    flag = retc;
      }
   else
      {
      PrfCloseProfile(hini);
    flag = 2;
      }
   }
   PrfCloseProfile(hini);

   if (flag == 8)
   {
      strcpy(fullname, stringg);
      strcat(fullname, Fname[x]);
   printf("\n%s - New INI File", fullname);
   strcpy(arrayNI[found],Fname[x]);
   found=found+1;
   }
   if (flag == 0)
   {
      strcpy(fullname, stringg);
      strcat(fullname, Fname[x]);
   printf("\n%s", fullname);
   strcpy(arrayNI[found],Fname[x]);
   found=found+1;

   hObject = CreateQDObject(&qd, &RetCode, &Msg);
   if (!hObject)
   {
   printf(" - Error - Unable to Query Directory.\n");
     return(5);
   } /* endif */

    qd->ObjHandle = ObjectHandle;
    qd->Action = ACTION_QUERY_FROM_OBJECTHANDLE;

    fSuccess = SetQDObjectData();
    if (qd->CallNr >=2)
    {
     printf(" - %s",qd->String);

     strcpy(SmLStr, qd->String);

     GetEALongname(SmLStr, &Buff);

     printf(" - %s", Buff);

    } else {
   printf("Error - Unable to Get Desktop Name.\n");
    } /* endif */
    DeleteQDObject();
   }
   x++;
  }
   return(0);
}

GetEALongname(PSZ FileName, PSZ *LongName)
{
         APIRET     rc;
         ULONG      EntryNum;
         ULONG      EnumCount;
         PSZ        EAName;
         PSZ        EA2Name;
         PSZ        EAData;
         PSZ        Result;
         PSZ        EAPtr;
         FEA2      *pFEA2;
         EAOP2      eaopGet;
         GEA2LIST  *pGEAList;
         USHORT     EADataLen;

*LongName = (PSZ)NULL;

if ((EAName = malloc(MAX_EANAMESIZE)) == (PSZ)NULL)
   return(99);

if ((EA2Name = malloc(MAX_EANAMESIZE)) == (PSZ)NULL)
   {
   free(EAName);
   return(99);
   }

pFEA2 = (FEA2 *)EAName;

for (EntryNum = 1; ; EntryNum++)
   {
   EnumCount = 1;
   rc = DosEnumAttribute(ENUMEA_REFTYPE_PATH,
                         FileName, EntryNum,
                         EAName, MAX_EANAMESIZE,
                         &EnumCount, 1);
   if (rc)
      break;

   if (!EnumCount)
      break;

   if (!strcmp(pFEA2->szName, ".LONGNAME"))
      {
      EADataLen = sizeof(FEA2LIST) + pFEA2->cbName + 1 + pFEA2->cbValue;

      EAData = malloc(EADataLen);

      if (EAData)
         {
         pGEAList = eaopGet.fpGEA2List = (GEA2LIST *)EA2Name;
         eaopGet.fpFEA2List = (FEA2LIST *)EAData;

         pGEAList->cbList = sizeof(GEA2LIST) + pFEA2->cbName; /* +1 for NULL */
         pGEAList->list[0].oNextEntryOffset = 0L;
         pGEAList->list[0].cbName = pFEA2->cbName;
         strcpy(pGEAList->list[0].szName, pFEA2->szName);

         eaopGet.fpFEA2List->cbList = EADataLen;

         rc = DosQueryPathInfo(FileName, FIL_QUERYEASFROMLIST,
                               (PVOID)&eaopGet, sizeof(EAOP2));
         if (!rc)
            {
            EAPtr = &EAData[sizeof(FEA2LIST) + pFEA2->cbName];
            if ((*(USHORT *)EAPtr) == EAT_ASCII)
               {
               Result = malloc(1 + (unsigned int)*(USHORT *)(&EAPtr[2]));
               if (Result)
                  {
                  memcpy(Result, &EAPtr[4],
                         (unsigned int)*(USHORT *)(&EAPtr[2]));
                  Result[(unsigned int)*(USHORT *)(&EAPtr[2])] = '\0';
                  *LongName = Result;
                  }
               }
            }
         free(EAData);
         }
      break;
      }
   }

free(EA2Name);
free(EAName);
return(rc);
}
