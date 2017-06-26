//**************************************************************************************** */
//
//  DMPHONE.C - a demo program by Alan Chambers - UK PS Technical Support, on residency
//             at the ITSC, Boca Raton
//
//  Modified 6 May 1992 to work with GA code.
//
//
//  This program is designed to be used with the DMCUST sample program.  A customer
//  can be dragged onto the telephone icon displayed by this program, with the result
//  that a series of beeps, reminiscent of touchtone dialling tones, are sounded.
//
//  The program illustrates the use of a private rendering mechanism, DRM_SHAREMEM, from
//  the target's point of view.
//
//**************************************************************************************** */
//
//  The program displays a telephone icon as using the WC_STATIC class, then subclasses
//  this static window to add function for drag/drop, moving the icon around the desktop,
//  and providing a context (pop-up) menu.
//
/**************************************************************************************** */


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>                        /* PM header file               */
#include "dmphone.h"                      /* Resource symbolic identifiers*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>


#define OBJECTDATASIZE 100
#define DRAGXFERMEMNAME "\\SHAREMEM\\Drgphone.mem"


typedef struct {
                 char name[30];
                 char address[100];
                 char phone[15];
                 int  credit_limit;
               } 
                 CUSTOMER, *PCUSTOMER;

typedef struct{
                RECORDCORE core;
                CUSTOMER cust;
              }
                CONTRECORD, *PCONTRECORD;

MRESULT EXPENTRY PhoneSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void mfbeep(ULONG, ULONG);
void phonedial(PCHAR phone);


HAB  hab;          
HPOINTER hptrPostIcon, hptrArrow;
PFNWP StaticWinProc;
HWND hwndPhone,
     hwndMenu;

INT main (VOID)
{ 
  HMQ  hmq;                             /* Message queue handle         */
  QMSG qmsg;                            /* Message from message queue   */

  hab = WinInitialize(0);

  hmq = WinCreateMsgQueue( hab, 0 );

  hptrPostIcon = WinLoadPointer(HWND_DESKTOP, 0, ID_MYPOINTER);
  hptrArrow = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);

  hwndPhone = WinCreateWindow(HWND_DESKTOP, WC_STATIC, "#9999",
                                        WS_VISIBLE | SS_ICON,
                                        10, 10, 64, 64, 
                                        HWND_DESKTOP, HWND_TOP, ID_PHONEICON,
                                        0, 0);

  StaticWinProc = WinSubclassWindow(hwndPhone, PhoneSubclassProc);

  hwndMenu =  WinLoadMenu(hwndPhone, (ULONG)NULL, RESID_MENU);


  while(WinGetMsg(hab, &qmsg, 0L, 0, 0))       // message processing loop
     WinDispatchMsg(hab, &qmsg);



  WinDestroyMsgQueue( hmq );             
  WinTerminate( hab );                   
} 



MRESULT EXPENTRY PhoneSubclassProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  SWP swp;
  PDRAGTRANSFER pdxfer;
  PCUSTOMER pxfercust;
  PDRAGINFO pdinfo;
  PDRAGITEM pditem;
  ULONG rc;
  static char IconText[20] = "Icon #";
  char curDir[100];
  char msgtext[100];
  static BOOL button2down = FALSE;
  static POINTS oldpos, newpos;
  static BOOL moved = FALSE;
                    

  switch( msg )
  {
     case DM_DRAGOVER:
       pdinfo = (PDRAGINFO)mp1; 
       DrgAccessDraginfo(pdinfo);
       pditem = DrgQueryDragitemPtr(pdinfo, 0);
       if(DrgVerifyRMF(pditem, "DRM_SHAREMEM", "DRF_TEXT"))
       {
         DrgFreeDraginfo(pdinfo);
         return(MPFROM2SHORT(DOR_DROP, DO_COPY));
       }
       else
       {
         DrgFreeDraginfo(pdinfo);
         return(MPFROM2SHORT(DOR_NEVERDROP, 0));
       }
       break;


     case DM_DROP:
       pdinfo = (PDRAGINFO)mp1;
       DrgAccessDraginfo(pdinfo);
       pditem = DrgQueryDragitemPtr(pdinfo, 0);
       
       // allocated a named shared memory object for a customer *pxfercust

       rc = DosAllocSharedMem((PPVOID)&pxfercust, DRAGXFERMEMNAME, sizeof(CUSTOMER), PAG_COMMIT | PAG_WRITE | PAG_READ);

       // allocate and fill the dxfer structure;

       pdxfer = DrgAllocDragtransfer(1);      

       pdxfer->cb = sizeof(DRAGTRANSFER);
       pdxfer->hwndClient = hwnd;
       pdxfer->pditem = pditem;
       pdxfer->hstrSelectedRMF = DrgAddStrHandle("<DRM_CUSTOMER,DRF_TEXT>");
       pdxfer->hstrRenderToName = DrgAddStrHandle(DRAGXFERMEMNAME);
       pdxfer->ulTargetInfo = 0;
       pdxfer->usOperation = DO_COPY;
 
       rc = (ULONG)DrgSendTransferMsg(pdinfo->hwndSource, DM_RENDER, (MPARAM)pdxfer, NULL);

       if(rc == TRUE)
       {       
          phonedial(pxfercust->phone);
       }


       DrgFreeDraginfo(pdinfo);
       DrgFreeDragtransfer(pdxfer);
       DosFreeMem((PVOID)pxfercust);

       break;           

    //*****************************************************************************
    // The next three cases are to detect a user's request for the context menu
    // If he presses button 2, then releases it without moving the mouse
    // the context menu is displayed.  Pressing button 2 and moving before releasing
    // is the way the telephone can be moved about the screen.
    //*****************************************************************************
  
    case WM_BUTTON2DOWN:
       button2down = TRUE;
       moved = FALSE;
       memcpy((PVOID)&oldpos, (PVOID)&mp1, sizeof(POINTS));     //save mouse pos.
       break;

    case WM_BUTTON2UP:
       button2down = FALSE;
       if(moved)
       {
         WinSetPointer(HWND_DESKTOP, hptrArrow);
         WinSetCapture(HWND_DESKTOP, (HWND)NULL);
         memcpy((PVOID)&newpos, (PVOID)&mp1, sizeof(POINTS));     //save mouse pos.
         WinQueryWindowPos(hwndPhone, &swp);
         WinSetWindowPos(hwndPhone, HWND_TOP, swp.x + (newpos.x - oldpos.x),
                                              swp.y + (newpos.y - oldpos.y), 
                         swp.cx, swp.cy, SWP_MOVE);       
       }
       else
       {  
         rc = WinPopupMenu(hwndPhone, hwndPhone, hwndMenu, 0, 0, 0, 
                     PU_MOUSEBUTTON1 /*| PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN*/ );
       }
    
       break;

    case WM_MOUSEMOVE:
       if(button2down)
       {
         if(!moved)
         {
           WinSetPointer(HWND_DESKTOP, hptrPostIcon);
           WinSetCapture(HWND_DESKTOP, hwndPhone);
         }
         moved = TRUE;
       }
       break;
       
    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1))
      {
        case ID_EXITPROG:
          WinPostMsg(hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0);
          break;
      }
      break;  

    case WM_CLOSE:
      WinPostMsg(hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0);     
      break;
   
    default:
      return (MRESULT)StaticWinProc(hwnd, msg, mp1, mp2);
  }
  return (MRESULT)FALSE;
} 


//****************************************************************************
//
//  The following two functions implement the phone dialling functions.  
//  Unfortunately, OS/2 does not support the production of multiple concurrent
//  tones on the PC speaker.  Since a touchtone phone generates a pair of tones
//  for each number, we can't reproduce this accurately.  For this sample program
//  therefore, all we do is rapidly alternate the two tones in each case.  The
//  frequencies used are correct.
//
//****************************************************************************

void phonedial(PCHAR phone)
{  
  char *p;
  ULONG num;
  int hi, lo;

  for(p=phone; p != '\0' && (p-phone) < 8; p++)
  {
    if(*p >= '0' && *p <= '9')
    {
      num = (*p - '0');

      if(num == 1 || num == 2 || num == 3)
        lo = 697;
      else if(num == 4 || num == 5 || num == 6)
        lo = 770;
      else if(num == 7 || num == 8 || num == 9)
        lo = 852;
      else
        lo = 941;    // zero

      if(num == 1 || num == 4 || num == 7)
        hi = 1209;
      else if(num == 2 || num == 5 || num == 8)
        hi = 1336;
      else if(num == 3 || num == 6 || num == 9)
        hi = 1477;
      mfbeep(lo, hi);
    }
  }
}


#define FLIPTIME 10 
void mfbeep(ULONG f1, ULONG f2)
{
  int i;
   
  for(i=1; i<30/FLIPTIME; i++)
  {
     DosBeep(f1, FLIPTIME);
     DosBeep(f2, FLIPTIME);
  }
  DosSleep(50);
}
