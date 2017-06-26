/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: MEMLAB2.EXE                         ***/
/***                                                    ***/
/***  Created     : 7. May 1990                         ***/
/***                                                    ***/
/***  Author      : Bo Falkenberg                       ***/
/***                                                    ***/
/***  Revised     : February, 1992 by Darryl Frost      ***/
/***                                                    ***/
/***  Purpose     : To demonstrate the different types  ***/
/***                of memory allocation.               ***/
/***                                                    ***/
/***  Compile     : icc /W2 memlab2.c                   ***/
/***                                                    ***/
/***  Execute     : memlab2 (no commandline parameters) ***/
/***                                                    ***/
/***  Input param : 1. Amount of memory in Kb           ***/
/***                2. Type of memory allocation.       ***/
/***                3. Type of memory usage             ***/
/***                                                    ***/
/**********************************************************/
/**********************************************************/

/**********************************************************/
/***  DEFINES                                           ***/
/**********************************************************/
#define INCL_DOSMEMMGR

/**********************************************************/
/***  INCLUDE                                           ***/
/**********************************************************/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************/
/***  FUNCTION PROTOTYPES                               ***/
/**********************************************************/
void main(int argc, char *argv[], char *envp[]);

/**********************************************************/
/***  MAIN PROGRAM                                      ***/
/**********************************************************/
void main(int argc, char *argv[], char *envp[])
{                        /*******************************************/
   PULONG pulBlock;      /* pointer to the starting memory location */
   ULONG ulErr;          /* error variable                          */
   ULONG ulLoop;         /* loop variable                           */
   ULONG ulAmount;       /* amount of memory to be allocated        */
   ULONG ulSelection;    /* input selection                         */
   char cLetter;         /* input char                              */
   BOOL OK = TRUE;       /* memory check indicator                  */
                         /*******************************************/

   setbuf(stdout, NULL);

/* Read the parameters                                */
/*     1. Amount of memory in Kbytes to be allocated  */
/*     2. Combination of allocation flags             */
/*     3. Whether data should read from or written    */
/*        the memory.                                 */
   printf("How much memory (in Kb) do you want to allocate : ");
   scanf("%u", &ulAmount);
   ulAmount = ulAmount * 1024;

   printf("\nWhat type of memory allocation do you want: \n");
   printf("  1. PAG_COMMIT and PAG_READ\n");
   printf("  2. PAG_COMMIT and PAG_WRITE\n");
   printf("  3. PAG_COMMIT and PAG_EXECUTE\n");
   printf("  4. PAG_COMMIT and PAG_GUARD and PAG_WRITE\n");
   printf("  5. PAG_WRITE\n");
   printf("Enter your selection (1, 2, 3, 4 or 5 ) : ");
   scanf("%u", &ulSelection);

   printf("\nDo you want to Read or Write in the memory (R/W) : ");
   fflush(stdin);
   scanf("%c", &cLetter);
/* Allocate the memory                                */
   switch (ulSelection)
   {
      case 1:
         ulErr = DosAllocMem ((PPVOID)&pulBlock, ulAmount,
                               PAG_COMMIT | PAG_READ);
         break;
      case 2:
         ulErr = DosAllocMem ((PPVOID)&pulBlock, ulAmount,
                               PAG_COMMIT | PAG_WRITE);
         break;
      case 3:
         ulErr = DosAllocMem ((PPVOID)&pulBlock, ulAmount,
                               PAG_COMMIT | PAG_EXECUTE);
         break;
      case 4:
         ulErr = DosAllocMem ((PPVOID)&pulBlock, ulAmount,
                               PAG_COMMIT | PAG_GUARD | PAG_WRITE);
         break;
      case 5:
         ulErr = DosAllocMem ((PPVOID)&pulBlock, ulAmount,
                               PAG_WRITE);
         break;
      default:
         printf("\nYou made a WRONG selection !!!\n");
         exit (0);
   } /* endswitch */

   if (ulErr != 0) {
     printf("Error in allocation : code %u\n", ulErr);
     exit(1);
   } /* endif */

   if (cLetter == 'W' || cLetter == 'w')
   {
/* insert data into allocated memory */
      printf("\nWriting...\n");
      for (ulLoop = 0; ulLoop < ulAmount/4; ulLoop++)
      {
         *(pulBlock + ulLoop) = 7;
      } /* endfor */
   } else
   {
/* read data from allocated memory */
      printf("\nReading...\n");
      for (ulLoop = 0; ulLoop < ulAmount/4; ulLoop++)
      {
         if (*(pulBlock + ulLoop) != 7)
         {
            OK = FALSE;
         } /* endif */
      } /* endfor */
   } /* endif */

   ulErr = DosFreeMem (pulBlock);
   if (ulErr != 0)
   {
     printf("\nError in freeing : code %u\n", ulErr);
   } /* endif */
}
