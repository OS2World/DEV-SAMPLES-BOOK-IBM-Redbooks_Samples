/*****************************************************************************/
/*                                                                           */
/*  PROGRAM NAME:  DROPINFO                                                  */
/*                                                                           */
/*  PURPOSE:       This program provides information for testing and         */
/*                 debugging drag-and-drop applications.  When an object is  */
/*                 dropped over this program's window, the window shows the  */
/*                 information received from the source.                     */
/*                                                                           */
/*****************************************************************************/

#define INCL_WIN
#define INCL_GPI

#define INCL_WINSTDDRAG
#include <os2.h>                                 /* PM header file           */
#include "dropinfo.h"                            /* Application header file  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*****************************************************************************/
/* Window procedure prototypes                                               */
/*****************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpSubList( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void PutMsg(HWND hwnd, char *type, MPARAM mp1, MPARAM mp2);
void PutDInfo(HWND hwnd, PDRAGINFO pdinfo);
void PutDItem(HWND hwnd, PDRAGITEM pditem);

/*****************************************************************************/
/* Global data                                                               */
/*****************************************************************************/
HAB  hAB;                                        /* Anchor block handle      */
PFNWP pwpList;                                   /* Listbox winproc address  */

/*****************************************************************************/
/* Application main routine                                                  */
/*****************************************************************************/
INT main (VOID)
{
  HMQ  hMsgQ;                                    /* Message queue handle     */
  QMSG qMsg;                                     /* Message structure        */
  HWND hFrame;                                   /* Frame window handle      */

  ULONG flCreate = FCF_STANDARD & ~FCF_MENU;     /* Frame creation flags     */

  ULONG rc;                                      /* Return code              */

  hAB = WinInitialize(0);                        /* Register application     */

  hMsgQ = WinCreateMsgQueue(hAB, 0);             /* Create message queue     */

  rc = WinRegisterClass(hAB,                     /* Register window class    */
                        (PSZ)"MyWindow",         /* Class name               */
                        (PFNWP)MyWindowProc,     /* Window procedure address */
                        CS_SIZEREDRAW,           /* Class style              */
                        sizeof(PVOID));          /* Window words             */

  hFrame = WinCreateStdWindow(HWND_DESKTOP,      /* Desktop is parent        */
                              0,                 /* Standard window style    */
                              &flCreate,         /* Frame control flags      */
                              "MyWindow",        /* Window class name        */
                              "DropInfo Sample", /* Window title text        */
                              0,                 /* No special class style   */
                              (HMODULE)0L,       /* Resources in EXE file    */
                              ID_WINDOW,         /* Frame window identifier  */
                              NULL);             /* No pres params           */

  while (WinGetMsg(hAB, &qMsg, 0L, 0, 0))        /* Process messages until   */
        WinDispatchMsg(hAB, &qMsg);              /* WM_QUIT received         */

  WinDestroyWindow(hFrame);                      /* Destroy window           */
  WinDestroyMsgQueue(hMsgQ);                     /* Destroy message queue    */
  WinTerminate(hAB);                             /* Deregister application   */
}

/*****************************************************************************/
/* Main window procedure                                                     */
/*****************************************************************************/
MRESULT EXPENTRY MyWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SWP   swp;
  ULONG rc;

  HWND  hFrame;
  HWND  hListBox;                                /* Listbox window handle    */

  switch (msg)
         {
         case WM_CREATE:
              hListBox = WinCreateWindow(hwnd,
                                         WC_LISTBOX,
                                         NULL,
                                         WS_VISIBLE     |
                                         LS_NOADJUSTPOS |
                                         LS_HORZSCROLL,
                                         0, 0, 0, 0,
                                         hwnd,
                                         HWND_TOP,
                                         ID_LISTBOX,
                                         0,
                                         0);

              pwpList = WinSubclassWindow(hListBox,
                           wpSubList);

              hFrame = WinQueryWindow(hwnd,
                                      QW_PARENT);
              rc = WinSetWindowPos(hFrame,
                                   HWND_TOP,
                                   0, 0,
                                   400, 300,
                                   SWP_SIZE     |
                                   SWP_ACTIVATE |
                                   SWP_SHOW);
              break;

         case WM_SIZE:
              WinQueryWindowPos(hwnd, &swp);
              hListBox=WinWindowFromID(hwnd,
                                       ID_LISTBOX);
              WinSetWindowPos(hListBox,
                              HWND_TOP,
                              swp.x, swp.y,
                              swp.cx, swp.cy,
                              SWP_SIZE  |
                              SWP_SHOW);
              break;

         default:
              return(WinDefWindowProc(hwnd,
                                      msg,
                                      mp1,
                                      mp2));
         }
  return((MRESULT)FALSE);
}

/*****************************************************************************/
/* Listbox subclass window procedure                                         */
/*****************************************************************************/
MRESULT EXPENTRY wpSubList(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  PDRAGINFO pDInfo;                              /* Pointer to DRAGINFO      */
  PDRAGITEM pDItem;                              /* Pointer to DRAGITEM      */
  ULONG rc;                                      /* Return code              */
  int i;                                         /* Loop counter             */

  switch (msg)
         {
         case DM_DRAGOVER:
              pDInfo = (PDRAGINFO)mp1;
              rc = DrgAccessDragInfo(pDInfo);
              pDItem = DrgQueryDragitemPtr(pDInfo, 0);
              return(MPFROM2SHORT(DOR_DROP,      /* Drop Allowed             */
                                  DO_UNKNOWN));
              break;

         case DM_DROP:                           /* Object being dropped     */
              PutMsg(hwnd,"DM_DROP",mp1,mp2);    /* Put message in listbox   */
              pDInfo = (PDRAGINFO)mp1;           /* Extract DRAGINFO pointer */
              rc =DrgAccessDragInfo(pDInfo);     /* Access DRAGINFO data     */
              PutDInfo(hwnd, pDInfo);            /* Put DRAGINFO in listbox  */

              pDItem = DrgQueryDragitemPtr(pDInfo, 0); /* Get DRAGITEMs      */

              for (i=0; i < pDInfo->cditem; i++) /* For each DRAGITEM        */
                  {
                  PutDItem(hwnd, pDItem++);      /* Put DRAGITEM in listbox  */
                  }
              break;

         default:                                /* All other messages       */
              return((MRESULT)pwpList(hwnd,      /* Invoke default listbox   */
                                      msg,       /* window procedure         */
                                      mp1,
                                      mp2));
              break;
         }
}

/*****************************************************************************/
/* Put message info in listbox item                                          */
/*****************************************************************************/
void PutMsg(HWND hwnd, char *type, MPARAM mp1, MPARAM mp2)
{
  char buf[100];                                 /* Message buffer           */

  sprintf(buf,                                   /* Copy info to buffer      */
          "%s %08X, %08x",
          type,
          mp1,
          mp2);
  WinSendMsg(hwnd,                               /* Set listbox top index    */
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,            /* Position of new item     */
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);
  return;
}

/*****************************************************************************/
/* Put DRAGINFO structure contents in listbox item                           */
/*****************************************************************************/
void PutDInfo(HWND hwnd, PDRAGINFO pDInfo)
{
  char buf[500];                                 /* Buffer                   */
  sprintf(buf,                                   /* Copy info to buffer      */
          "   DRAGINFO: size: %d(DragInfo), %d(DragItem), \n   ---Ops: %d, hwndSource: %08X, drop coords: (%d, %d), cdItem cnt: %d",
          pDInfo->cbDraginfo,
          pDInfo->cbDragitem,
          pDInfo->usOperation,
          pDInfo->hwndSource,
          pDInfo->xDrop,
          pDInfo->yDrop,
          pDInfo->cditem);

  WinSendMsg(hwnd,                               /* Set listbox top index    */
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,            /* Position of new item     */
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);
  return;
}

/*****************************************************************************/
/* Put DRAGITEM structure contents in listbox item                           */
/*****************************************************************************/
void PutDItem(HWND hwnd, PDRAGITEM pDItem)
{
  char buf[500],                                 /* Buffers                  */
       srceType[100],
       srceContainer[100],
       srceName[100],
       srceRMF[100],
       targetName[100];

  DrgQueryStrName(pDItem->hstrSourceName,        /* Get info from DRAGITEM   */
                  100,
                  srceName);
  DrgQueryStrName(pDItem->hstrContainerName,
                  100,
                  srceContainer);
  DrgQueryStrName(pDItem->hstrType,
                  100,
                  srceType);
  DrgQueryStrName(pDItem->hstrRMF,
                  100,
                  srceRMF);
  DrgQueryStrName(pDItem->hstrTargetName,
                  100,
                  targetName);

  sprintf(buf,                                   /* Copy info to buffer      */
          "   DRAGITEM: ulItemID: %d",
          pDItem->ulItemID);
  WinSendMsg(hwnd,                               /* Set listbox top index    */
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,            /* Position of new item     */
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --Type:      \"%s\"",
          srceType);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --RMF:       \"%s\"",
          srceRMF);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --Container: \"%s\"",
          srceContainer);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --SourceName:      \"%s\"",
          srceName);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --TargetName:      \"%s\"",
          targetName);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);

  sprintf(buf,
          "   --Offset: (%d, %d), cntl: %d, ops: %d",
          pDItem->cxOffset,
          pDItem->cyOffset,
          pDItem->fsControl,
          pDItem->fsSupportedOps);
  WinSendMsg(hwnd,
             LM_SETTOPINDEX,
             (MPARAM)WinSendMsg(hwnd,
                                LM_INSERTITEM,
                                (MPARAM)LIT_END,
                                MPFROMP(buf)),
             NULL);
  return;
}
