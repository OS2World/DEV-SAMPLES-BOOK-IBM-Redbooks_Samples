//****************************************************************************************
//
//  DMORDER.C - a demo program written by Franco Federico and Alan Chambers
//              whilst on a residency at the ITSC, Boca Raton
//
//  Updated to work with GA code, 6 May 1992
//
//  This program will display an order form when the appropriate action bar selection
//  is made.  Customers, dragged from the DMCUST sample program, can be dropped onto
//  this order form, with the result that the customer's details are entered into
//  the appropriate fields.
//
//  The code for the window and dialog box, and the technique used for subclassing
//  were written by Franco, the drag/drop code added by Alan.
//
//**************************************************************************************** */

#define INCL_GPI
#define INCL_DOS
#define INCL_WIN

#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <process.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <bsememf.h>

#define WC_NAME_FRAME            "#1"
#define WC_NAME_COMBOBOX         "#2"
#define WC_NAME_BUTTON           "#3"
#define WC_NAME_MENU             "#4"
#define WC_NAME_STATIC           "#5"
#define WC_NAME_ENTRYFIELD       "#6"
#define WC_NAME_LISTBOX          "#7"
#define WC_NAME_SCROLLBAR        "#8"
#define WC_NAME_TITLEBAR         "#9"
#define WC_NAME_MLE              "#10"
#define WC_NAME_APPSTAT          "#16"
#define WC_NAME_KBDSTAT          "#17"
#define WC_NAME_PECIC            "#18"
#define WC_NAME_DBE_KKPOPUP      "#19"
#define WC_NAME_SPINBUTTON       "#32"
#define WC_NAME_CONTAINER        "#37"
#define WC_NAME_SLIDER           "#38"
#define WC_NAME_VALUESET         "#39"
#define WC_NAME_NOTEBOOK         "#40"

#define DRAGXFERMEMNAME  "\\SHAREMEM\\DMORDXFER.DAT"

#include "dmorder.h"
#include "dmorddlg.h"


#define DebugBox(title, text)  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP, \
                               (PSZ) text , (PSZ) title, 0, \
                               MB_OK | MB_INFORMATION )

typedef struct {
                 char name[30];
                 char address[100];
                 char phone[15];
               } 
                 CUSTOMER, *PCUSTOMER;


typedef struct _SUBCLASSDATA {
        HWND  hwnd;
        PFNWP pSubWinProc;
        PFNWP pPreviousWinProc;
        CHAR  szClassName[40];
        HWND  hwndSurrogate;
        PVOID pWindowData;
        BOOL  fEmphasis;
        struct _SUBCLASSDATA *pNext;
       } 
         SUBCLASSDATA, *PSUBCLASSDATA;

MRESULT EXPENTRY OrderDlgProc(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY OrderSubWinProc(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY MainWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

PSUBCLASSDATA GetSubClassData(HWND hwnd, PFNWP pSubWinProc);
VOID SubClassWindow(HWND hwnd, PFNWP pSubWinProc, PVOID pData, HWND  hwndSurrogate );
VOID FreeUnwantedSubClassData(VOID);
VOID DrawEmphasis(HWND hwnd);

HWND hwndCliArea;
HWND hwndMLEWindow;

HAB  hab;
CHAR szMsgString[200];

PSUBCLASSDATA pSubClassDataFirst = NULL;
PSUBCLASSDATA pSubClassDataLast  = NULL;

main ()
{
  HMQ  hmq;
  HWND hwndFrame;
  QMSG qmsg;
  ULONG flCreate;

  hab = WinInitialize(0);

  hmq = WinCreateMsgQueue( hab, 0 );

  WinRegisterClass( hab, (PSZ)"MainWindow",
                        (PFNWP)MainWindowProc,
                        CS_SIZEREDRAW, 0 );

  flCreate = FCF_STANDARD & ~FCF_ACCELTABLE & ~FCF_ICON;

  hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0,
                                  &flCreate, "MainWindow",
                                  "Order Form Sample", 0,
                                  (HMODULE)0L, ID_FRAME_WND,
                                  &hwndCliArea ) ;

  strcpy(szMsgString,"To take an order, select \"File\"/\"Take order\".");


  while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
     WinDispatchMsg( hab, &qmsg );


  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
}



MRESULT EXPENTRY MainWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{

  switch( msg )
  {
      case WM_CREATE:
      {
        WinSetWindowPos( WinQueryWindow(hwnd, QW_PARENT),  HWND_TOP, 150, 250, 400, 120,
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);
      }
      break;


      case WM_COMMAND:
       {
         switch (SHORT1FROMMP(mp1))
         {
           case MID_TAKE_ORDER:

                WinDlgBox(HWND_DESKTOP,
                          HWND_DESKTOP,
                          OrderDlgProc,
                          0L,
                          ID_DLG_ORDERFORM,
                          (PVOID)NULL);

                FreeUnwantedSubClassData();

             break;

           case MID_OTHER:

             WinPostMsg(WinQueryWindow(HWND_DESKTOP,QW_PREV), WM_COMMAND, MPFROMSHORT(705) ,(MPARAM)0);
             DosBeep(300,200);
             WinPostMsg(WinQueryWindow(HWND_DESKTOP,QW_PREV), WM_USER+1659, MPFROMSHORT(705) ,(MPARAM)0);
             DosBeep(300,200);
             WinPostMsg(WinQueryWindow(HWND_DESKTOP,QW_PREV), WM_USER+1660, MPFROMSHORT(705) ,(MPARAM)0);
             DosBeep(300,200);

             break;

           default:
             return WinDefWindowProc( hwnd, msg, mp1, mp2 );
         }
      }
      break;

    case WM_PAINT:
      {
         HPS    hps;
         RECTL  rc;

         hps = WinBeginPaint( hwnd, 0L, &rc );

         WinFillRect(hps, &rc, CLR_WHITE);
         WinQueryWindowRect(hwnd,&rc);

         WinDrawText(hps, (LONG)strlen( szMsgString ),
                     szMsgString, &rc, CLR_RED,
                     CLR_WHITE, DT_CENTER | DT_VCENTER);

         WinEndPaint( hps );
      }
      break;

    default:
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)FALSE;
}


MRESULT EXPENTRY OrderDlgProc(HWND hwndDlg, ULONG msg,
                                  MPARAM mp1, MPARAM mp2)
{

 switch (msg)
 {
    case WM_USER+1:
    /* Subclass the dialog and all it's children */
       {
          HWND   hwndChild;       /* current dialog child      */
          HENUM  henum;           /* enumeration handle        */

          SubClassWindow( hwndDlg, OrderSubWinProc, NULL , (HWND)0 );

          henum = WinBeginEnumWindows(hwndDlg);

          while ( (hwndChild = WinGetNextWindow(henum)) )
          {
             SubClassWindow( hwndChild, OrderSubWinProc, NULL, hwndDlg);
          }
       }
       break;

    case WM_INITDLG:
       WinPostMsg(hwndDlg, WM_USER+1, (MPARAM)0, (MPARAM)0 );
       break;

    case WM_COMMAND:
       {
          switch (SHORT1FROMMP(mp1))
          {
            case DID_OK:
              {
                 //
              }
              return (MRESULT) TRUE;
              break;

            case DID_CANCEL:
              WinDismissDlg(hwndDlg,DID_CANCEL);
              break;
          }
       }
       return (MRESULT) TRUE;
 }
 return (WinDefDlgProc(hwndDlg, msg, mp1, mp2) );
}



VOID SubClassWindow(HWND hwnd, PFNWP pSubWinProc, PVOID pData, HWND  hwndSurrogate )
{
   PFNWP  pOldWinProc;   /*Old window procedure */

   PSUBCLASSDATA pSubClassData;

   pOldWinProc = WinSubclassWindow(hwnd, pSubWinProc);

   /* allocate our subclass data and fill it  */

   pSubClassData = (PSUBCLASSDATA) malloc( sizeof(SUBCLASSDATA) );

   pSubClassData->hwnd = hwnd;
   pSubClassData->pPreviousWinProc = pOldWinProc;
   pSubClassData->pWindowData = pData;
   pSubClassData->hwndSurrogate = hwndSurrogate;
   pSubClassData->fEmphasis = FALSE;
   pSubClassData->pSubWinProc = pSubWinProc;

   WinQueryClassName(hwnd,
                     sizeof(pSubClassData->szClassName),
                     pSubClassData->szClassName);

   pSubClassData->pNext = NULL;

   /* Now add to our linked list */

   if(pSubClassDataLast == NULL)
   {
      pSubClassDataFirst = pSubClassData;
      pSubClassDataLast  = pSubClassData;
   }
   else
   {
      pSubClassDataLast->pNext = pSubClassData;
      pSubClassDataLast = pSubClassData;
   }
}

PSUBCLASSDATA GetSubClassData(HWND hwnd, PFNWP pSubWinProc)
{
   PSUBCLASSDATA pSubClassData;

   pSubClassData = pSubClassDataFirst;

   for(;;)
   {
      if( pSubClassData->hwnd == hwnd && pSubClassData->pSubWinProc == pSubWinProc)
      {
         break; /* Found a hwnd match */
      }

      pSubClassData = pSubClassData->pNext;

      if( pSubClassData == NULL)
      {
         break; /* Not found returns NULL */
      }
   }

   return (pSubClassData);
}

VOID FreeUnwantedSubClassData(VOID)
{

   PSUBCLASSDATA pSubClassData;
   PSUBCLASSDATA pDeleteable;

   if(pSubClassDataFirst != NULL)
   {
      pSubClassData = pSubClassDataFirst;

      for(;;)
      {
         if( pSubClassData->pNext == NULL)
         {
            pSubClassDataLast = pSubClassData;
            break;
         }

         if(! WinIsWindow((HAB)0, (pSubClassData->pNext)->hwnd ) )
         {
            /* Window no longer exists so let us delete this entry   */
            /* To keep the simple the first entry never gets deleted */
            pDeleteable = pSubClassData->pNext;
            pSubClassData->pNext = pDeleteable->pNext;

            free(pDeleteable);
         }
         else
         {
            pSubClassData = pSubClassData->pNext;
         }
      }
   }
}



MRESULT EXPENTRY OrderSubWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  PSUBCLASSDATA pSubClassData;
  PDRAGINFO pdinfo;
  PDRAGITEM pditem;
  PDRAGTRANSFER pdxfer;
  PCUSTOMER pxfercust;
  ULONG rc;

  pSubClassData = GetSubClassData(hwnd, OrderSubWinProc );


  switch( msg )
  {
    case DM_DRAGLEAVE:
       {
          if(pSubClassData->hwndSurrogate)
          {
             break;
          }
          else
          {
             DrawEmphasis(hwnd);
             pSubClassData->fEmphasis = FALSE;
             return ((MRESULT)(*(pSubClassData->pPreviousWinProc))(hwnd, msg, mp1, mp2));
          }
       }
       break;

    case DM_DRAGOVER:
       {
          if(pSubClassData->hwndSurrogate)
          {
             return( WinSendMsg(pSubClassData->hwndSurrogate, msg, mp1, mp2) );
          }
          else
          {
             if( pSubClassData->fEmphasis == FALSE)
             {
                DrawEmphasis(hwnd);
                pSubClassData->fEmphasis = TRUE;
             }

             pdinfo = (PDRAGINFO)mp1;
             DrgAccessDraginfo(pdinfo);
             pditem = DrgQueryDragitemPtr(pdinfo, 0);
             if(DrgVerifyRMF(pditem, "DRM_SHAREMEM", "DRF_TEXT"))
             {
                DrgFreeDraginfo(pdinfo);
                return(MRFROM2SHORT(DOR_DROP, DO_COPY));
             }
             else
             {
               DrgFreeDraginfo(pdinfo);
               return(MPFROM2SHORT(DOR_NEVERDROP, 0));
             }
             return ((MRESULT)(*(pSubClassData->pPreviousWinProc))(hwnd, msg, mp1, mp2));

          }
       }
       break;

   case DM_DROP:
       {
          if(pSubClassData->hwndSurrogate)
          {
             return( WinSendMsg(pSubClassData->hwndSurrogate, msg, mp1, mp2) );
          }
          else
          {
             pdinfo = (PDRAGINFO)mp1;
             DrgAccessDraginfo(pdinfo);
             pditem = DrgQueryDragitemPtr(pdinfo, 0);
             
             rc = DosAllocSharedMem((PPVOID)&pxfercust, DRAGXFERMEMNAME, sizeof(CUSTOMER), PAG_COMMIT | PAG_WRITE | PAG_READ);
          
             pdxfer = DrgAllocDragtransfer(1);      
          
             pdxfer->cb = sizeof(DRAGTRANSFER);
             pdxfer->hwndClient = hwnd;
             pdxfer->pditem = pditem;
             pdxfer->hstrSelectedRMF = DrgAddStrHandle("DRM_CUSTOMER");
             pdxfer->hstrRenderToName = DrgAddStrHandle(DRAGXFERMEMNAME);
             pdxfer->ulTargetInfo = 0;
             pdxfer->usOperation = DO_COPY;
          
             rc = (ULONG)DrgSendTransferMsg(pdinfo->hwndSource, DM_RENDER, (MPARAM)pdxfer, NULL);
          
             if(rc == TRUE)
             {       
                 WinSetWindowText(WinWindowFromID(hwnd, ID_EF_NAME), pxfercust->name);
                 WinSetWindowText(WinWindowFromID(hwnd, ID_EF_ADDRESS), pxfercust->address);
                 WinSetWindowText(WinWindowFromID(hwnd, ID_EF_TELEPHONE), pxfercust->phone);
                 DosBeep(2000,20); DosSleep(20);DosBeep(2000,20); DosSleep(20); DosBeep(2000,20); DosSleep(20); DosBeep(2000,20); DosSleep(20); 
          
             }
          
          
             DrgFreeDraginfo(pdinfo);
             DrgFreeDragtransfer(pdxfer);
             DosFreeMem((PVOID)pxfercust);
           }
       }
       break;           

    default:
      return ((MRESULT)(*(pSubClassData->pPreviousWinProc))(hwnd, msg, mp1, mp2));
      break;
  }
  return (MRESULT)FALSE;

}

VOID DrawEmphasis(HWND hwnd)
{
   RECTL  rc;
   POINTL ptl;
   HPS    hps;

   hps = DrgGetPS(hwnd);

   WinQueryWindowRect(hwnd,&rc);

   GpiSetMix(hps, FM_XOR);
   GpiSetColor(hps, CLR_BACKGROUND);

   ptl.x = rc.xLeft + 4L; ptl.y = rc.yBottom  + 4L;
   GpiMove(hps, &ptl);
   ptl.x = rc.xLeft + 4L; ptl.y = rc.yTop  - 4L;
   GpiLine(hps, &ptl);
   ptl.x = rc.xRight - 4L; ptl.y = rc.yTop  - 4L;
   GpiLine(hps, &ptl);
   ptl.x = rc.xRight - 4L; ptl.y = rc.yBottom  + 4L;
   GpiLine(hps, &ptl);
   ptl.x = rc.xLeft + 4L; ptl.y = rc.yBottom  + 4L;
   GpiLine(hps, &ptl);

   DrgReleasePS(hps);
}

