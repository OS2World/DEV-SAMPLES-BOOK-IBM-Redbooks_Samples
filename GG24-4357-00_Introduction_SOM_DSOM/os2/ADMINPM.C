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
 *   PM Version of ADMIN client                                         *
 *                                                                      *
 * ******************************************************************** *
 */                                                                    
#define INCL_WIN
#define INCL_DOS
#define INCL_GPI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <os2.h>                       /* System Include File      */
#ifndef NOSOM
 #include <somd.h>
 #include "clientcl.h"
 #include "bankdef.h"
#endif
#include "ADMINPM.h"                  /* Application Include File */
#define INCL_WINSTDBOOK

MRESULT EXPENTRY ADMINPMDlgProcDel( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ADMINPMDlgProcAbout( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ADMINPMWinProcClient( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ADMINPMDlgProcCover( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY ADMINPMDlgProcAdd( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY FrameWndProc(HWND hwnd,ULONG msg,MPARAM mp1, MPARAM mp2);
void DisplayLastError(HAB hab);
SHORT  DebugPrintf(PCHAR szFormat, ...);

#ifdef NOSOM
LONG  *somGetGlobalEnvironment();
LONG  ClientClassNew();
VOID  _somFree(LONG clClass);
VOID  _inquire (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double *dAmount);
BOOL  _restoreCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode);
LONG  _createCustomer(LONG clClass,LONG *e, CHAR *Name,CHAR *Branch,LONG *CustomerPIN);
BOOL  _deleteCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode);
VOID  _createCheckingAccount(LONG clClass,LONG *e, double Input);
VOID  _createSavingsAccount(LONG clClass,LONG *e, double Input);
LONG  __get_CustomerID(LONG clClass,LONG *e);
PCHAR __get_CustomerName(LONG clClass,LONG *e);
PCHAR __get_CustomerBranchID(LONG clClass,LONG *e);
LONG  __get_CheckingAccountID(LONG clClass,LONG *e);
LONG  __get_SavingsAccountID(LONG clClass,LONG *e);
#endif

/* Global Variables */
PCHAR pszTitle = "SOMBank Admin Application";
enum THREADENUM { CREATE=1, INQUIRE, DELETE, END} threadphase;
BOOL VGA, R1024x768, R1280x1024 ;
ULONG  Fcx, Fcy;
HAB hab;
ULONG flCreate1;
HWND hwndFrame;
HWND hwndClient;
HMODULE hModADMINPM;
HWND hwndCover;
HWND hwndAbout;
//HWND hwndInquire;
HWND hwndAdd;
HWND hwndDel;
PFNWP   OldpFrame;
BOOL    bThreadRunning;
HEV     hevSem;
HEV     hevSemEnd;
HWND hwndNotebook;  /* Notebook handle */

VOID KickThread(enum THREADENUM  threadf);
VOID _Optlink SOMCalls();
#define SAVINGS         2
#define CHECKING        3

#ifdef NOSOM
 LONG         clClass;
 LONG         *e;
 double       dSav, dChe;
 char         cName[50];
 char         cBranch[5]; 
 LONG         ulCustID;
 BOOL         bSav, bChe;
#else
 ClientClass  clClass;
 Environment  *e;
#endif

struct {
   CHAR cName[50];
   CHAR cBranch[5];
   LONG lCustID;
   LONG lCustPIN;
   LONG lSavID;
   LONG lCheckID;
   double dSav;
   double dCheck;
   BOOL bSav;
   BOOL bCheck;
   BOOL bResult;
} CustInfo;


// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
main(int argc,char *argv[],char *envp[])
{
        QMSG qmsg;               /* Message Queue                */
    HMQ hmq;                 /* Handle to Queue              */
    HPS hps;                 /* Handle to Presentation Space */
    
    DosCreateEventSem(NULL,&hevSem, 0, FALSE);
    DosCreateEventSem(NULL,&hevSemEnd, 0, FALSE);

    if ( ( WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN) >= 1280 ) &&
         ( WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN) >= 1024)  ) {
       R1280x1024 = TRUE;
    } else {
    if ( ( WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN) >= 1024 ) &&
         ( WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN) >= 768)  ) {
       R1024x768 = TRUE; 
    } else { 
       VGA = TRUE; 
    } } /* endif */

    /* Intialize PM and obtain anchor block */
    hab = WinInitialize((USHORT) NULL);
        
    /* Create queue to receive messsages */
    hmq = WinCreateMsgQueue(hab,0);
    
    /* Register the window class "ADMINPMPmgActualWindow1" with procedure ADMINPMWinProcClient */
    WinRegisterClass(hab,
                     "ADMINPMPmgActualWindow1",
                     (PFNWP) ADMINPMWinProcClient,
                     CS_SIZEREDRAW,
                     0);
    
    flCreate1= FCF_BORDER | FCF_MINBUTTON | FCF_SYSMENU | FCF_TASKLIST | FCF_TITLEBAR | FCF_ICON;
    
    /* Create a standard window */
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                        WS_VISIBLE,
                                       &flCreate1,
                                       "ADMINPMPmgActualWindow1",
                                       pszTitle,
                                       0L,
                                       hModADMINPM,
                                       ID_ADMINPM,
                                       (PHWND) & hwndClient);
    
    OldpFrame = WinSubclassWindow(hwndFrame, (PFNWP)FrameWndProc);

    /* Set the window position */
    WinSetWindowPos(hwndFrame,
                    HWND_BOTTOM,
                    (WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN)-Fcx)/2,
                    (WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)-Fcy)/2,
                    Fcx,
                    Fcy,
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

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
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

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
MRESULT EXPENTRY ADMINPMWinProcClient( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
//    static HWND hwndNotebook;  /* Notebook handle */
    static LONG nbPageidAbout;
    static LONG nbPageidInquire;
    static LONG nbPageidDel;
    static LONG nbPageidAdd;
    LONG nbPageid;
    HPS hps;                 /* Handle to Presentation Space */
    RECTL rectl;             /* Dimensions of window         */
    PAGESELECTNOTIFY *pNewPage;

    switch(msg)
    {
            
        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefWindowProc(hwnd,msg,mp1,mp2));

        case USR_THREAD:
            WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));
            WinSendMsg(hwndFrame,USR_LIGHT,(MPARAM) 0, (MPARAM) 0);
            WinEnableWindow(hwndNotebook, TRUE);
            break;

        case WM_CREATE:
            
            /* Create the WC_NOTEBOOK, ""  */
            WinCreateWindow(hwnd,
                            WC_NOTEBOOK,
                            "",
                             WS_VISIBLE | BKS_BACKPAGESBR | BKS_MAJORTABRIGHT | BKS_ROUNDEDTABS | BKS_SPIRALBIND,
                            0,
                            -20,
                            0,  // will be calculated in the cover dialog
                            0,  // will be calculated in the cover dialog 
                            hwnd,
                            HWND_TOP,
                            BK_ADMIN,
                            0L,
                            NULL);
            
                           /* Store notebook window handle */
            hwndNotebook = WinWindowFromID(hwnd, BK_ADMIN);
            if (VGA) {
               WinSendDlgItemMsg (hwnd, BK_ADMIN , BKM_SETDIMENSIONS,
                     MPFROM2SHORT (70,30), MPFROMSHORT (BKA_MAJORTAB));
            } else {
               if (R1024x768) {
                  WinSendDlgItemMsg (hwnd, BK_ADMIN , BKM_SETDIMENSIONS,
                        MPFROM2SHORT (90,30), MPFROMSHORT (BKA_MAJORTAB));
               } else {
                  WinSendDlgItemMsg (hwnd, BK_ADMIN , BKM_SETDIMENSIONS,
                        MPFROM2SHORT (100,30), MPFROMSHORT (BKA_MAJORTAB));
               } /* endif */
            } /* endif */
            WinSendDlgItemMsg (hwnd, BK_ADMIN , BKM_SETDIMENSIONS,
                  MPFROM2SHORT (20,20),
                  MPFROMSHORT (BKA_MINORTAB));
            WinSendDlgItemMsg (hwnd, BK_ADMIN , BKM_SETDIMENSIONS,
                  MPFROM2SHORT (20,20),
                  MPFROMSHORT (BKA_PAGEBUTTON));

            hwndCover = WinLoadDlg(hwndNotebook,
                                      hwndNotebook,
                                      ADMINPMDlgProcCover,
                                      hModADMINPM,
                                      ID_DLG_COVER,
                                      NULL);
            nbPageid=LONGFROMMR(WinSendDlgItemMsg(hwnd,BK_ADMIN, BKM_INSERTPAGE, 0,
                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON | BKA_MAJOR, BKA_LAST)));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageid),
                MPFROMP(hwndCover));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETTABTEXT , MPFROMLONG(nbPageid),
              MPFROMP("Cover"));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETSTATUSLINETEXT , MPFROMLONG(nbPageid),
              MPFROMP(""""));

            nbPageidAbout=LONGFROMMR(WinSendDlgItemMsg(hwnd,BK_ADMIN, BKM_INSERTPAGE, 0,
                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON | BKA_MAJOR, BKA_LAST)));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETTABTEXT , MPFROMLONG(nbPageidAbout),
              MPFROMP("Appl Info"));

            nbPageidInquire=LONGFROMMR(WinSendDlgItemMsg(hwnd,BK_ADMIN, BKM_INSERTPAGE, 0,
                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON | BKA_MAJOR, BKA_LAST)));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETTABTEXT , MPFROMLONG(nbPageidInquire),
              MPFROMP("Inquire"));
            

            nbPageidAdd=LONGFROMMR(WinSendDlgItemMsg(hwnd,BK_ADMIN, BKM_INSERTPAGE, 0,
                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON | BKA_MAJOR, BKA_LAST)));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETTABTEXT , MPFROMLONG(nbPageidAdd),
              MPFROMP("Create"));

            nbPageidDel=LONGFROMMR(WinSendDlgItemMsg(hwnd,BK_ADMIN, BKM_INSERTPAGE, 0,
                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON | BKA_MAJOR, BKA_LAST)));
            
            WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETTABTEXT , MPFROMLONG(nbPageidDel),
              MPFROMP("Delete"));
            
            bThreadRunning = TRUE;
            WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
            WinEnableWindow(hwndNotebook, FALSE);
            _beginthread(SOMCalls, NULL, 32768, NULL);
            break;

        case WM_CONTROL:
            switch (SHORT2FROMMP(mp1))
            {
            case BK_ADMIN:
                break;
            case BKN_PAGESELECTED:
               pNewPage =  (PAGESELECTNOTIFY *) mp2;
               if ((pNewPage->ulPageIdNew == nbPageidAbout) && (!hwndAbout) ) {
                  hwndAbout = WinLoadDlg(hwndNotebook,
                                            hwndNotebook,
                                            ADMINPMDlgProcAbout,
                                            hModADMINPM,
                                            ID_DLG_ABOUT,
                                            NULL);
                  
                  WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidAbout),
                      MPFROMP(hwndAbout));

                  WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETSTATUSLINETEXT , MPFROMLONG(nbPageidAbout),
                      MPFROMP("Product Information"));
                  
               } else {
               if ((pNewPage->ulPageIdNew == nbPageidAdd) && !hwndAdd ) {
                  hwndAdd = WinLoadDlg(hwndNotebook,
                                            hwndNotebook,
                                            ADMINPMDlgProcAdd,
                                            hModADMINPM,
                                            ID_DLG_ADDCUSTOMER,
                                            NULL);
                  WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidAdd),
                      MPFROMP(hwndAdd));
                  
                  WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETSTATUSLINETEXT , MPFROMLONG(nbPageidAdd),
                    MPFROMP("Create Customer Account"));
               } else {
               if ((pNewPage->ulPageIdNew == nbPageidDel) || (pNewPage->ulPageIdNew == nbPageidInquire) ) {
                  if (!hwndDel) {
                     hwndDel = WinLoadDlg(hwndNotebook,
                                               hwndNotebook,
                                               ADMINPMDlgProcDel,
                                               hModADMINPM,
                                               ID_DLG_DELCUSTOMER,
                                               NULL);
                     WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETSTATUSLINETEXT , MPFROMLONG(nbPageidDel),
                       MPFROMP("Delete Customer Account"));
                     WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETSTATUSLINETEXT , MPFROMLONG(nbPageidInquire),
                         MPFROMP("Inquire Customer Account"));
                     if ( pNewPage->ulPageIdNew == nbPageidDel) {
                        WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidInquire),
                            MPFROMP(hwndDel));
   
                        WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidDel),
                            MPFROMP(hwndDel));
   
                     } else {
                        WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidDel),
                            MPFROMP(hwndDel));
                        
                        WinSendDlgItemMsg(hwnd,BK_ADMIN,BKM_SETPAGEWINDOWHWND , MPFROMLONG(nbPageidInquire),
                            MPFROMP(hwndDel));
                     
                     } /* endif */
                  } /* endif */
               } } } /* endif */
               if (pNewPage->ulPageIdNew == nbPageidAdd ) {
                  WinSendMsg(hwndAdd,USR_ADD_CUSTOMER,0,0);
               } else {
               if (pNewPage->ulPageIdNew == nbPageidDel ) {
                  WinSetActiveWindow(HWND_DESKTOP,hwndDel); 
                  WinSendMsg(hwndDel,USR_DEL_CUSTOMER,0,0);
               } else {
               if (pNewPage->ulPageIdNew == nbPageidInquire ) {
                  WinSetActiveWindow(HWND_DESKTOP,hwndDel); 
                  WinSendMsg(hwndDel,USR_INQ_CUSTOMER,0,0);
               } } } /* endif */
               break;
            }
            break;

        case WM_PAINT:
            
            /* Default code for WM_PAINT */
            {
            ULONG  clr,attrfound;
            LONG ColorTable;
            POINTL p;
            
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
            
            
            
            WinEndPaint( hps );
            }
            break;

     case WM_CLOSE:
          if (WinMessageBox (HWND_DESKTOP, hwnd, "Are you sure you want to exit?",
                             pszTitle, 0, MB_APPLMODAL |
                             MB_YESNO | MB_ICONQUESTION) == MBID_NO) {
             return (MRESULT) FALSE;
          } /* endif */

          KickThread(END );
          DosWaitEventSem(hevSemEnd, SEM_INDEFINITE_WAIT);
          WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
          return (MRESULT) FALSE;

        default:
            return(WinDefWindowProc(hwnd,msg,mp1,mp2));
    }
    return(FALSE);
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
MRESULT EXPENTRY ADMINPMDlgProcAbout( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   RECTL rectl;   
    switch(msg)
    {

        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefDlgProc(hwnd,msg,mp1,mp2));

        case WM_INITDLG:
            WinQueryWindowRect(WinWindowFromID(hwndClient, BK_ADMIN),&rectl);
            WinSendMsg(WinWindowFromID(hwndClient, BK_ADMIN),BKM_CALCPAGERECT , (MPARAM) &rectl, (MPARAM)TRUE);
//            WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,0,0,SWP_MOVE);
            WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,
                            rectl.xRight-rectl.xLeft,
                            rectl.yTop-rectl.yBottom,SWP_MOVE | SWP_SIZE);
            break;

        case WM_PAINT:
            
            /* Execute default processing */
            return(WinDefDlgProc(hwnd, msg, mp1, mp2) );
            break;

        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
    }
    return(FALSE);
}


// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
MRESULT EXPENTRY ADMINPMDlgProcAdd( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CHAR Buffer[30];
    RECTL rectl;   
    switch(msg)
    {
        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefDlgProc(hwnd,msg,mp1,mp2));

        case USR_ADD_CUSTOMER:
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd, EF_ADD_NAME));
            return (MRESULT) (TRUE);   /* required for  set focus */

        case USR_ADD_DONE:
            if (CustInfo.bResult) {
               WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_MESSAGE),  "Customer Created");
               if (CustInfo.lCheckID) 
                  WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_ID_C),  _ltoa(CustInfo.lCheckID,Buffer,10));
               if (CustInfo.lSavID) 
                  WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_ID_S),  _ltoa(CustInfo.lSavID,Buffer,10));
               sprintf(Buffer,"%04u",CustInfo.lCustID);
               WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_CUSTOMER_ID_1),  Buffer);
               sprintf(Buffer,"%04u",CustInfo.lCustPIN);
               WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_CUSTOMER_PIN_1), Buffer);
            } else {
               WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_MESSAGE),  "Customer Creation Failed");
            } /* endif */
            break;

        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
            case PB_ADD_DOIT:
                WinQueryDlgItemText(hwnd, EF_ADD_NAME,  50, &CustInfo.cName[0]);
                WinQueryDlgItemText(hwnd, CB_ADD_BRANCH, 5,  &CustInfo.cBranch[0]);
                CustInfo.lCustID = 0;
                CustInfo.lCustPIN = 0;
                CustInfo.lCheckID = 0;
                CustInfo.lSavID = 0;
                if ( WinQueryButtonCheckstate(hwnd,PB_ADD_CHECKING)) {
                   CustInfo.bCheck = TRUE;
                   WinQueryDlgItemText(hwnd, EF_ADD_CHECKING, 30,  Buffer);
                   CustInfo.dCheck = atof(Buffer);
                } else {
                   CustInfo.bCheck = FALSE;
                } /* endif */
                if ( WinQueryButtonCheckstate(hwnd,PB_ADD_SAVINGS )) {
                   CustInfo.bSav   = TRUE;
                   WinQueryDlgItemText(hwnd, EF_ADD_SAVINGS , 30,  Buffer);
                   CustInfo.dSav   = atof(Buffer);
                } else {
                   CustInfo.bSav   = FALSE;
                } /* endif */
                WinEnableWindow(WinWindowFromID(hwnd,PB_ADD_DOIT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd,PB_ADD_CLEAR), TRUE);
                KickThread(CREATE);
                break;
            case PB_ADD_CLEAR:
                WinSetWindowText(WinWindowFromID(hwnd,EF_ADD_NAME),  "");
                WinSetWindowText(WinWindowFromID(hwnd,EF_ADD_SAVINGS),  "");
                WinCheckButton(hwnd, PB_ADD_SAVINGS, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_SAVINGS), FALSE);
                WinSetWindowText(WinWindowFromID(hwnd,EF_ADD_CHECKING),  "");
                WinCheckButton(hwnd, PB_ADD_CHECKING, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_CHECKING), FALSE);
                WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_MESSAGE),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_ID_C),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_ID_S),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_CUSTOMER_ID_1),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_ADD_CUSTOMER_PIN_1), "");
                WinEnableWindow(WinWindowFromID(hwnd,PB_ADD_DOIT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd,PB_ADD_CLEAR), FALSE);
                break;
            }
            break;
        case WM_CONTROL:
            switch (SHORT1FROMMP(mp1))
            {
            case EF_ADD_NAME:
                break;
            case EF_ADD_CHECKING:
                break;
            case EF_ADD_SAVINGS:
                break;
            case CB_ADD_BRANCH:
                break;
            case PB_ADD_CHECKING:
                switch (SHORT2FROMMP(mp1))
                {
                    case BN_CLICKED:
                        
                        if ( WinQueryButtonCheckstate(hwnd,PB_ADD_CHECKING)) {
                           /* Set button PB_ADD_CHECKING to checked */
                           WinCheckButton(hwnd, PB_ADD_CHECKING, FALSE);
                           WinSetWindowText(WinWindowFromID(hwnd,EF_ADD_CHECKING),  "");
                           WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_CHECKING), FALSE);
                        } else {
                           /* Set button PB_ADD_CHECKING to checked */
                           WinCheckButton(hwnd, PB_ADD_CHECKING, TRUE);
                           WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_CHECKING), TRUE);
                        } /* endif */
                        break;
                }
                break;
            case PB_ADD_SAVINGS:
                switch (SHORT2FROMMP(mp1))
                {
                    case BN_CLICKED:
                        
                        if ( WinQueryButtonCheckstate(hwnd,PB_ADD_SAVINGS)) {
                           /* Set button PB_ADD_SAVINGS to checked */
                           WinCheckButton(hwnd, PB_ADD_SAVINGS, FALSE);
                           WinSetWindowText(WinWindowFromID(hwnd,EF_ADD_SAVINGS),  "");
                           WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_SAVINGS), FALSE);
                        } else {
                           /* Set button PB_ADD_SAVINGS to checked */
                           WinCheckButton(hwnd, PB_ADD_SAVINGS, TRUE);
                           WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_SAVINGS), TRUE);
                        } /* endif */
                        break;
                }
                break;
            }
            break;
        case WM_INITDLG:
                
            WinQueryWindowRect(WinWindowFromID(hwndClient, BK_ADMIN),&rectl);
            WinSendMsg(WinWindowFromID(hwndClient, BK_ADMIN),BKM_CALCPAGERECT , (MPARAM) &rectl, (MPARAM)TRUE);
//            WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,0,0,SWP_MOVE);
            WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,
                            rectl.xRight-rectl.xLeft,
                            rectl.yTop-rectl.yBottom,SWP_MOVE | SWP_SIZE);

            /* Max no. of chars for EF_ADD_NAME is 49 */
            WinSendDlgItemMsg(hwnd,
                              EF_ADD_NAME,
                              EM_SETTEXTLIMIT,
                              MPFROMSHORT(49),
                              0L);
            
            /* Set the text of CB_ADD_BRANCH to "B001" */
            WinSetWindowText(WinWindowFromID(hwnd,CB_ADD_BRANCH), "B001");
            
            /* Max no. of chars for CB_ADD_BRANCH   is 4 */
            WinSendDlgItemMsg(hwnd,
                              CB_ADD_BRANCH,
                              EM_SETTEXTLIMIT,
                              MPFROMSHORT(4),
                              0L);
            
            /* Max no. of chars for EF_ADD_CHECKING is 30 */
            WinSendDlgItemMsg(hwnd,
                              EF_ADD_CHECKING,
                              EM_SETTEXTLIMIT,
                              MPFROMSHORT(30),
                              0L);
            
            /* Max no. of chars for EF_ADD_SAVINGS is 30 */
            WinSendDlgItemMsg(hwnd,
                              EF_ADD_SAVINGS,
                              EM_SETTEXTLIMIT,
                              MPFROMSHORT(30),
                              0L);
            WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_CHECKING), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,EF_ADD_SAVINGS), FALSE);
            
            /* Add item to listbox CB_ADD_BRANCH and sort in ascending order */
            WinSendDlgItemMsg(hwnd,
                              CB_ADD_BRANCH,
                              LM_INSERTITEM,
                              (MPARAM) LIT_SORTASCENDING,
                              MPFROMP("B001") /* text */
                              );
            
            /* Add item to listbox CB_ADD_BRANCH and sort in ascending order */
            WinSendDlgItemMsg(hwnd,
                              CB_ADD_BRANCH,
                              LM_INSERTITEM,
                              (MPARAM) LIT_SORTASCENDING,
                              MPFROMP("B002") /* text */
                              );
            
            /* Add item to listbox CB_ADD_BRANCH and sort in ascending order */
            WinSendDlgItemMsg(hwnd,
                              CB_ADD_BRANCH,
                              LM_INSERTITEM,
                              (MPARAM) LIT_SORTASCENDING,
                              MPFROMP("B003") /* text */
                              );
            WinEnableWindow(WinWindowFromID(hwnd,PB_ADD_CLEAR), FALSE);
            break;

        case WM_PAINT:
            
            /* Execute default processing */
            return(WinDefDlgProc(hwnd, msg, mp1, mp2) );
            break;

        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
        }
        return(FALSE);
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
MRESULT EXPENTRY ADMINPMDlgProcDel( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   CHAR CustomerID[5];
   CHAR Buffer[30];
   RECTL rectl;   
   static BOOL Inquire, Delete, CustomerFound, CustomerDeleted;

    switch(msg)
    {

        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefDlgProc(hwnd,msg,mp1,mp2));

        case USR_INQ_CUSTOMER:
            Inquire = TRUE; Delete = FALSE;
            if (CustomerFound) {
                WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT), FALSE);
            } /* endif */
            WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Do It");
            WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Clear");
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd, EF_DEL_CUSTOMER));
            return (MRESULT) (TRUE);  

        case USR_DEL_CUSTOMER:
            Inquire = FALSE; Delete = TRUE;
            if (CustomerFound && !CustomerDeleted) {
                WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT), TRUE);
                WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Del It");
                WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Cancel");
            } else {
                WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Find");
                WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Clear");
            } /* endif */
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd, EF_DEL_CUSTOMER));
            return (MRESULT) (TRUE);   /* required for set focus */

        case USR_DEL_DONE:
            if (CustInfo.bResult) {
              WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT ), FALSE);
              WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Clear");
              WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Find");
              WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_MESSAGE),  "Customer Deleted");
              CustomerDeleted = TRUE; 
            } /* endif */
            break; 
            
        case USR_DEL_FINDDONE:
            if (CustInfo.bResult) {
               WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_NAME_1),  CustInfo.cName);
               WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_BRANCH_1),CustInfo.cBranch);
               if (CustInfo.lCheckID) {
                  WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_C_ID), _ltoa(CustInfo.lCheckID,Buffer,10) );
                  sprintf(Buffer,"%1.2f",CustInfo.dCheck);
                  WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_C_BAL), Buffer);
               } 
               if (CustInfo.lSavID) {
                  WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_S_ID), _ltoa(CustInfo.lSavID,Buffer,10)   );
                  sprintf(Buffer,"%1.2f",CustInfo.dSav  );
                  WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_S_BAL), Buffer);
               }
               WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_MESSAGE),  "Customer Found");
               CustomerFound = TRUE; 
               if (Inquire) {
                  WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT ), FALSE);
               } else {
                  WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Del It");
                  WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Cancel");
               } /* endif */
            } else {
               WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT ), FALSE);
               WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_MESSAGE),  "Customer Not Found");
            } /* endif */
            break; 
            
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
            case PB_DEL_DOIT:
                WinQueryDlgItemText(hwnd, EF_DEL_CUSTOMER,  5, CustomerID );
                CustInfo.lCustID = atol(CustomerID);
                if (!CustomerFound) {
                   KickThread(INQUIRE);
                } else {
                   KickThread(DELETE);
                } /* endif */
                WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_CLEAR), TRUE);
                break;
            case PB_DEL_CLEAR:
                CustomerFound = FALSE; 
                CustomerDeleted = FALSE; 
                WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_CLEAR), "Clear");
                if (Inquire) {
                   WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Do It");
                } else {
                   WinSetWindowText(WinWindowFromID(hwnd,PB_DEL_DOIT),  "Find");
                } /* endif */
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_NAME_1),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_BRANCH_1),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_C_ID),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_S_ID),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_C_BAL),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_S_BAL),  "");
                WinSetWindowText(WinWindowFromID(hwnd,STX_DEL_MESSAGE),  "");
                WinSetWindowText(WinWindowFromID(hwnd,EF_DEL_CUSTOMER), "");
                WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_DOIT ), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_CLEAR), FALSE);
                break;
            }
            break;

        case WM_CONTROL:
            switch (SHORT1FROMMP(mp1))
            {
            case EF_DEL_CUSTOMER:
                break;
            }
            break;

        case WM_INITDLG:
                
            WinQueryWindowRect(WinWindowFromID(hwndClient, BK_ADMIN),&rectl);

            WinSendMsg(WinWindowFromID(hwndClient, BK_ADMIN),BKM_CALCPAGERECT , (MPARAM) &rectl, (MPARAM)TRUE);
//            WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,0,0,SWP_MOVE);
             WinSetWindowPos(hwnd,HWND_TOP,rectl.xLeft,rectl.yBottom,
                            rectl.xRight-rectl.xLeft,
                            rectl.yTop-rectl.yBottom,SWP_MOVE | SWP_SIZE);
            /* Max no. of chars for EF_DEL_CUSTOMER is 4 */
            WinSendDlgItemMsg(hwnd, EF_DEL_CUSTOMER,
                              EM_SETTEXTLIMIT, MPFROMSHORT(4), 0L);
            WinEnableWindow(WinWindowFromID(hwnd,PB_DEL_CLEAR), FALSE);
            break;
                
        case WM_PAINT:
            
            /* Execute default processing */
            return(WinDefDlgProc(hwnd, msg, mp1, mp2) );
            break;

        default:
            return(WinDefDlgProc(hwnd,msg,mp1,mp2));
        }
        return(FALSE);
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
MRESULT EXPENTRY ADMINPMDlgProcCover( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   static HWND  hwndBMP;
   HPS hps;
   RECTL  rectl;

   switch(msg)
   {
        case WM_CONTROLPOINTER:
        case WM_MOUSEMOVE:
           if (bThreadRunning) {
               return (MRESULT) WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
           } /* endif */
        return(WinDefDlgProc(hwnd,msg,mp1,mp2));

       case WM_INITDLG:
           /* get the size of our dialog box */
           WinQueryWindowRect(hwnd,&rectl);
           /* calcualte the size the notebook should have */
           WinSendMsg(hwndNotebook,BKM_CALCPAGERECT , (MPARAM) &rectl, (MPARAM)FALSE);
           /* calcualte the size of our frame window */
           Fcx = rectl.xRight-rectl.xLeft;
           Fcy = rectl.yTop-rectl.yBottom-20+WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR);
//               DisplayLastError(hab);
           /* set the notebook size  */
           WinSetWindowPos(hwndNotebook,HWND_TOP,
                            0,0, 
                            rectl.xRight-rectl.xLeft,
                            rectl.yTop-rectl.yBottom, SWP_SIZE | SWP_SHOW);
           /* Get handle to presentation space */
           hps = WinGetPS (hwnd);
           /* Call GpiLoadBitMap to obtain a handle for the bitmap */
           hwndBMP =  GpiLoadBitmap(hps,0,BIT_ITSC,740,726);
           /* Return handle to presentation space */
           WinReleasePS (hps);
           break;

       case WM_PAINT:
           
           /* Execute default processing */
           WinDefDlgProc(hwnd, msg, mp1, mp2);
           hps = WinGetPS (hwnd);
           if ( VGA ) {
              rectl.xLeft   = 30;        
              rectl.yBottom = 100;       
              rectl.xRight  = rectl.xLeft    + 225;  
              rectl.yTop    = rectl.yBottom  + 225; 
           } else {
              if (R1024x768) {
                 rectl.xLeft   = 50;        
                 rectl.yBottom = 130;       
                 rectl.xRight  = rectl.xLeft    + 300;  
                 rectl.yTop    = rectl.yBottom  + 300; 
              } else {
                 rectl.xLeft   = 60;        
                 rectl.yBottom = 120;       
                 rectl.xRight  = rectl.xLeft    + 345;  
                 rectl.yTop    = rectl.yBottom  + 355; 
              } /* endif */
           } /* endif */
           WinDrawBitmap(hps, hwndBMP, NULL,(PPOINTL) &rectl, DBM_IMAGEATTRS, DBM_IMAGEATTRS, DBM_STRETCH);
           WinReleasePS (hps);
           break;
           
        case WM_CLOSE:
           GpiDeleteBitmap(hwndBMP);
           return(WinDefDlgProc(hwnd,msg,mp1,mp2));
           
       default:
           return(WinDefDlgProc(hwnd,msg,mp1,mp2));
   }
   return(FALSE);
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
void KickThread(enum THREADENUM  threadf)
{
   threadphase = threadf;
   bThreadRunning = TRUE;
   WinSendMsg(hwndFrame,USR_LIGHT,(MPARAM) 0, (MPARAM) 0);
   WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
   WinEnableWindow(hwndNotebook, FALSE);
   DosPostEventSem(hevSem);
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
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
  
      case CREATE:
          _createCustomer(clClass, e, &CustInfo.cName[0], &CustInfo.cBranch[0],&CustInfo.lCustPIN);
          CustInfo.lCustID = __get_CustomerID(clClass,e);
          CustInfo.lSavID = CustInfo.lCheckID = 0;
          if (CustInfo.bCheck) 
             _createCheckingAccount(clClass,e, CustInfo.dCheck);
          if (CustInfo.bSav  ) 
             _createSavingsAccount(clClass,e, CustInfo.dSav  );
          if (_restoreCustomer(clClass, e, CustInfo.lCustID)==0){
             CustInfo.bResult = TRUE; 
             if (CustInfo.bCheck) 
                CustInfo.lCheckID = __get_CheckingAccountID(clClass, e);
             if (CustInfo.bSav  ) 
                CustInfo.lSavID   = __get_SavingsAccountID(clClass, e);
          } else {
             CustInfo.bResult = FALSE; 
          } /* endif */
          WinPostMsg(hwndAdd,USR_ADD_DONE  , (MPARAM) 0L , (MPARAM) 0L);
          break;
  
      case INQUIRE:
          if (_restoreCustomer(clClass, e, CustInfo.lCustID)==0){
             strcpy(CustInfo.cName,__get_CustomerName(clClass,e));
             strcpy(CustInfo.cBranch,__get_CustomerBranchID(clClass,e));
             CustInfo.lCheckID = __get_CheckingAccountID(clClass,e);
             CustInfo.lSavID = __get_SavingsAccountID(clClass,e);
             CustInfo.dSav = CustInfo.dCheck = 0;
             if (CustInfo.lCheckID) 
                _inquire(clClass,e,CustInfo.lCheckID,CHECKING,&CustInfo.dCheck);
             if (CustInfo.lSavID) 
                _inquire(clClass,e,CustInfo.lSavID,SAVINGS ,&CustInfo.dSav);
             CustInfo.bResult = TRUE; 
          } else {
             CustInfo.bResult = FALSE; 
          } /* endif */
          WinPostMsg(hwndDel,USR_DEL_FINDDONE  , (MPARAM) 0L , (MPARAM) 0L);
          break;
  
      case DELETE:
          if ( _deleteCustomer(clClass,e,CustInfo.lCustID) ) {
             CustInfo.bResult = FALSE;  // customer not found
          } else {
             CustInfo.bResult = TRUE; 
          } /* endif */
          WinPostMsg(hwndDel,USR_DEL_DONE  , (MPARAM) 0L , (MPARAM) 0L);
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

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
SHORT  DebugPrintf(PCHAR szFormat, ...)
{
   static CHAR chBuffer[1024];
   SHORT  sLength;
   va_list pArguments;

   va_start(pArguments, szFormat);
   sLength = vsprintf(chBuffer, szFormat, pArguments);
   va_end(pArguments);

   WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, chBuffer,
                 "Debug", 0, MB_OK | MB_MOVEABLE);

   return sLength;
}

// нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн
void DisplayLastError(HAB hab)
{
CHAR chBuffer[300];
PERRINFO  pErrInfoBlk,pLastErrInfoBlk;
PSZ       pszOffSet, pszErrMsg;

   if ((pErrInfoBlk = WinGetErrorInfo(hab)) != (PERRINFO)NULL) {
     /* Get error info and write to log */
     pLastErrInfoBlk = pErrInfoBlk;
     pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
     pszErrMsg = ((PSZ)pErrInfoBlk) + *(PSHORT)pszOffSet;
     sprintf (chBuffer, "Error ID:  %X\n\nError Msg:\n  %s\n",
                      ERRORIDERROR(pErrInfoBlk->idError),pszErrMsg);

     WinMessageBox( HWND_DESKTOP,HWND_DESKTOP,chBuffer,
                    "LAST ERROR IN THIS THREAD",0,MB_OK);
     /* Free memory allocated by WinGetErrorInfo. */
     WinFreeErrorInfo (pErrInfoBlk);
   }
}

#ifdef NOSOM
LONG ClientClassNew()
{
  DosSleep(2000L);
  return 0L;
}

VOID _somFree(LONG clClass)
{
}

LONG *somGetGlobalEnvironment()
{
   return NULL;
}

BOOL  _restoreCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode)
{
   DosSleep( 500L);
   if (ulCustomerCode == ulCustID) {
      return FALSE;
   } else {
      return TRUE;
   } /* endif */
}

LONG __get_CheckingAccountID(LONG clClass,LONG *e)
{
   if (bChe) {
      return 1L;
   } else {
      return 0L;
   } /* endif */
}

LONG __get_SavingsAccountID(LONG clClass,LONG *e)
{
   if (bSav) {
      return 2L;
   } else {
      return 0L;
   } /* endif */
}

VOID _inquire (LONG clClass,LONG *e,LONG  cAccountID,SHORT type,double *dAmount)
{
     if (type == SAVINGS) {
        if (cAccountID == 2L) 
           *dAmount = dSav;
     } else {
        if (cAccountID == 1L) 
           *dAmount = dChe;
     } /* endif */
}

VOID  _createCheckingAccount(LONG clClass,LONG *e, double Input)
{
  DosSleep( 100L);
  dChe == Input;
  bChe = TRUE;
}

VOID  _createSavingsAccount(LONG clClass,LONG *e, double Input)
{
  DosSleep( 100L);
  dSav == Input;
  bSav = TRUE;
}

LONG  _createCustomer(LONG clClass,LONG *e, CHAR *Name,CHAR *Branch,LONG *CustomerPIN)
{
   ulCustID = 1234;
   strcpy(cBranch,Branch);
   strcpy(cName,Name);
   *CustomerPIN = 1234;
   return 1;
}
BOOL  _deleteCustomer(LONG clClass,LONG *e,ULONG ulCustomerCode)
{
  *cBranch = 0;
  *cName = 0;
  ulCustID = 0;
  bChe = FALSE;
  bSav = FALSE;
}
LONG  __get_CustomerID(LONG clClass,LONG *e)
{
  return ulCustID;
}
PCHAR __get_CustomerName(LONG clClass,LONG *e)
{
  return cName;
}
PCHAR __get_CustomerBranchID(LONG clClass,LONG *e)
{
  return cBranch;
}
#endif


