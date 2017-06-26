/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: MEMLAB1.EXE                         ***/
/***                                                    ***/
/***  Created     : 7. May 1990                         ***/
/***                                                    ***/
/***  Author      : Bo Falkenberg                       ***/
/***                                                    ***/
/***  Revised     : February, 1992 by Darryl Frost      ***/
/***                                                    ***/
/***  Purpose     : To demonstrate the use of the new   ***/
/***                DosAllocMem API, and the handling   ***/
/***                of General Protection Exceptions.   ***/
/***                                                    ***/
/***  Compile     : icc /W2 memlab1.c;                  ***/
/***                                                    ***/
/***  Execute     : memlab1 (No command line parameters)***/
/***                                                    ***/
/***  Input param : 1. Memory to allocate               ***/
/***                2. Memory to use for read/write     ***/
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
#include <signal.h>

/***      GLOBAL VARIABLES                              ***/
ULONG     ulLoop;     /* loop variable                    */

/***      FUNCTION PROTOTYPES                           ***/
void main(int argc, char *argv[], char *envp[]);
void traphandler(int sig);
void normalexit(void);

/***  MAIN PROGRAM                                      ***/
void main(int argc, char *argv[], char *envp[])
{                        /*******************************************/
   PULONG    pulBlock;   /* pointer to the starting memory location */
   ULONG     ulErr;      /* error variable                          */
   ULONG     ulAmount;   /* amount of memory to be allocated        */
                         /*             (long integers)             */
   ULONG     ulBytes;    /* amount of memory to be allocated (bytes)*/
   ULONG     ulUse;      /* amount of memory to be used             */
                         /*             (long integers)             */
   BOOL      OK = TRUE;  /* memory check indicator                  */
                         /*******************************************/
   setbuf(stdout, NULL);
/* Register an exception handler for memory exception     */
   if (signal(SIGSEGV, traphandler) != SIG_ERR)
      printf("\nSignal Handler registered for memory exceptions\n");
/* Register an exit routine for normal exits */
   if (atexit(normalexit) == 0)
      printf("\nExit handler for normal termination registered\n");
/* Read parameters: 1. Memory to allocate    */
/*                  2. Memory to use         */
/* Both parameters as number of long integers*/
   printf("\nFor how many long integers should memory be allocated : ");
   scanf("%u", &ulAmount);
/* Determine number of bytes to allocate     */
   ulBytes = ulAmount * sizeof(ULONG);
   printf("\nHow long integers should be written into this memory : ");
   scanf("%u", &ulUse);
/* Allocate the memory                       */
   ulErr = DosAllocMem ( (PPVOID)&pulBlock, ulBytes,
                         PAG_COMMIT | PAG_READ | PAG_WRITE);
   if (!ulErr)
   {
/* Insert values into ulUse memory           */
      printf("\nInserting integers into memory\n");
      for (ulLoop = 0; ulLoop < ulUse; ulLoop++)
      {
         *(pulBlock + ulLoop) = '\xAB';
      } /* endfor */
/* Read the memory to check that it is OK    */
      for (ulLoop = 0; ulLoop < ulUse; ulLoop++)
      {
         if (*(pulBlock + ulLoop) != '\xAB')
         {
            printf("\nError in byte %u\n", ulLoop);
            OK = FALSE;
         } /* endif */
      } /* endfor */

      if (OK)
      {
         printf("\nAll memory checked out OK\n");
      } /* endif */

/* Free the memory                           */
      ulErr = DosFreeMem (pulBlock);
      if (ulErr != 0)
      {
        printf("\nError in freeing : code %u\n", ulErr);
      } /* endif */
   } else
   {
     printf("\nError in allocation : code %u\n", ulErr);
   } /* endif */

}
/* ABNORMAL TERMINATION HANDLER              */
void traphandler (int sig)
{
     printf("\nA General Protection Exception was detecting writing to"
            " position %u\n", ulLoop+1);
}
/* NORMAL TERMINATION ROUTINE                */
void normalexit(void)
{
     printf("\n%u integers successfully inserted into memory\n", ulLoop);
}
