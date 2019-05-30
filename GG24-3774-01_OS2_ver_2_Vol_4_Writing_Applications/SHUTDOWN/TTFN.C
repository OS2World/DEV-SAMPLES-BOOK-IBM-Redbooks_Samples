/*    Alternative shut down code                */
/*    Rough and ready                           */
/*    Tim Sennitt - ITSO Boca Raton             */
/*    November 1993                             */

#define INCL_WIN
#define INCL_DOS
#define INCL_GPIBITMAPS

#include <os2.h>
#include "TTFN.H"

MRESULT EXPENTRY TTFNWinProc1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY TTFNDlgProc3( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/* Global Variables */
HAB         hab;
ULONG       flCreate1;
HWND        hwnd1, hwndc1;
HBITMAP     hbit2, hbit3, hbitq;
RECTL       rectl2;
HMODULE     hModTTFN;
SHORT       ShutFlag;
LONG        cxScreen;
QMSG        qmsg;
HMQ         hmq;

main(argc,argv,envp)
int argc;
char *argv[];
char *envp[];
{
    HPS hps;                 /* Handle to Presentation Space */

    ShutFlag = 0;

    /* Intialize PM and obtain anchor block */
    hab = WinInitialize((USHORT) NULL);

    /* Create queue to receive messsages */
    hmq = WinCreateMsgQueue(hab,0);

    /* Register the window class "TimPmg" with procedure TTFNWinProc1 */
    WinRegisterClass(hab, "TimPmg", (PFNWP) TTFNWinProc1, CS_SIZEREDRAW, 0);

    flCreate1= FCF_TASKLIST;

    /* Create a standard window */
    hwnd1 = WinCreateStdWindow(HWND_DESKTOP,
                               WS_VISIBLE,
                               &flCreate1,
                               "TimPmg",
                               "TTFN",
                                0L,
                                hModTTFN,
                                ID_ONE,
                                (PHWND) & hwndc1);

    /* Get handle to presentation space */
    hps=WinGetPS(HWND_DESKTOP);

    /* Call GpiLoadBitMap to obtain a handle for the bitmap */
    hbit2 = GpiLoadBitmap(hps, hModTTFN, 1014, 32, 32);

    hbit3 = GpiLoadBitmap(hps, hModTTFN, 1015, 32, 32);

    /* Return handle to presentation space */
    WinReleasePS(hps);

    /* Initialize the dimensions of the bitmap     */
    /* and set size of window                      */
    if (WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN) == 1024)
    {
    rectl2.xRight   = 100;
    rectl2.yTop     = 100;
    } else {
    rectl2.xRight   = 50;
    rectl2.yTop     = 50;
    } /* endif */

    rectl2.xLeft    = 0;
    rectl2.yBottom  = 0;

    /* Set the window position */
    WinSetWindowPos(hwnd1, HWND_BOTTOM, 8, 8, rectl2.xRight, rectl2.xRight,
                    SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);

    /* Message Loop */
    while ( WinGetMsg(hab, (PQMSG) &qmsg, (HWND) NULL, 0, 0))
       WinDispatchMsg(hab, (PQMSG) &qmsg );

    /* Destroy the window */
    WinDestroyWindow(hwnd1);

    /* Destroy the message queue */
    WinDestroyMsgQueue( hmq );

    /* Terminate and release resources */
    WinTerminate( hab );

  return(0);
}
MRESULT EXPENTRY TTFNWinProc1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HPS   hps;               /* Handle to Presentation Space */
    RECTL rectl;             /* Dimensions of window         */
    SHORT rc;
    switch(msg)
    {
     case WM_BUTTON1DBLCLK:
       WinDlgBox (HWND_DESKTOP,
                       hwnd,
                       (PFNWP) TTFNDlgProc3,
                       hModTTFN,
                       ID_DLG,
                       NULL);
            break;
         case WM_USER:
            if (ShutFlag == 0) {
                    DosBeep(300,300);
                    DosBeep(600,300);
            } else {
                    DosBeep(600,300);
                    DosBeep(300,300);
            } /* endif */
              WinInvalidateRect(hwnd, NULL, TRUE);
              return (FALSE);
     case WM_PAINT:
            /*  Determine which BITMAP to Display */
            if (ShutFlag == 0)
            {
               hbitq = hbit2;
            } else {
               hbitq = hbit3;
            } /* endif */
      hps = WinBeginPaint( hwnd, (HPS) NULL, (PRECTL) &rectl );
            GpiErase(hps);

            /* Draw ON or OFF bitmap into the presentation space */
            WinDrawBitmap(hps, hbitq, NULL, (PPOINTL) &rectl2,
                          1L, -2L,
                          DBM_NORMAL | DBM_STRETCH);

            WinEndPaint( hps );
            /* Do we need to close down          */
            if (ShutFlag == 1)
            {
              WinSendMsg(hwnd, WM_CLOSE, mp1, mp2);
              rc = DosShutDown(0l);
              WinShutdownSystem(hab, hmq);
            }
            break;
        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
    }
    return(FALSE);
}
MRESULT EXPENTRY TTFNDlgProc3( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg)
    {
      case WM_COMMAND:
           switch (COMMANDMSG(&msg)->cmd)
                  {
               case PB_CN:
                      ShutFlag = 0;
                      WinDismissDlg (hwnd, TRUE);
                      WinSendMsg(hwnd1, WM_USER, 0L, 0L);
                    return (MRESULT)TRUE;
                 case PB_OK:
                     ShutFlag = 1;
                     WinDismissDlg (hwnd, TRUE);
                     WinSendMsg(hwnd1, WM_USER, 0L, 0L);
                   return (MRESULT)TRUE;
                  } /* endswitch */
           break;
      case WM_INITDLG:
              WinInvalidateRect(hwnd, NULL, TRUE);
           break;
      case WM_PAINT:
                return(WinDefDlgProc(hwnd, msg, mp1, mp2) );
                break;
            default:
                return(WinDefDlgProc(hwnd,msg,mp1,mp2));
        }
        return(FALSE);
}
