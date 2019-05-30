#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#include <os2.h>

#define MAX_EANAMESIZE   500


         APIRET     GetEALongname(PSZ FileName, PSZ *LongName)
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
   return(ERROR_NOT_ENOUGH_MEMORY);

if ((EA2Name = malloc(MAX_EANAMESIZE)) == (PSZ)NULL)
   {
   free(EAName);
   return(ERROR_NOT_ENOUGH_MEMORY);
   }

pFEA2 = (FEA2 *)EAName;

for (EntryNum = 1; ; EntryNum++)
   {
   EnumCount = 1;
   rc = DosEnumAttribute(ENUMEA_REFTYPE_PATH,
                         FileName,
                         EntryNum,
                         EAName,
                         MAX_EANAMESIZE,
                         &EnumCount,
                         1);
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

         rc = DosQueryPathInfo(FileName,
                               FIL_QUERYEASFROMLIST,
                               (PVOID)&eaopGet,
                               sizeof(EAOP2));
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