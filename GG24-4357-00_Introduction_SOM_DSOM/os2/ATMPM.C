/* ******************************************************************** *
 * Copyright International Business Machines Corporation 1993, 1994     *
 * DISCLAIMER OF WARRANTIES.                                            *
 * The following [enclosed] code is sample code created by IBM          *
 * Corporation. This sample code is not part of any standard or IBM     *
 * product and is provided to you solely for the purpose of assisting   *
 * you in the development of your applications.  The code is provided   *
 * "AS IS". IBM MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT  *
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS *
 * FOR A PARTICULAR PURPOSE, REGARDING THE FUNCTION OR PERFORMANCE OF   *
 * THIS CODE.  IBM shall not be liable for any damages arising out of   *
 * your use of the sample code, even if they have been advised of the   *
 * possibility of such damages.                                         *
 *                                                                      *
 * DISTRIBUTION.                                                        *
 * This sample code can be freely distributed, copied, altered, and     *
 * incorporated into other software, provided that it bears the above   *
 * Copyright notice and DISCLAIMER intact.                              *
 * ----------------------------------------------------------------------
 * SOMBank Sample Application                                           *
 *                                                                      *
 * Documented in:                                                       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *
 *  "SOMobjects: A Practical Introduction to SOM and DSOM" (GG24-4357)  *
 * ----------------------------------------------------------------------
 *                                                                      *
 *    PM version of ATM Client                                          *
 *                                                                      *
 * ******************************************************************** *
 */
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <os2.h>                       /* System Include File      */
#ifndef NOSOM
 #include <som.h>
 #include <somd.h>
 #include "bankdef.h"
 #include "clientcl.h"
#endif
#include "atmpm.h"                     /* Application Include File */

MRESULT EXPENTRY FrameWndProc(HWND hwnd,ULONG msg,MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY SOMBANKWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Animate( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
int FontMetricsFromFont(PCHAR szFontNameSize, PFONTMETRICS pfm);
VOID CreateKeyPad(HWND hwnd);
VOID KeyPadEnable(HWND hwnd, BOOL state);
VOID PressButton(HWND hwnd, ULONG id);
VOID Sequencer(HWND hwnd);
#ifdef NOSOM
LONG *somGetGlobalEnvironment();
LONG ClientClassNew();
VOID _somFree(LONG clClass);
BOOL _restoreATMCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode,ULONG ulPin);
LONG __get_CheckingAccountID(LONG clClass,LONG *e);
LONG __get_SavingsAccountID(LONG clClass,LONG *e);
VOID _inquire (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double *dAmount);
VOID _withdraw(LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double dAmount);
VOID _deposit (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double dAmount);
#endif


/* Global Variables */
enum { INIT=1, ACCOUNT, PIN, CUST_VALID, ANOTHERT, ACCOUNTRANSACTYPE, TRANSACTYPE, DOTRANSACTION, TRANSACDONE, ENDTRANSACTION} phase = INIT;
enum THREADENUM { CUSTOMER=1, INQCHECK, INQSAV, WITHDCHECK, WITHDSAV, DEPCHECK, DEPSAV, END} threadphase;

PCHAR pszTitle = "SOMBank ATM Application";
CHAR   Buffer[20];
HAB hab;
ULONG flCreate1;
HWND hwndFrame;
HWND hwndClient;
HMODULE hModSOMBANK;

#define SAVINGS         2
#define CHECKING        3
#define USR_THREAD      WM_USER
#define USR_LIGHT       WM_USER+1

#ifdef NOSOM
 LONG         clClass;
 LONG         *e;
 double       dSav, dChe;
#else
 ClientClass  clClass;
 Environment  *e;
#endif


VOID KickThread(enum THREADENUM  threadf);
VOID _Optlink SOMCalls();

BOOL    bThreadRunning;
BOOL    CustomerFound;
ULONG   ulPin;
ULONG   ulCustomerCode;
LONG    cAccountID;
LONG    sAccountID;
double  dAmount;
HEV     hevSem;
HEV     hevSemEnd;
PFNWP   OldpFrame;

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
main(int argc,char *argv[],char *envp[])
{
    QMSG qmsg;               /* Message Queue                */
    HMQ hmq;                 /* Handle to Queue              */
    HPS hps;                 /* Handle to Presentation Space */
    

    DosCreateEventSem(NULL,&hevSem, 0, FALSE);
    DosCreateEventSem(NULL,&hevSemEnd, 0, FALSE);

    /* Intialize PM and obtain anchor block */
    hab = WinInitialize((USHORT) NULL);
    
    /* Create queue to receive messsages */
    hmq = WinCreateMsgQueue(hab,0);
    
    /* Register the window class "SOMBANK_PM_Class" with procedure SOMBANKWinProc */
    WinRegisterClass(hab,
                     "SOMBANK_PM_Class",
                     (PFNWP) SOMBANKWinProc,
                     CS_SIZEREDRAW,
                     0);
    
    flCreate1= FCF_BORDER | FCF_MINBUTTON | FCF_SYSMENU | FCF_TASKLIST | FCF_TITLEBAR | FCF_ICON;
    
    /* Create a standard window */
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                        WS_VISIBLE,
                                       &flCreate1,
                                       "SOMBANK_PM_Class",
                                       pszTitle,
                                       0L,
                                       hModSOMBANK,
                                       ID_MAIN_WINDOW,
                                       (PHWND) & hwndClient);

    OldpFrame = WinSubclassWindow(hwndFrame, (PFNWP)FrameWndProc);
//    WinSendMsg(hwndFrame, WM_UPDATEFRAME, MPFROMSHORT(FCF_VERTSCROLL), mpNULL);
    
    /* Set the window position */
    WinSetWindowPos(hwndFrame,
                    HWND_BOTTOM,
                    (WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN)-615)/2,
                    (WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)-455)/2,
                    615,
                    455,
                    SWP_MOVE | SWP_SIZE | SWP_ACTIVATE);
    
    /* Message Loop */
    while ( WinGetMsg(hab, (PQMSG) &qmsg, (HWND) NULL, 0, 0))
       WinDispatchMsg(hab, (PQMSG) &qmsg );
    
    /* Destroy the window */
    WinDestroyWindow(hwndFrame);
    
    /* Destroy the message queue */
    WinDestroyMsgQueue( hmq );
    
    /* Terminate and release resources */
    WinTerminate( hab );

    DosCloseEventSem(hevSem);
    DosCloseEventSem(hevSemEnd);
  return(0);
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
MRESULT EXPENTRY FrameWndProc(HWND hwnd,ULONG msg,MPARAM mp1, MPARAM mp2)
{
   LONG i;
   LONG NumberOfSWPs;
   PSWP pswpTitleBar, aswp;
   POINTL ptl;
   static LONG Light_cx, Light_cy, Light_x, Light_y;
   HPS  hps;
//   ARCPARAMS arcp = { 1, 1, 0, 0};

   switch(msg) {

     case WM_FORMATFRAME: 
        NumberOfSWPs = (LONG) OldpFrame(hwnd, msg, mp1, mp2);
        aswp = mp1;
        for (i=0; i<NumberOfSWPs; i++ ) {
           if (WinQueryWindowUShort(aswp[i].hwnd, QWS_ID) == FID_TITLEBAR) {
              pswpTitleBar = &aswp[i];
              break;
           } /* endif */
        } /* endfor */
        pswpTitleBar->cx -= pswpTitleBar->cy;
        Light_cx = pswpTitleBar->cy;
        Light_cy = pswpTitleBar->cy;
        Light_x = pswpTitleBar->x + pswpTitleBar->cx;
        Light_y = pswpTitleBar->y;
        return (MRESULT) NumberOfSWPs;

     case WM_PAINT:
        OldpFrame(hwnd, msg, mp1, mp2);
        hps = WinGetPS (hwnd);
        GpiSetColor(hps,SYSCLR_BUTTONDARK  );
        ptl.x = Light_x;
        ptl.y = Light_y; 
        GpiMove(hps, &ptl);
        ptl.x = Light_x + Light_cx;
        ptl.y = Light_y + Light_cy; 
        GpiBox(hps,DRO_FILL, &ptl , 1L, 1L);
        GpiSetColor(hps,SYSCLR_BUTTONMIDDLE);
        ptl.x = Light_x+1;
        ptl.y = Light_y+1; 
        GpiMove(hps, &ptl);
        ptl.x = Light_x + Light_cx -1;
        ptl.y = Light_y + Light_cy -1; 
        GpiBox(hps,DRO_FILL, &ptl , 1L, 1L);
//        GpiSetArcParams(hps,&arcp);
        if (bThreadRunning) {
           GpiSetColor(hps,CLR_RED);
        } else {
           GpiSetColor(hps,CLR_GREEN);
        } /* endif */
        ptl.y = Light_y+ Light_cy/2; 
        ptl.x = Light_x+ Light_cx/2 ;
        GpiMove(hps, &ptl);
        GpiFullArc(hps, DRO_FILL, MAKEFIXED(Light_cx/3,0));
        WinReleasePS (hps);
        return (MRESULT) 0;

     case USR_LIGHT:
        hps = WinGetPS (hwnd);
        if (bThreadRunning) {
           GpiSetColor(hps,CLR_RED);
        } else {
           GpiSetColor(hps,CLR_GREEN);
        } /* endif */
        ptl.y = Light_y+ Light_cy/2; 
        ptl.x = Light_x+ Light_cx/2 ;
        GpiMove(hps, &ptl);
        GpiFullArc(hps, DRO_FILL, MAKEFIXED(Light_cx/3,0));
        WinReleasePS (hps);
        return (MRESULT) 0;

     default:
        return OldpFrame(hwnd, msg, mp1, mp2);
   }
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
MRESULT EXPENTRY SOMBANKWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    LONG            lPresParam;
    HPS             hps;                 /* Handle to Presentation Space */
    RECTL           rectl;             /* Dimensions of window         */
    PSZ             sKey = "0";
    HWND            hwndSysMenu;
    HWND            hwndSysSubMenu ;
    SHORT           idSysMenu, i;
    static BOOL     bDotPressed;
    static CHAR     cStars[10];
    static SHORT    sNumChars = 0;
    MENUITEM        miSysMenu ;
    static CHAR     *szMenuText [5] = { NULL,
                                        "~Product Information" } ;
    static MENUITEM mi [2] = {
                             MIT_END, MIS_SEPARATOR, 0, 0, 0L, 0L,
                             MIT_END, MIS_TEXT,      0, ID_DLG_ABOUT, 0L, 0L
                             } ;

    switch(msg)
    {
        case WM_COMMAND:
             switch (SHORT1FROMMP(mp1))
             {
                 case PB_KB_1:
                 case PB_KB_2:
                 case PB_KB_3:
                 case PB_KB_4:
                 case PB_KB_5:
                 case PB_KB_6:
                 case PB_KB_7:
                 case PB_KB_8:
                 case PB_KB_9:
                 case PB_KB_0:
                      if (WinIsWindowShowing(WinWindowFromID(hwnd, STX_MSG_3 ))) {
                         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  FALSE );
                      } /* endif */
                      if ((phase == PIN) || (phase == ACCOUNT)) {
                         if (++sNumChars > 4) {
                            WinAlarm(HWND_DESKTOP, WA_ERROR);
                            break;
                         } /* endif */
                      } /* endif */
                      if (bDotPressed && (++sNumChars > 2)) {
                          WinAlarm(HWND_DESKTOP, WA_ERROR);
                          break;
                      } /* endif */
                      *sKey = (SHORT1FROMMP(mp1) - PB_KB_0) | 0x30;
                      strcat(Buffer,sKey);
                      if (phase == PIN) {
                        strcat(cStars,"*");
                        WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),cStars);
                      } else {
                        WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),Buffer);
                      } /* endif */
                      break;
                 
                 case PB_KB_DOT:
                      if (!bDotPressed) {
                         strcat(Buffer,".");
                         WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),Buffer);
                         bDotPressed = TRUE;
                         sNumChars = 0;   
                      } else {
                         WinAlarm(HWND_DESKTOP, WA_ERROR);
                      } /* endif */
                      break;
                 
                 case PB_KB_OK:
                      if (phase == ENDTRANSACTION) {
                         phase = ANOTHERT;
                         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  FALSE );
                      } /* endif */
                      bDotPressed = FALSE;
                      *cStars = 0;
                      sNumChars = 0;   
                      Sequencer(hwnd);
                      break;
                 
                 case PB_KB_CLR:
                      bDotPressed = FALSE;
                      *Buffer = 0;  
                      *cStars = 0;
                      sNumChars = 0;   
                      WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),Buffer);
                      break;
                 
                 case PB_KB_CANCEL:
                      if (phase == DOTRANSACTION) {
                         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  FALSE );
                      } /* endif */
                      bDotPressed = FALSE;
                      phase = INIT;  
                      sNumChars = 0;   
                      Sequencer(hwnd);
                      break;
                 
                  case ID_DLG_ABOUT:
                       WinDlgBox ( (HWND) HWND_DESKTOP, (HWND) hwnd, (PFNWP) AboutDlgProc,
                                    0L, ID_DLG_ABOUT, 0L) ;
                       break;
             }
             break;
                 
        case WM_CONTROL:
             switch (SHORT1FROMMP(mp1))
             {
             case RB_SAVINGS:
                  WinCheckButton (hwnd, RB_CHECKING,  FALSE);
                  WinCheckButton (hwnd, RB_SAVINGS,   TRUE );
                  break;
             case RB_CHECKING:
                  WinCheckButton (hwnd, RB_CHECKING,  TRUE );
                  WinCheckButton (hwnd, RB_SAVINGS,   FALSE);
                  break;
             case RB_INQUIRE:
                  WinCheckButton (hwnd, RB_INQUIRE,  TRUE );
                  WinCheckButton (hwnd, RB_DEPOSIT,  FALSE);
                  WinCheckButton (hwnd, RB_WITHDRAW, FALSE);
                  break;
             case RB_WITHDRAW:
                  WinCheckButton (hwnd, RB_INQUIRE,  FALSE);
                  WinCheckButton (hwnd, RB_DEPOSIT,  FALSE);
                  WinCheckButton (hwnd, RB_WITHDRAW, TRUE );
                  break;
             case RB_DEPOSIT:
                  WinCheckButton (hwnd, RB_INQUIRE,  FALSE);
                  WinCheckButton (hwnd, RB_DEPOSIT,  TRUE );
                  WinCheckButton (hwnd, RB_WITHDRAW, FALSE);
                  break;
             }
             break;
                
        case WM_CREATE:
             
             hwndSysMenu = WinWindowFromID (
                                WinQueryWindow (hwnd, QW_PARENT),
                                FID_SYSMENU) ;

             WinSendMsg (hwndSysMenu, MM_DELETEITEM,
                          MPFROM2SHORT (SC_MAXIMIZE, TRUE), 0L);
             WinSendMsg (hwndSysMenu, MM_DELETEITEM,
                          MPFROM2SHORT (SC_SIZE, TRUE), 0L);

             idSysMenu = SHORT1FROMMR (WinSendMsg (hwndSysMenu,
                                                   MM_ITEMIDFROMPOSITION,
                                                   NULL, NULL)) ;
             WinSendMsg (hwndSysMenu, MM_QUERYITEM,
                         MPFROM2SHORT (idSysMenu, FALSE),
                         MPFROMP (&miSysMenu)) ;

             hwndSysSubMenu = miSysMenu.hwndSubMenu ;

             for (i = 0 ; i <2 ; i++)
                  WinSendMsg (hwndSysSubMenu, MM_INSERTITEM,
                              MPFROMP (mi + i),
                              MPFROMP (szMenuText [i])) ;

             CreateKeyPad(hwnd);
             
             /* Create the WC_BUTTON, "Savings"  */
             WinCreateWindow(hwnd,
                             WC_BUTTON,
                             "",
                             BS_RADIOBUTTON | BS_NOPOINTERFOCUS,
                             580, 155, 20, 20,
                             hwnd, HWND_TOP, RB_SAVINGS, 0L, NULL);
             WinCheckButton (hwnd, RB_SAVINGS, TRUE);
             
             /* Create the WC_BUTTON, "Checking"  */
             WinCreateWindow(hwnd,
                             WC_BUTTON,
                             "",
                             BS_RADIOBUTTON | BS_NOPOINTERFOCUS,
                             580, 125, 20, 20,
                             hwnd, HWND_TOP, RB_CHECKING, 0L, NULL);
             
             /* Create the WC_BUTTON, "Inquire"  */
             WinCreateWindow(hwnd,
                             WC_BUTTON,
                             "",
                             BS_RADIOBUTTON | BS_NOPOINTERFOCUS,
                             580, 155, 20, 20,
                             hwnd, HWND_TOP, RB_INQUIRE, 0L, NULL);
             WinCheckButton (hwnd, RB_INQUIRE, TRUE);
             
             /* Create the WC_BUTTON, "Withdraw"  */
             WinCreateWindow(hwnd,
                             WC_BUTTON,
                             "",
                             BS_RADIOBUTTON | BS_NOPOINTERFOCUS,
                             580, 125, 20, 20,
                             hwnd, HWND_TOP, RB_WITHDRAW, 0L, NULL);
             
             /* Create the WC_BUTTON, "Deposit"  */
             WinCreateWindow(hwnd,
                             WC_BUTTON,
                             "",
                             BS_RADIOBUTTON | BS_NOPOINTERFOCUS,
                             580,  95, 20, 20,
                             hwnd, HWND_TOP, RB_DEPOSIT, 0L, NULL);
             
             /* Create the WC_STATIC, "Message 1"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "Initializing Please Wait",
                             SS_TEXT | WS_VISIBLE | DT_CENTER | DT_VCENTER,
                             250, 245, 280, 30,
                             hwnd, HWND_TOP, STX_MSG_1, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_MSG_1),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);
             
             /* Create the WC_STATIC, "Message 2"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "",
                             SS_TEXT | WS_VISIBLE | DT_CENTER | DT_VCENTER,
                             250, 215, 280, 30,
                             hwnd, HWND_TOP, STX_MSG_2, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_MSG_2),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);

             /* Create the WC_STATIC, "Message 3"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "",
                             SS_TEXT | DT_CENTER | DT_VCENTER,
                             250,  50, 280, 30,
                             hwnd, HWND_TOP, STX_MSG_3, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_MSG_3),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);
            lPresParam = CLR_RED  /* CLR_RED */;
            WinSetPresParam(WinWindowFromID(hwnd, STX_MSG_3),
                            PP_FOREGROUNDCOLORINDEX,
                            (ULONG) sizeof(LONG),
                            (PVOID) &lPresParam);

             /* Create the WC_STATIC, "Selection 1"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "",
                             SS_TEXT | DT_RIGHT  | DT_VCENTER,
                             470, 150, 80, 30,
                             hwnd, HWND_TOP, STX_SEL_1, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_SEL_1),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);

             /* Create the WC_STATIC, "Selection 2"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "",
                             SS_TEXT | DT_RIGHT  | DT_VCENTER,
                             470, 120, 80, 30,
                             hwnd, HWND_TOP, STX_SEL_2, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_SEL_2),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);

             /* Create the WC_STATIC, "Selection 3"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "Deposit",
                             SS_TEXT | DT_RIGHT  | DT_VCENTER,
                             470,  90, 80, 30,
                             hwnd, HWND_TOP, STX_SEL_3, 0L, NULL);
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_SEL_3),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);
             
             /* Create the WC_STATIC, "ATM Display"  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "ATM Display",
                             SS_GROUPBOX | WS_VISIBLE,
                             225,  35, 350, 280, hwnd,
                             HWND_TOP, GB_DISPLAY, 0L, NULL);
             
             /* Create the WC_STATIC, ""  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "",
                             SS_TEXT | WS_VISIBLE | DT_CENTER | DT_VCENTER,
                             340, 125, 120, 30,
                             hwnd, HWND_TOP, STX_ENTRY, 0L, NULL);
             
//             lPresParam = 16777215 /* CLR_ */;
             lPresParam = 0x00AADBAAL /* CLR_ */;
             WinSetPresParam(WinWindowFromID(hwnd, STX_ENTRY),
                             PP_BACKGROUNDCOLOR,
                             (ULONG) sizeof(LONG),
                             (PVOID) &lPresParam);
             WinSetPresParam(WinWindowFromID(hwnd, STX_ENTRY),
                             PP_FONTNAMESIZE,
                             (ULONG) strlen("18.System VIO")+1,
                             (PVOID) "18.System VIO");
             
             /* Create the WC_STATIC, ICON  */
             WinCreateWindow(hwnd,
                             WC_STATIC,
                             "#10",
                             SS_ICON | WS_VISIBLE,
                             240, 340, 30, 30,
                             hwnd, HWND_TOP, IC_SOM, 0L, NULL);


            lPresParam = 13421772 /* CLR_ */;
//            lPresParam = 0x003F3F3FL /* CLR_ */;
            WinSetPresParam(hwnd,
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(LONG),
                            (PVOID) &lPresParam);
            Animate(hwnd,  msg,  mp1, mp2); 
            bThreadRunning = TRUE;
            WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
            _beginthread(SOMCalls, NULL, 32768, NULL);
            break;
                
        case USR_THREAD:
            WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));
            WinSendMsg(hwndFrame,USR_LIGHT,(MPARAM) 0, (MPARAM) 0);
            Sequencer(hwnd);
            break;
                
        case WM_FOCUSCHANGE:
            Animate(hwnd,  msg,  mp1, mp2); 
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
                
        case WM_TIMER:
            Animate(hwnd,  msg,  mp1, mp2); 
            break;
                
        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefWindowProc(hwnd,msg,mp1,mp2));
                
        case WM_PAINT:
            
            /* Default code for WM_PAINT */
            {
            ULONG  clr,attrfound;
            LONG ColorTable;
            POINTL ptl;
            SWP   swp;
            
            ColorTable = 0 ;
            
            hps = WinBeginPaint( hwnd,
                                (HPS) NULL,
                                (PRECTL) &rectl );
           clr=SYSCLR_WINDOW;  /* default colour for client */
            
                                /* get background pres param if exists */
            WinQueryPresParam(hwnd,PP_BACKGROUNDCOLORINDEX,PP_BACKGROUNDCOLOR,&attrfound,sizeof(LONG),
                          &clr,QPF_ID1COLORINDEX);
            
                                    /* Change color table to accept RGB values */
            GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,&ColorTable) ;
            
            WinFillRect( hps,       /* paint the rectangle */
                         (PRECTL) &rectl,
                         clr);
            
            
            WinQueryWindowPos(WinWindowFromID(hwnd, GB_DISPLAY) ,&swp);
            GpiSetColor(hps,SYSCLR_BUTTONLIGHT);
            ptl.x = swp.x + 10;
            ptl.y = swp.y + 10; 
            GpiMove(hps, &ptl);
            ptl.x = swp.x+swp.cx - 12;  
            ptl.y = swp.y+swp.cy - 32;  
            GpiBox(hps,DRO_FILL, &ptl , 1L, 1L);
            GpiSetColor(hps,SYSCLR_BUTTONDARK  );
            ptl.x = swp.x + 12;
            ptl.y = swp.y + 12;
            GpiMove(hps, &ptl);
            ptl.x = swp.x+swp.cx - 10;  
            ptl.y = swp.y+swp.cy - 30;  
            GpiBox(hps,DRO_FILL, &ptl , 1L, 1L);
            GpiSetColor(hps,0x00AADBAAL  );
            ptl.x = swp.x + 12;
            ptl.y = swp.y + 12; 
            GpiMove(hps, &ptl);
            ptl.x = swp.x+swp.cx - 12;  
            ptl.y = swp.y+swp.cy - 32;  
            GpiBox(hps,DRO_FILL, &ptl , 1L, 1L);
            WinEndPaint( hps );
            }
            Animate(hwnd,  msg,  mp1, mp2); 
            break;
                
        case WM_CHAR:
            if ( CHARMSG (&msg)->fs & KC_KEYUP )
               return (MPARAM) TRUE;

            if ( (CHARMSG (&msg)->fs & KC_VIRTUALKEY) &&  !(CHARMSG (&msg)->fs & KC_CHAR)) {
               switch ( CHARMSG(&msg)->vkey ) {

               case VK_TAB:
                  if (WinIsWindowShowing(WinWindowFromID(hwnd, RB_SAVINGS ))) {
                     WinSendDlgItemMsg(hwnd,
                                       (WinSendDlgItemMsg(hwnd,RB_SAVINGS,BM_QUERYCHECK,0,0)) ? RB_CHECKING : RB_SAVINGS ,
                                       BM_CLICK, 0, 0);
                  } else {
                     if (WinIsWindowShowing(WinWindowFromID(hwnd, RB_INQUIRE ))) {
                        if (WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_QUERYCHECK,0,0)) {
                           WinSendDlgItemMsg(hwnd,RB_WITHDRAW,BM_CLICK,0,0);
                        } else {
                           if (WinSendDlgItemMsg(hwnd,RB_WITHDRAW,BM_QUERYCHECK,0,0)) {
                              WinSendDlgItemMsg(hwnd,RB_DEPOSIT,BM_CLICK,0,0);
                           } else {
                              WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_CLICK,0,0);
                           } /* endif */
                        } /* endif */
                     } /* endif */
                  } /* endif */
                  break;

               case VK_DELETE:
                  if (WinIsWindowEnabled(WinWindowFromID(hwnd, PB_KB_CLR))) 
                     PressButton(hwnd, PB_KB_CLR);
                  break;

               case VK_ESC:
                  if (WinIsWindowEnabled(WinWindowFromID(hwnd, PB_KB_CANCEL))) 
                     PressButton(hwnd, PB_KB_CANCEL);
                  break;

               case VK_ENTER:
               case VK_NEWLINE:
                  if (WinIsWindowEnabled(WinWindowFromID(hwnd, PB_KB_OK))) 
                     PressButton(hwnd, PB_KB_OK);
                  break;

               default:
                 break;
               } /* endswitch */

            } else {

               if ( CHARMSG (&msg)->fs & KC_CHAR ) {
                  if ((CHARMSG (&msg)->chr >= '0')&&(CHARMSG (&msg)->chr <= '9')) {
                     if (WinIsWindowEnabled(WinWindowFromID(hwnd,PB_KB_0+(CHARMSG (&msg)->chr & 0x0F) ))) {
                        PressButton(hwnd, PB_KB_0+(CHARMSG (&msg)->chr & 0x0F));
                     } /* endif */
                  } else {
                     if ( (CHARMSG (&msg)->chr == '.') && ( WinIsWindowEnabled(WinWindowFromID(hwnd, PB_KB_DOT)) ) ) {
                        PressButton(hwnd, PB_KB_DOT);
                     } else {
                         if ( (CHARMSG (&msg)->chr == 0x0D) || (CHARMSG (&msg)->chr == 0x0A) ) {
                            if ( WinIsWindowEnabled(WinWindowFromID(hwnd, PB_KB_OK)) ) 
                               PressButton(hwnd, PB_KB_OK);
                         } else {
                            if ( (CHARMSG (&msg)->chr == '\t') ) {
                               if (WinIsWindowShowing(WinWindowFromID(hwnd, RB_SAVINGS ))) {
                                  WinSendDlgItemMsg(hwnd,
                                                    (WinSendDlgItemMsg(hwnd,RB_SAVINGS,BM_QUERYCHECK,0,0)) ? RB_CHECKING : RB_SAVINGS ,
                                                    BM_CLICK, 0, 0);
                               } else {
                                  if (WinIsWindowShowing(WinWindowFromID(hwnd, RB_INQUIRE ))) {
                                     if (WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_QUERYCHECK,0,0)) {
                                        WinSendDlgItemMsg(hwnd,RB_WITHDRAW,BM_CLICK,0,0);
                                     } else {
                                        if (WinSendDlgItemMsg(hwnd,RB_WITHDRAW,BM_QUERYCHECK,0,0)) {
                                           WinSendDlgItemMsg(hwnd,RB_DEPOSIT,BM_CLICK,0,0);
                                        } else {
                                           WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_CLICK,0,0);
                                        } /* endif */
                                     } /* endif */
                                  } /* endif */
                               } /* endif */
                            } /* endif */
                         } /* endif */
                     } /* endif */
                  } /* endif */
               } /* endif */

            } /* endif */
            return (MPARAM) TRUE;
            break;

        case WM_CLOSE:
          if (WinMessageBox (HWND_DESKTOP, hwnd, "Are you sure you want to exit?",
                             pszTitle, 0, MB_APPLMODAL |
                             MB_YESNO | MB_ICONQUESTION) == MBID_NO) {
             return (MRESULT) FALSE;
          } /* endif */

          WinStopTimer (hab, hwnd, ID_TIMER);
          KickThread(END );
          DosWaitEventSem(hevSemEnd, SEM_INDEFINITE_WAIT);
          WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
          return (MRESULT) FALSE;
                
        default:
                return(WinDefWindowProc(hwnd,msg,mp1,mp2));
        }
        return(FALSE);
}


// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
VOID CreateKeyPad(HWND hwnd)
{
   SHORT i,j;
   PSZ   BText = "1";
   ULONG ulWId = PB_KB_1;

   for (i=0;i<3 ;i++ ) {
      for (j=0;j<3 ;j++ ) {
       /* Create the WC_BUTTON "1" - "9" */
       WinCreateWindow(hwnd,
                       WC_BUTTON,
                       BText,
                       BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                       60+j*45, 320-i*45, 45, 45,
                       hwnd, HWND_TOP, ulWId++, 0L, NULL);
      *BText += 1; 
      } /* endfor */
   } /* endfor */

   /* Create the WC_BUTTON, "0"  */
   WinCreateWindow(hwnd,
                   WC_BUTTON,
                   "0",
                   BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                   60, 185, 90, 45,
                   hwnd, HWND_TOP, PB_KB_0, 0L, NULL);
   
   /* Create the WC_BUTTON, "."  */
   WinCreateWindow(hwnd,
                   WC_BUTTON,
                   "\xFE",
                   BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                   150, 185, 45, 45,
                   hwnd, HWND_TOP, PB_KB_DOT, 0L, NULL);
   
   /* Create the WC_BUTTON, "Ok"  */
   WinCreateWindow(hwnd,
                   WC_BUTTON,
                   "Ok",
                   BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                   60, 110, 60, 45,
                   hwnd, HWND_TOP, PB_KB_OK, 0L, NULL);
   
   /* Create the WC_BUTTON, "CLR"  */
   WinCreateWindow(hwnd,
                   WC_BUTTON,
                   "CLR",
                   BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                   135, 110, 60, 45,
                   hwnd, HWND_TOP, PB_KB_CLR, 0L, NULL);
   
   /* Create the WC_BUTTON, "Cancel"  */
   WinCreateWindow(hwnd,
                   WC_BUTTON,
                   "Cancel",
                   BS_PUSHBUTTON | WS_VISIBLE | BS_NOPOINTERFOCUS | WS_DISABLED,
                   60,  50, 135, 45,
                   hwnd, HWND_TOP, PB_KB_CANCEL, 0L, NULL);

   /* Create the WC_STATIC, "ATM Keypad"  */
   WinCreateWindow(hwnd,
                   WC_STATIC,
                   "ATM Keypad",
                   SS_GROUPBOX | WS_VISIBLE,
                   45,  35, 165, 360,
                   hwnd, HWND_TOP, GB_KB, 0L, NULL);
   
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
VOID KeyPadEnable(HWND hwnd, BOOL state)
{
   SHORT i;

   for (i=0;i<10;i++) {
       WinEnableWindow(WinWindowFromID(hwnd, PB_KB_0+i) , state );
   } /* endfor */
   WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CLR) , state );
   WinEnableWindow(WinWindowFromID(hwnd, PB_KB_DOT) , state );
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
VOID PressButton(HWND hwnd, ULONG id)
{
   HWND hwndButton = WinWindowFromID(hwnd, id);

   WinSendMsg(hwndButton, BM_CLICK, 0, 0);
   WinSendMsg(hwndButton, BM_SETHILITE, (MPARAM)TRUE, 0);
   DosSleep(50L);                     
   WinSendMsg(hwndButton, BM_SETHILITE, (MPARAM)FALSE, 0);
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
MRESULT EXPENTRY Animate( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    SWP   swp1;
    PSZ   pString;
    static POINTL aptl [4];
//  static CHAR  *szMsg = "12345678901234567890123456789012345678901234567890";
    static CHAR  *szMsg = "                              You are using SOMBank, an application developed with IBM's SOMobjects Developer Toolkit and utilizing Distributed SOM!   ";
    static FONTMETRICS fm;
    static POINTL ptl1;
           POINTL ptl;
    static RECTL  rectl1;
    static SHORT  sx, sy, snumchars, sindex;
    static SHORT  sMsgStart, sMsgLen;
    static USHORT  usFocus = 1;
    HPS   hps;

    switch(msg)
    {


        case WM_FOCUSCHANGE:
            usFocus = SHORT1FROMMP(mp2);
            break;

        case WM_PAINT:
            hps = WinGetPS (hwnd);
            GpiSetBackColor(hps,SYSCLR_DIALOGBACKGROUND);
            GpiSetColor(hps,CLR_DARKGRAY);
            ptl.x = sx; ptl.y = sy+3;
            GpiCharStringAt(hps,&ptl, snumchars+1,&szMsg[sindex-snumchars]);
            WinReleasePS (hps);
            break;

        case WM_CREATE:
            WinCreateWindow(hwnd,
                            WC_STATIC,
                            "",
                            SS_GROUPBOX | WS_VISIBLE,
                            300, 340, 275, 40,
                            hwnd, HWND_TOP, GB_ANIMATE, 0L, NULL);
            if ( ( WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN) >= 1024 ) &&
                 ( WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN) >= 768)  ) {
                 WinSetPresParam(hwnd, PP_FONTNAMESIZE, (ULONG) 14, (PVOID) "14.System VIO");
                 FontMetricsFromFont("14.System VIO", &fm);
            } else {
                 WinSetPresParam(hwnd, PP_FONTNAMESIZE, (ULONG) 14, (PVOID) "12.System VIO");
                 FontMetricsFromFont("12.System VIO", &fm);
            } /* endif */
            WinQueryWindowPos(WinWindowFromID(hwnd, GB_ANIMATE), &swp1);
            pString = szMsg;
            while (*pString == ' ') {
               sMsgStart++;
               pString++;
            } /* endwhile */
            sMsgLen = strlen(szMsg);
            sindex = sMsgStart;
            sx = swp1.x+4; sy = swp1.y+4; snumchars = swp1.cx / fm.lAveCharWidth - 2;
            aptl[0].x = sx ;                                            aptl[0].y = sy  ;  /* lower left target  */
            aptl[1].x = sx+fm.lAveCharWidth*snumchars;                  aptl[1].y = sy+fm.lMaxBaselineExt ;  /* upper-right target */
            aptl[2].x = sx+fm.lAveCharWidth ;                           aptl[2].y = sy ;  /* lower left source  */
            aptl[3].x = sx+fm.lAveCharWidth*snumchars+fm.lAveCharWidth; aptl[3].y = sy+fm.lMaxBaselineExt ;  /* upper-right source */
            ptl1.x = sx+fm.lAveCharWidth*snumchars;                     ptl1.y = sy+3;
            rectl1.xLeft   = ptl1.x;
            rectl1.yBottom = sy;
            rectl1.xRight  = ptl1.x+fm.lAveCharWidth;
            rectl1.yTop    = sy+fm.lMaxBaselineExt-1;
            WinStartTimer (hab, hwnd, ID_TIMER, 250);
            break;

        case WM_TIMER:

            if (usFocus) {
               hps = WinGetPS (hwnd);
               GpiSetBackColor(hps,SYSCLR_DIALOGBACKGROUND);
               GpiSetColor(hps,CLR_DARKGRAY);
               GpiBitBlt(hps, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);
               GpiCharStringPosAt(hps,&ptl1,&rectl1, CHS_OPAQUE, 1,&szMsg[sindex  = (sindex == sMsgLen) ? sMsgStart : ++sindex], NULL);
               WinReleasePS (hps);
            } /* endif */
            break;

    }
    return(FALSE);
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
int FontMetricsFromFont(PCHAR szFontNameSize, PFONTMETRICS pfm)
{

  CHAR  buff[5];
  HPS hps;
  PFONTMETRICS pFontStructure;
  long remainder,numberFonts,zero = 0L;
  short sFontSize ;

  while (*szFontNameSize != '.') {
     buff[zero++] = *szFontNameSize++; buff[zero] = 0;
  } /* endwhile */
  szFontNameSize++;
  sFontSize = atoi(buff) * 10;
  zero = 0L;


  hps = WinGetPS(HWND_DESKTOP);


                                //determine the number of fonts avail
  numberFonts = GpiQueryFonts(hps,QF_PUBLIC|QF_PRIVATE,szFontNameSize,&zero,
              (ULONG)sizeof(FONTMETRICS),NULL);

                                //Allocate space for structures

  pFontStructure = (PFONTMETRICS) malloc ((SHORT)sizeof(FONTMETRICS)*numberFonts);

                                //Get all of the fonts
  remainder = GpiQueryFonts(hps,QF_PUBLIC|QF_PRIVATE,szFontNameSize,&numberFonts,
            (ULONG)sizeof(FONTMETRICS),pFontStructure);

  for(zero=1;zero<=numberFonts;zero++)     //For each structure obtained
  {
                                //If font matches device resolution
     if(sFontSize == pFontStructure->sNominalPointSize) {
        memcpy(pfm,pFontStructure, sizeof(FONTMETRICS));
        free(pFontStructure);
        WinReleasePS(hps);
        return 0;
     }
     pFontStructure ++;
  }
  free(pFontStructure);
  WinReleasePS(hps);
  return 1;
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
VOID Sequencer(HWND hwnd)
{

   switch (phase) {

   case INIT:
      WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Welcome to SOMBank");
      WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Please Enter Your ID");
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_1),  FALSE);
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_2),  FALSE);
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_3),  FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_SAVINGS), FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_CHECKING),FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_INQUIRE), FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_WITHDRAW),FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_DEPOSIT), FALSE);
      WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),"");
      WinShowWindow(WinWindowFromID(hwnd, STX_ENTRY),  TRUE);
      WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  FALSE);
      KeyPadEnable(hwnd, TRUE);
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_DOT) , FALSE );
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CANCEL) , TRUE );
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_OK    ) , TRUE );
      phase = ACCOUNT;
      cAccountID = sAccountID = 0L;
      ulPin = ulCustomerCode = 0L;
      Buffer[0] = 0;
      break;

   case ACCOUNT:
      if (strlen(Buffer) != 4) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_3),"Error - ID has 4 digits!");
         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  TRUE );
         WinAlarm(HWND_DESKTOP, WA_ERROR);
         Buffer[0] = 0;
         WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),"");
         break;
      } /* endif */
      ulCustomerCode = atol(Buffer);
      WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Please Enter Your PIN");
      WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),"");
      phase = PIN;
      Buffer[0] = 0;
      break;

   case PIN:
      if (strlen(Buffer) != 4) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_3),"Error - PIN has 4 digits!");
         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  TRUE );
         WinAlarm(HWND_DESKTOP, WA_ERROR);
         Buffer[0] = 0;
         WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),"");
         break;
      } /* endif */
      ulPin = atol(Buffer);
      phase = CUST_VALID;
      KeyPadEnable(hwnd, FALSE);
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CANCEL) , FALSE);
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_OK    ) , FALSE);
      KickThread(CUSTOMER);
      break;

   case CUST_VALID:
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CANCEL) , TRUE );
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_OK    ) , TRUE );
      if (CustomerFound){
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_3),"Error - ID or PIN invalid!");
         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  TRUE );
         WinAlarm(HWND_DESKTOP, WA_ERROR);
         phase = INIT;
         break;
      }
      if (! (cAccountID || sAccountID)) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_3),"Error - No account exist!");
         WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  TRUE );
         WinAlarm(HWND_DESKTOP, WA_ERROR);
         phase = INIT;
         break;
      } /*end if*/
   case ANOTHERT:
      WinShowWindow(WinWindowFromID(hwnd, STX_ENTRY),  FALSE);
      KeyPadEnable(hwnd, FALSE);
      phase = ACCOUNTRANSACTYPE;
      if (cAccountID && sAccountID) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Your Account Types");
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Please Select Account");
         WinSetWindowText(WinWindowFromID(hwnd, STX_SEL_1),"Savings");
         WinSetWindowText(WinWindowFromID(hwnd, STX_SEL_2),"Checking");
         WinShowWindow(WinWindowFromID(hwnd, STX_SEL_1),  TRUE );
         WinShowWindow(WinWindowFromID(hwnd, STX_SEL_2),  TRUE );
         WinShowWindow(WinWindowFromID(hwnd, RB_SAVINGS), TRUE);
         WinShowWindow(WinWindowFromID(hwnd, RB_CHECKING),TRUE);
         break;
      } else {
         if (cAccountID) {
            WinCheckButton (hwnd, RB_CHECKING,  TRUE );
            WinCheckButton (hwnd, RB_SAVINGS,   FALSE);
         } else {
            WinCheckButton (hwnd, RB_CHECKING,  FALSE);
            WinCheckButton (hwnd, RB_SAVINGS,   TRUE );
         } /* endif */
      } /* endif */

   case ACCOUNTRANSACTYPE:
      if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Checking Account");
      } else {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Savings Account");
      } /* endif */
      WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Select Transaction Type");
      WinShowWindow(WinWindowFromID(hwnd, RB_SAVINGS), FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_CHECKING),FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_INQUIRE), TRUE);
      WinShowWindow(WinWindowFromID(hwnd, RB_WITHDRAW),TRUE);
      WinShowWindow(WinWindowFromID(hwnd, RB_DEPOSIT), TRUE);
      WinSetWindowText(WinWindowFromID(hwnd, STX_SEL_1),"Inquire");
      WinSetWindowText(WinWindowFromID(hwnd, STX_SEL_2),"Withdraw");
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_1),  TRUE );
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_2),  TRUE );
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_3),  TRUE );
      phase = TRANSACTYPE;
      break;

   case TRANSACTYPE:
      WinShowWindow(WinWindowFromID(hwnd, RB_INQUIRE), FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_WITHDRAW),FALSE);
      WinShowWindow(WinWindowFromID(hwnd, RB_DEPOSIT), FALSE);
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_1),  FALSE);
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_2),  FALSE);
      WinShowWindow(WinWindowFromID(hwnd, STX_SEL_3),  FALSE);
      phase = DOTRANSACTION;
      if (!WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_QUERYCHECK,0,0)) {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Please Enter Amount");
         KeyPadEnable(hwnd, TRUE);
         WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),"");
         WinShowWindow(WinWindowFromID(hwnd, STX_ENTRY),  TRUE);
         if (WinSendDlgItemMsg(hwnd,RB_DEPOSIT,BM_QUERYCHECK,0,0)) {
            if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
               WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Checking Account - Deposit");
            } else {
               WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Savings Account - Deposit");
            } /* endif */
         } else {
            if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
               WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Checking Account - Withdraw");
            } else {
               WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Savings Account - Withdraw");
            } /* endif */
         } /* endif */
         Buffer[0] = 0;
         break;
      } /* end if*/  

   case DOTRANSACTION:
      KeyPadEnable(hwnd, FALSE);
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CANCEL) , FALSE);
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_OK    ) , FALSE);
      if (WinSendDlgItemMsg(hwnd,RB_INQUIRE,BM_QUERYCHECK,0,0)) {
         if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
            WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Checking Account - Inquire");
            KickThread(INQCHECK  );
         } else {
            WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_1),"Savings Account - Inquire");
            KickThread(INQSAV  );
         } /* endif */
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"Current Balance");
      } else {
         WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_2),"New Balance");
         KeyPadEnable(hwnd, FALSE);
         dAmount = (double) atof(Buffer);
         if (WinSendDlgItemMsg(hwnd,RB_DEPOSIT,BM_QUERYCHECK,0,0)) {
            if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
                KickThread(DEPCHECK);
            }else {
                KickThread(DEPSAV  );
            } /* endif */
         } else {
            if (WinSendDlgItemMsg(hwnd,RB_CHECKING,BM_QUERYCHECK,0,0)) {
                KickThread(WITHDCHECK);
            }else {
                KickThread(WITHDSAV  );
            } /* endif */
         } /* endif */
      } /* end if*/  
      phase = TRANSACDONE;
      break;

   case TRANSACDONE:
      sprintf(Buffer,"%1.2f",dAmount);
      WinSetWindowText(WinWindowFromID(hwnd, STX_ENTRY),Buffer);
      if (!WinIsWindowShowing(WinWindowFromID(hwnd, STX_ENTRY ))) {
         WinShowWindow(WinWindowFromID(hwnd, STX_ENTRY),  TRUE);
      } /* endif */
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_CANCEL) , TRUE );
      WinEnableWindow(WinWindowFromID(hwnd, PB_KB_OK    ) , TRUE );
      WinSetWindowText(WinWindowFromID(hwnd, STX_MSG_3),"Another Transaction?");
      WinShowWindow(WinWindowFromID(hwnd, STX_MSG_3),  TRUE );
      WinAlarm(HWND_DESKTOP, WA_NOTE);
      phase = ENDTRANSACTION;
      break;

   default:
     break;
   } /* endswitch */

}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
void KickThread(enum THREADENUM  threadf)
{
   threadphase = threadf;
   bThreadRunning = TRUE;
   WinSendMsg(hwndFrame,USR_LIGHT,(MPARAM) 0, (MPARAM) 0);
   WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
   DosPostEventSem(hevSem);
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
void SOMCalls()
{
   BOOL keeprunning = TRUE;
   ULONG ulPostCt;

   e = somGetGlobalEnvironment();
   clClass=ClientClassNew();

   while (keeprunning) {

      bThreadRunning = FALSE;
      WinPostMsg(hwndClient,USR_THREAD  , (MPARAM) 0L , (MPARAM) 0L);
      DosWaitEventSem(hevSem, SEM_INDEFINITE_WAIT);
      DosResetEventSem(hevSem, &ulPostCt);

      switch (threadphase) {

      case CUSTOMER:
         if (!(CustomerFound = _restoreATMCustomer(clClass,e,ulCustomerCode,ulPin))) {
            cAccountID = __get_CheckingAccountID(clClass, e);
            sAccountID = __get_SavingsAccountID(clClass, e);
         } /* endif */
         break;

      case INQCHECK:
         _inquire(clClass,e,cAccountID,CHECKING,&dAmount);
         break;
      case WITHDCHECK:
         _withdraw(clClass,e,cAccountID, CHECKING,dAmount);
         _inquire(clClass,e,cAccountID,CHECKING,&dAmount);
         break;
      case DEPCHECK:
         _deposit(clClass,e,cAccountID, CHECKING,dAmount);
         _inquire(clClass,e,cAccountID,CHECKING,&dAmount);
         break;

      case INQSAV:
         _inquire(clClass,e,sAccountID, SAVINGS,&dAmount);
         break;
      case WITHDSAV:
         _withdraw(clClass,e,sAccountID, SAVINGS,dAmount);
         _inquire(clClass,e,sAccountID, SAVINGS,&dAmount);
         break;
      case DEPSAV:
         _deposit(clClass,e,sAccountID, SAVINGS,dAmount);
         _inquire(clClass,e,sAccountID, SAVINGS,&dAmount);
         break;

      case END:
         _somFree(clClass);
         keeprunning = FALSE;
         break;
      } /* endswitch */

   } /* endwhile */

   DosPostEventSem(hevSemEnd);
   _endthread;
}

// þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
    SWP   swp1, swp2;

    switch (msg) {
         case WM_INITDLG:
             WinQueryWindowPos(hwnd, &swp1);
             WinQueryWindowPos(hwndFrame, &swp2);
             WinSetWindowPos(hwnd, HWND_TOP, (swp2.cx-swp1.cx)/2+swp2.x,(swp2.cy-swp1.cy)/2+swp2.y,0,0, SWP_MOVE);
             return 0;

         case WM_COMMAND:
              switch (COMMANDMSG(&msg)->cmd) {
                   case DID_OK:
                   case DID_CANCEL:
                        WinDismissDlg (hwnd, TRUE) ;
                        return 0 ;
              }
              break ;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
}


#ifdef NOSOM
LONG ClientClassNew()
{
  return 0L;
}

VOID _somFree(LONG clClass)
{
}

LONG *somGetGlobalEnvironment()
{
   return NULL;
}

BOOL _restoreATMCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode,ULONG ulPin)
{
   DosSleep( 500L);
   if (ulPin == ulCustomerCode) {
      return FALSE;
   } else {
      return TRUE;
   } /* endif */
}

LONG __get_CheckingAccountID(LONG clClass,LONG *e)
{    
   return 1L;
}

LONG __get_SavingsAccountID(LONG clClass,LONG *e)
{    
   return 2L;
}

VOID _inquire (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double *dAmount)
{
  if (type == SAVINGS) {
     *dAmount = dSav;
  } else {
     *dAmount = dChe;
  } /* endif */
}

VOID _withdraw(LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double dAmount)
{
   DosSleep( 500L);
  if (type == SAVINGS) {
     dSav -= dAmount;
  } else {
     dChe -= dAmount;
  } /* endif */
}

VOID _deposit (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double dAmount)
{
  DosSleep( 100L);
  if (type == SAVINGS) {
     dSav += dAmount;
  } else {
     dChe += dAmount;
  } /* endif */
}
#endif

