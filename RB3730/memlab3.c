/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: MEMLAB3.EXE                         ***/
/***                                                    ***/
/***  Created     : 7 May, 1990                         ***/
/***                                                    ***/
/***  Revised     : February, 1992                      ***/
/***                                                    ***/
/***  Author      : Bo Falkenberg                       ***/
/***                                                    ***/
/***  Purpose     : To demonstrate multiple DOS sessions***/
/***                started from a OS/2 program, and    ***/
/***                to show how the size of the swap    ***/
/***                file varies as the sessions are     ***/
/***                started and what happens to the swap***/
/***                file after the sessions are stopped.***/
/***                                                    ***/
/***  Compile     : icc /O+ /W2 memlab3.c               ***/
/***                                                    ***/
/***  Execute     : memlab3 n                           ***/
/***                where n = the number of DOS sessions***/
/***                to be started. If nothing is        ***/
/***                enterred 4 sessions will be started.***/
/***                                                    ***/
/***  File input  : Reads file MEMLAB3.PRO which must be***/
/***                setup using an editor such as the   ***/
/***                OS/2 System Editor. The file        ***/
/***                contains three lines. The first     ***/
/***                line contains the name of the swap  ***/
/***                file including its full path. The   ***/
/***                next line contains the name of the  ***/
/***                program which is to be executed in  ***/
/***                the DOS sessions. The path used to  ***/
/***                find the program must be included.  ***/
/***                The last line  contains the         ***/
/***                parameter string which is to be     ***/
/***                passed to the program on startup.   ***/
/***                                                    ***/
/**********************************************************/
/**********************************************************/

/**********************************************************/
/***  DEFINES                                           ***/
/**********************************************************/
 #define INCL_DOS
 #define LENGTH       sizeof(buffer)
                           /* DosFindFirst returned buffer
                              size */
 #define TIMEINTERVAL 10   /* Seconds to wait when checking
                              swap file size */
 #define MAXLOOP      10   /* No of intervals with same
                              swap file size after which
                              program in terminated */
 #define NOSESS       4    /* No of Sessions to start */

/**********************************************************/
/***  INCLUDE                                           ***/
/**********************************************************/
 #include        <os2.h>
 #include        <stdio.h>
 #include        <string.h>
 #include        <stdlib.h>

/**********************************************************/
/***  GLOBAL VARIABLES                                  ***/
/**********************************************************/
 ULONG            SessID;      /* Session ID (returned)   */
 PID              DOSpid;      /* Process ID (returned)   */
 USHORT           rc = 0;      /* return code             */
 USHORT           frc = 0;     /* file return code        */
 struct _STARTDATA StartData;  /* start program structure */
 struct _FILEFINDBUF buffer;   /* file information struct */
 char szFname[64];
 char szProgname[64];
 char szProginp[64];
 FILE *fptr;
 PULONG pStartedSessID;
 ULONG *p;

/**********************************************************/
/***  FUNCTION PROTOTYPES                               ***/
/**********************************************************/

void main(int argc, char *argv[], char *envp[]);
void printtrouble(void);
ULONG GetSwapperSize();

/**********************************************************/
/***  MAIN PROGRAM                                      ***/
/**********************************************************/

void main(int argc, char *argv[], char *envp[])
{
   int radix = 10;
   int loop;
   int no_of_DOS;
   char Related;
   unsigned char *Title1;
   unsigned char *Title2;
   char chloop1[30];
   char chloop2[30];
   char *dummy1;
   char *dummy2;
   char *pchrc;
   ULONG sLen;
   ULONG fsize;
   ULONG ulrc;
   ULONG ulTargetOption;
   ULONG ulSessid;
   ULONG ulTimeInterval = TIMEINTERVAL * 1000;
   ULONG elapsed;
   ULONG loopflag;
   ULONG timecount;
   ULONG samecount;
   ULONG savesize;

/* Default no of sessions to start                        */
   no_of_DOS = NOSESS;

/* Get arguments from the command line, if present        */
   if (argc >= 2)
   {
/* Number of sessions to start                            */
      no_of_DOS = atoi (argv[1]);
   }

/* Read parameters from MEMLAB3.PRO file                  */
   fptr = fopen("memlab3.pro", "r");
   if (fptr == (FILE *)NULL)
   {
      printf("\nFile MEMLAB3.PRO cannot be found\n");
      return;
   }
/* line 1 : swapper file path and filename                */
   pchrc = fgets(szFname, sizeof(szFname)-1, fptr);
   if (pchrc == (char *)NULL)
   {
      printtrouble();
      return;
   }
   szFname[strlen(szFname)-1] = '\0';
/* line 2 : name of program to start in the DOS sessions  */
   pchrc = fgets(szProgname, sizeof(szProgname)-1, fptr);
   if (pchrc == (char *)NULL)
   {
      printtrouble();
      return;
   }
   szProgname[strlen(szProgname)-1] = '\0';
/* line 3 : parameters to be passed to the program        */
   pchrc = fgets(szProginp, sizeof(szProginp)-1, fptr);
   if (pchrc == (char *)NULL)
   {
      printtrouble();
      return;
   }
   sLen = strlen(szProginp);
   szProginp[sLen-1] = '\0';

/* Set up parameter block for DosStartSession             */
   StartData.PgmName = szProgname;
   StartData.PgmInputs = szProginp;
   StartData.Length = 32;
   StartData.FgBg = 1;

   StartData.Related = 1; /* related to parent */

   StartData.TermQ = NULL;
   StartData.InheritOpt = 0;
   StartData.Environment = 0;
   loop = 0;
   rc = 0;

/* Allocate memory to save IDs of started sessions        */
   sLen = no_of_DOS * sizeof(ULONG);
   frc = DosAllocMem ((PPVOID)&pStartedSessID, sLen,
                      PAG_WRITE | PAG_READ | PAG_COMMIT );
   if (frc != 0)
   {
      printf("Memory Allocation Failure, return code %u\n",frc);
      exit (1);
   }
   p = pStartedSessID;

   /* Keep starting DOS sessions, until an error occurs   */
   /* or the requested number of DOS sessions is reached. */
   /* Save the IDs of the started sessions.               */
   /* Display the size of the swap file before starting   */
   /* any sessions and after each session is started.     */

   printf("Program MEMLAB3 is executing\n");
   printf("%u DOS sessions will be started\n", no_of_DOS);
   fsize = GetSwapperSize();
   printf("Size of SWAPPER.DAT is now %u Kb\n", fsize);
   while (!rc && loop < no_of_DOS)
   {
      loop++;
      StartData.SessionType = 4; /* Start a DOS session       */
                                 /* make the program title    */
      Title1 = ". DOS\n\0";      /* with a DOS session number */
      dummy1 = _itoa(loop,  chloop1, radix);
      strcat (dummy1, Title1);
      StartData.PgmTitle = dummy1;

      rc = DosStartSession(&StartData, &SessID, &DOSpid);
      if (rc == 0)
      {
         printf("DOS Session no %u is started; Session ID: %u\n",
                loop, SessID);
         fsize = GetSwapperSize();
         printf("Size of SWAPPER.DAT is now %u Kb\n", fsize);
         *p++ = SessID;
       } else
       {
         printf("An error occurred starting Dos Session no %u\n", loop);
         printf("Return code from DosStartSession = %u\n", frc);
         loop = no_of_DOS;
       } /* endif */
   } /* endwhile */

/* Wait for a key on the keyboard to be depressed, then   */
/* terminate the DOS sessions. Display the swap file size */
/* after each session is terminated.                      */
   printf("Press <Enter> to terminate the DOS Sessions...");
   fflush(stdout);
   loop = getchar();  /*wait for input */
   p = pStartedSessID;
   for (loop = 1; loop <= no_of_DOS; loop++)
   {
      ulSessid = *p++;
      ulTargetOption = 0;
      ulrc = DosStopSession (ulTargetOption, ulSessid);
      if (ulrc == 0)
      {
         printf("Session with ID %u has been stopped\n", ulSessid);

         fsize = GetSwapperSize();
         printf("Size of SWAPPER.DAT is now %u Kb\n", fsize);
      }
   }

/* Monitor the swap file size and display it at intervals */
/* of TIMEINTERVAL seconds. When the size has remained    */
/* constant for TIMEINTERVAL * MAXLOOP seconds, terminate */
/* the program.                                           */
   loopflag = TRUE;
   savesize = fsize;
   timecount = 1;
   while (loopflag)
   {
      ulrc = DosSleep(ulTimeInterval);
      elapsed = timecount * TIMEINTERVAL;
      printf("Elapsed time since closing DOS sessions is %u seconds\n",
             elapsed);
      fsize = GetSwapperSize();
      printf("Size of SWAPPER.DAT is now %u Kb\n", fsize);
      timecount++;
      samecount++;
      if ( fsize != savesize)
      {
         savesize = fsize;
         samecount = 0;
      }
      if (samecount == MAXLOOP - 1)
      {
         elapsed = MAXLOOP * TIMEINTERVAL;
         printf("No change in SWAPPER.DAT size for %u seconds\n"
                "Program in terminating\n",elapsed);
         loopflag = FALSE;
      }

   }
   exit(0);
}

/* Function to report errors with MEMLAB3.PRO             */
void printtrouble(void)
{
   printf("\nSorry, trouble reading memlab3.pro\n");
   fclose(fptr);
   return;
}

/* Function which returns swap file size in Kb            */
ULONG GetSwapperSize ()
{
   HDIR fhandle;
   unsigned LONG count;
   int fsize;

   count = 1;
   fhandle = 0xFFFF;
   frc = DosFindFirst (szFname, &fhandle, 0, &buffer, LENGTH,
                       &count, 1L);
   if (frc != 0)
   {
      fflush(stdout);
      printf("File error :%u\n", frc);
      exit(0);
   } /* endif */
   fsize = buffer.cbFileAlloc / 1024;  /* in Kbytes */
   DosFindClose (fhandle);
   return(fsize);
}
