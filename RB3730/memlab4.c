/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: MEMLAB4.EXE                         ***/
/***                                                    ***/
/***  Created     : 7 May, 1990                         ***/
/***                                                    ***/
/***  Author      : Bo Falkenberg                       ***/
/***                                                    ***/
/***  Revised     : February, 1992 by Darryl Frost      ***/
/***                                                    ***/
/***  Purpose     : To demonstrate the new system       ***/
/***                limit for the number of threads     ***/
/***                per process in OS/2 2.0 and the     ***/
/***                effect of thread creation on the    ***/
/***                growth of SWAPPER.DAT.              ***/
/***                                                    ***/
/***  Compile     : icc /W2 /Gm+ memlab4.c              ***/
/***                                                    ***/
/***  Execute     : memlab4 n f                         ***/
/***                where n is the number of secondary  ***/
/***                threads the program must create,    ***/
/***                and f if present and 0 (zero) causes***/
/***                the created threads to terminate    ***/
/***                after 40 seconds else the threads   ***/
/***                after 40 seconds enter a wait and   ***/
/***                print loop. The wait time is        ***/
/***                randomly determined. If n is not    ***/
/***                specified 10 is assumed.
/***                                                    ***/
/**********************************************************/
/**********************************************************/



/**********************************************************/
/***  DEFINES                                           ***/
/**********************************************************/
#define INCL_DOS
#define INCL_DOSPROCESS


/**********************************************************/
/***  INCLUDE                                           ***/
/**********************************************************/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

void _Optlink NewThread( PVOID pThreadArg ); /* procedure declaration */

BOOL loopflag = TRUE;
int threadcount = 0;

/**********************************************************/
/***  MAIN PROGRAM                                      ***/
/**********************************************************/
main( int argc, char *argv[], char *envp[] )
{                                         /*************************/
  TID       ThreadID;                     /* thread identification */
  ULONG     ulThreadArg;                  /* thread arguments      */
  ULONG     ulThreadFlags;                /* thread flags          */
  ULONG     ulStack_size;                 /* thread stack size     */
  int       no_of_threads;                /* number of threads     */
  int       i;                            /* loop variable         */
                                          /*************************/

  ulThreadFlags = 0;          /* start thread immediately   */
  ulStack_size  = 1024;       /* give stack size in bytes   */
  ulThreadArg   = 1;

   if (argc < 2)
      {
         no_of_threads = 10;
      } else
      {
         no_of_threads    = atoi(argv[1]);
      }
  if (argc > 2)
  {
     if (*argv[2]=='0')
        loopflag = FALSE;
  }

  for (i = 1; i < no_of_threads+1; i++)
  {
    if ( ( ThreadID = _beginthread( NewThread, NULL, ulStack_size, (PVOID)i ) ) == -1 )
    {
      printf("_beginthread error\n");
      exit (1);
    }
    printf("Thread number %d created\n",i);
  }

  printf("To end the program press <CR> \n");
  getchar();
  loopflag = FALSE;
  printf("MEMLAB4 terminating\n");
/* Wait for all the threads to stop */
  while (TRUE)
  {
     DosSleep(1000);
     DosEnterCritSec();
     if(threadcount == 0){
        DosExitCritSec();
        printf("All threads stopped, program is terminated\n");
        exit (1);
     }
     DosExitCritSec();
  }
}

/**********************************************************/
/***  THREAD                                            ***/
/**********************************************************/
void NewThread( PVOID pThreadArg )
   {
   ULONG ulThreadArg = ( ULONG )pThreadArg;

   DosEnterCritSec();
   threadcount++;
   DosExitCritSec();

   printf( "Thread %u has started\n", ulThreadArg );
   srand( (int)ulThreadArg); /* seed random generator */
   DosSleep ( 40000 );       /* 40 SEC. sleep interval */

   while (loopflag)
      {
      printf("Thread %d just woke up\n", ulThreadArg);
      DosSleep ( rand() ); /* random sleep interval */
      }

   DosEnterCritSec();
   threadcount--;
   DosExitCritSec();
   }
