#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INCL_WINSHELLDATA
#define INCL_WINWINDOWMGR
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_WPERRORS
#include <os2.h>

#include "chgdt.h"

#define DISPLAY_SYNTAX        1
#define CHANGE_CONFIG_SYS     2
#define CHANGE_ASSOC_DIR      4

         void       Syntax(void);
         USHORT     ChangeConfigSys(PSZ SysIniName, PSZ UserIniName);
         APIRET     GetEALongname(PSZ FileName, PSZ *LongName);

         int        main(int argc, char *argv[])
{
static   CHAR       Application[] = "PM_Workplace:Location";
static   CHAR       Keyname[] = "<WP_DESKTOP>";
         HAB        hab;
         HOBJECT    hObjectDesktop;
         HOBJECT    hObjectDesktop2;
         BOOL       fSuccess;
         PPRFPROFILE pprfProfile;
         PPRFPROFILE pprfProfileNew;
         PVOID      Ptr;
         PVOID      PtrNew;
         PSZ        NewIniName = (PSZ)NULL;
         int        Flag = 0;
         APIRET     rc;
         int        i1;
         PSZ        DTDir;
         PSZ        IniName;
         PSZ        DirName = (PSZ)NULL;
         USHORT     CheckRC;
         PSZ        Longname;
         ULONG      DataLen;
         HINI       hini;
         CHAR       OldIniFile[300];

/* Check parameters */
for (i1 = 1; i1 < argc; i1++)
   {
   if (argv[i1][0] == '/')
      {
      if (!strcmp(argv[i1], "/?"))
         {
         Flag = DISPLAY_SYNTAX;
         break;
         }
      if (!stricmp(argv[i1], "/C"))
         {
         Flag |= CHANGE_CONFIG_SYS;
         continue;
         }
      if (strlen(argv[i1]) > 3)
         {
         if (!memicmp(argv[i1], "/D:", 3))
            {
            Flag |= CHANGE_ASSOC_DIR;
            DirName = &argv[i1][3];
            continue;
            }
         }
      printf("Invalid parameter %s ignored.\n", argv[i1]);
      }
   else
      {
      if (!strcmp(argv[i1], "?"))
         {
         Flag = DISPLAY_SYNTAX;
         break;
         }
      NewIniName = argv[i1];
      }
   }

if (Flag & DISPLAY_SYNTAX)
   {
   Syntax();
   return(1);
   }

hab = WinInitialize(0);

if (!hab)
   {
   printf("Unable to initialize application to PM.\n");
   return(1);
   }

rc = DosAllocMem(&Ptr, ALLOC_SIZE, PAG_COMMIT | PAG_READ | PAG_WRITE);

if (rc)
   {
   printf("Allocate Error, rc = %ld\n", (LONG)rc);
   WinTerminate(hab);
   return(1);
   }

pprfProfile = (PPRFPROFILE)Ptr;
pprfProfile->pszUserName = (PSZ)&pprfProfile[1];
pprfProfile->pszSysName = &pprfProfile->pszUserName[ININAME_LENGTH];
pprfProfile->cchUserName = ININAME_LENGTH;
pprfProfile->cchSysName = ININAME_LENGTH;

fSuccess = PrfQueryProfile(hab, pprfProfile);

if (fSuccess)
   {
   printf("Actual User-Profile: %s\n", pprfProfile->pszUserName);
   printf("Actual System-Profile: %s\n", pprfProfile->pszSysName);
   strcpy(OldIniFile, pprfProfile->pszUserName);
   }
else
   {
   printf("Error in PrfQueryProfile\n");
   DosFreeMem(Ptr);
   WinTerminate(hab);
   return(1);
   }

if (!NewIniName)
   {
   DTDir = QueryDesktopDirectory(HINI_USERPROFILE, &CheckRC);
   if (DTDir)
      {
      printf("The actual desktop directory is %s\n", DTDir);
      GetEALongname(DTDir, &Longname);
      if (Longname)
         {
         printf("The desktop name is %s\n", Longname);
         free(Longname);
         }
      else
         printf("No longname found for the desktop.\n");
      free(DTDir);
      }
   else
      {
      printf("Desktop directory cannot be queried, ");
      switch (CheckRC)
         {
         case 1:
              printf("location for WP_DESKTOP not\nfound in INI file.\n");
              break;
         case 2:
              printf("unable to allocate shared memory.\n");
              break;
         case 3:
              printf("object MyGEQueryObject in class\nGEQueryObject couldn't"
                     " be created. Maybe QDESK.DLL is not installed\nor "
                     "registered in the system.\n");
              break;
         case 4:
              printf("error in WinSetObjectData.\n");
              break;
         } /* endswitch */
      }
   printf("No name for new user ini defined\n");
   DosFreeMem(Ptr);
   WinTerminate(hab);
   return(1);
   }

IniName = malloc(2048);

if (!IniName)
   {
   printf("Allocation error for INI file name.\n");
   DosFreeMem(Ptr);
   WinTerminate(hab);
   return(1);
   }

rc = DosQueryPathInfo(NewIniName, FIL_QUERYFULLNAME, IniName, 2048);

if (rc)
   {
   printf("Error %ld in DosQueryPathInfo for %s.\n", (ULONG)rc, NewIniName);
   free(IniName);
   DosFreeMem(Ptr);
   WinTerminate(hab);
   return(1);
   }

if (Flag & CHANGE_ASSOC_DIR)
   {
   CheckRC = SetDesktopDirectory(IniName, hab, DirName);
   printf("Set desktop directory, rc = %u\n", CheckRC);
   }
else
   CheckRC = CheckIniFile(IniName, hab, &DirName);

switch (CheckRC)
   {
   case 2:
        printf("Ini file %s not found.\n", IniName);
        break;
   case 3:
        printf("The file %s is no valid user ini file or is\ncurrently in use"
               " by the system.\n", IniName);
        break;
   case 4:
        printf("Memory allocation error\n");
        break;
   case 5:
        printf("Handle of desktop object not found in INI file %s.\n",
               IniName);
        break;
   case 6:
        printf("Unable to allocate shared memory.\n");
        break;
   case 7:
        printf("Object MyGEQueryObject in class GEQueryObject couldn't"
               " be created.\nMaybe QDESK.DLL is not installed or "
               "registered in the system.\n");
        break;
   case 8:
        printf("The directory structure associated to inifile %s\n"
               "cannot be located.\n", IniName);
        break;
   case 9:
        printf("The desktop directory %s couldn't be located.\n", DirName);
        break;
   case 10:
        printf("Cannot update INI file %s.\n", IniName);
        break;
   default:
        if (Flag & CHANGE_ASSOC_DIR)
           printf("INI file %s successfully updated with new directory "
                        "structure\n", IniName);
        rc = DosAllocMem(&PtrNew, ALLOC_SIZE,
                         PAG_COMMIT | PAG_READ | PAG_WRITE);

        if (rc)
           {
           printf("Allocate Error, rc = %ld\n", (LONG)rc);
           free(IniName);
           DosFreeMem(Ptr);
           WinTerminate(hab);
           return(1);
           }

        pprfProfileNew = (PPRFPROFILE)PtrNew;
        pprfProfileNew->pszUserName = (PSZ)&pprfProfile[1];
        strcpy(pprfProfileNew->pszUserName, IniName);
        strupr(pprfProfileNew->pszUserName);
        pprfProfileNew->cchUserName = strlen(IniName) + 1;
        pprfProfileNew->pszSysName =
                          &pprfProfileNew->pszUserName[pprfProfileNew->cchUserName + 1];
        strcpy(pprfProfileNew->pszSysName, pprfProfile->pszSysName);
        pprfProfileNew->cchSysName = pprfProfile->cchSysName;

        DataLen = sizeof(hObjectDesktop);
        fSuccess = PrfQueryProfileData(HINI_USERPROFILE,
                                       (PSZ)Application,
                                       (PSZ)Keyname,
                                       (PSZ)&hObjectDesktop,
                                       &DataLen);
        if ((!fSuccess) || (DataLen < sizeof(hObjectDesktop)))
           hObjectDesktop = (HOBJECT)0;

        fSuccess = PrfReset(hab, pprfProfileNew);

        if (fSuccess)
           {
           printf("New User-Profile: %s\nNew System-Profile: %s\n",
                  pprfProfileNew->pszUserName, pprfProfileNew->pszSysName);

           if (!CheckRC)
              {
              if (DirName)
                 {
                 printf("Desktop directory used by %s is %s\n",
                        pprfProfileNew->pszUserName, DirName);
                 GetEALongname(DirName, &Longname);
                 if (Longname)
                    {
                    printf("The desktop name is %s\n", Longname);
                    free(Longname);
                    }
                 else
                    printf("No longname found for the desktop.\n");
                 }
              else
                 {
                 DTDir = QueryDesktopDirectory(HINI_USERPROFILE, &CheckRC);
                 if (DTDir)
                    {
                    printf("Desktop directory used by %s is %s\n",
                           pprfProfileNew->pszUserName, DTDir);
                    GetEALongname(DTDir, &Longname);
                    if (Longname)
                       {
                       printf("The desktop name is %s\n", Longname);
                       free(Longname);
                       }
                    else
                       printf("No longname found for the desktop.\n");
                    free(DTDir);
                    }
                 else
                    {
                    printf("Desktop directory cannot be queried, maybe "
                           "QDESK.DLL is not installed.\n");
                    }
                 }
              }
           if (Flag & CHANGE_CONFIG_SYS)
              {
              ChangeConfigSys(pprfProfileNew->pszSysName,
                              pprfProfileNew->pszUserName);
              }
           if (hObjectDesktop)
              {
              hini = (HINI)NULL;
              i1 = 0;
              while (!hini)
                 {
                 hini = PrfOpenProfile(hab, OldIniFile);
                 if (!hini)
                    {
                    i1++;
                    if (i1 > 60)
                       {
                       break;
                       }
                    DosSleep(1500L);  /* Wait 1.5 seconds, then try again */
                    }
                 }
              if (hini)
                 {
                 DataLen = sizeof(hObjectDesktop2);
                 fSuccess = PrfQueryProfileData(hini,
                                                (PSZ)Application,
                                                (PSZ)Keyname,
                                                (PSZ)&hObjectDesktop2,
                                                &DataLen);
                 if ((!fSuccess) || (DataLen < sizeof(hObjectDesktop2)))
                    {
                    PrfWriteProfileData(hini,
                                        (PSZ)Application,
                                        (PSZ)Keyname,
                                        (PVOID)&hObjectDesktop,
                                        sizeof(HOBJECT));
                    printf("Desktop key in %s updated.\n", OldIniFile);
                    }
                 PrfCloseProfile(hini);
                 }
              }
           }
        else
           printf("Error while switching to %s.\n", IniName);
        DosFreeMem(PtrNew);
   } /* endswitch */

if (DirName && (!(Flag & CHANGE_ASSOC_DIR)))
   free(DirName);

free(IniName);
DosFreeMem(Ptr);

WinTerminate(hab);

return(0);
}

         void       Syntax(void)
{
printf("\nCHGDT - Change actual Desktop\n"
       "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n"
       "Syntax:\n\n"
       "     CHGDT ÄÄÄÂÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÂÄÄÄ´\n"
       "              ³ ÀÄ new ini name ÄÙ  ÀÄ /C ÄÙ  ÀÄ /D:dir name ÄÙ ³\n"
       "              ³                                                 ³\n"
       "              ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ /? ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n"
       "              ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ? ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n"
       "     new ini name     Switch to that ini file\n"
       "     /C               Update CONFIG.SYS\n"
       "     /? or ?          Display the syntax\n"
       "     dir name         Name of associated directory structure to use\n"
       "                      with the new ini file\n\n"
       "     CHGDT without parameter will display the actual INI files and\n"
       "     desktop directory.\n");

}

         USHORT     ChangeConfigSys(PSZ SysIniName, PSZ UserIniName)
{
         CHAR       ConfigSys[20];
         CHAR       NewPath[10];
         PSZ        NewName;
         PSZ        EnvTmp;
         FILE      *Infile;
         FILE      *Outfile;
         CHAR       Buffer[500];
         int        StartPos;
         int        UserIniPos;
         BOOL       Modified = FALSE;
static   CHAR       EnvVarName[] = "TMP";

/* Delete environment variable TMP if exists */
EnvTmp = getenv(EnvVarName);
if (EnvTmp)
   putenv(EnvVarName);

sprintf(NewPath, "%c:\\", SysIniName[0]);
NewName = tempnam(NewPath, EnvVarName);

sprintf(ConfigSys, "%c:\\CONFIG.SYS", SysIniName[0]);

Infile = fopen(ConfigSys, "r");
if (!Infile)
   {
   printf("Cannot open %s\n", ConfigSys);
   return(1);
   }

Outfile = fopen(NewName, "w");
if (!Outfile)
   {
   fclose(Infile);
   printf("Cannot create tempfile %s\n", NewName);
   return(2);
   }

while (fgets(Buffer, sizeof(Buffer), Infile))
   {
   StartPos = strspn(Buffer, " ");
   if (!memicmp(&Buffer[StartPos], "SET ", 4))
      {
      UserIniPos = strspn(&Buffer[StartPos + 4], " ") + StartPos + 4;
      if (!memicmp(&Buffer[UserIniPos], "USER_INI=", 9))
         {
         sprintf(&Buffer[UserIniPos + 9], "%s\n", UserIniName);
         Modified = TRUE;
         }
      }
   fputs(Buffer, Outfile);
   }

fclose(Infile);
fclose(Outfile);

if (Modified)
   {
   if (!DosDelete(ConfigSys))
      {
      if (!DosMove(NewName, ConfigSys))
         printf("%s successfully updated.\n", ConfigSys);
      else
         printf("Cannot rename tempfile %s to %s.\n", NewName, ConfigSys);
      }
   else
      printf("Unable to replace %s.\n", ConfigSys);
   }
else
   {
   DosDelete(NewName);
   printf("Nothing found to update %s.\n", ConfigSys);
   }

free(NewName);

return(0);
}
