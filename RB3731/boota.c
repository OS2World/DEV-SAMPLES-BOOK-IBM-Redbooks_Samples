/*
 *  BOOTA:  A simple program to start a DOS Boot session under OS/2 2.0.
 *          This program can be run from an OS/2 command prompt and it
 *          then start to Boot DOS from the A: drive.
 *
 *  Last Modfied: 04/02/92
 *
 *  Author: Stacey Barnes
 *  Modified: Jeff Muir
 */

#define INCL_DOSSESMGR
#define INCL_DOSMISC
#include <os2.h>

/* messages used by BOOTA */
PSZ pBootAMsg = "BOOTA: Booting DOS from A: Drive.\r\n";
PSZ pBootSuccess = "Session started.\r\n";
PSZ pBootFailure = "Session could not be started.\r\n";

STARTDATA startd;                  /* Session start information */
USHORT    SessionID, ProcessID;    /* Session and Process ID for new session*/

void main(void)
{
  USHORT       rc;

  /* Print header message */
  DosPutMessage(1,strlen(pBootAMsg),pBootAMsg);

  /* Init fields to Boot from A: drive */
  startd.Length                   = sizeof(STARTDATA);
  startd.Related                  = SSF_RELATED_INDEPENDENT;
  startd.FgBg                     = SSF_FGBG_FORE;
  startd.TraceOpt                 = SSF_TRACEOPT_NONE;
  startd.PgmTitle                 = "Boot A: Drive";
  startd.PgmName                  = NULL;
  startd.PgmInputs                = NULL;
  startd.TermQ                    = NULL;
  startd.Environment              = "DOS_STARTUP_DRIVE=A:\0";
  startd.InheritOpt               = SSF_INHERTOPT_PARENT;
  startd.SessionType              = SSF_TYPE_VDM;

  /* Start the DOS Boot Session */
  rc = DosStartSession( &startd, &SessionID, &ProcessID );

  /* Print out either Success or Failure message */
  if(rc)
    DosPutMessage(1,strlen(pBootFailure),pBootFailure);
  else
    DosPutMessage(1,strlen(pBootSuccess),pBootSuccess);

return;
}

