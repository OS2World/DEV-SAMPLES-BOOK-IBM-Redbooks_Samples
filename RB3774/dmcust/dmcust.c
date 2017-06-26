//****************************************************************************************
//
//  DMCUST.C - a demo program by Alan Chambers - UK PS Technical Support, whilst on 
//             a residency at the ITSC, Boca Raton
//
//  Updated to work with GA code, 6 May 1992
//
//  This program reads several names, addresses and phone numbers from a file (DMCUST.DAT)
//  and displays them as icons in a container window.  These items can be moved from
//  one instance of this program to another by direct manipulation; they can be dropped
//  on an order form displayed by the DMORDER sample to fill in the customer details therein,
//  and they can be dropped on the telephone icon shown by the DMPHONE sample, to simulate
//  a telephone dialler.
//
//
//  This sample illustrates several different aspects of drag/drop:
//
//      - initiating a drag from a container window
//      - using the DRM_PRINT rendering mechanism to allow application printing by
//        drag/drop to a workplace shell printer object
//      - using the DRM_DISCARD rendering mechanism to allow deletion of application
//        items by dragging them to the workplace shell shredder object
//      - implementing both ends of a user defined rendering mechanism (here called
//        DRM_SHAREMEM)
//
//****************************************************************************************
//
// The program concentrates on demonstrating drag/drop techniques and is therefore
// very simple in other respects. In particular, the so called customers to be displayed
// are read from a simple flat file called DMCUST.DAT that can be read with fscanf().  
// The following lines illustrate the format of this file - note that each text 
/* field must have no imbedded blanks:

Alan_Chambers Somewhere_nice 123-4567
John_Major 10_Downing_Street 999-4444
A.N.Other Somewhere_horrible 987-5432
George_Bush The_Whitehouse 456-8765

******************************************************************************************/


#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>                         
#include "dmcust.h"                      
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>

//***************************************************************************** */
//
//  #defines, typedefs and function headers
//
//***************************************************************************** */



#define OBJECTDATASIZE 100
#define DRAGXFERMEMNAME "\\SHAREMEM\\DragXfer.mem"


typedef struct {
                 char name[30];
                 char address[100];
                 char phone[15];
               } 
                 CUSTOMER, *PCUSTOMER;

typedef struct{
                RECORDCORE core;          // control info for CONTAINER window
                CUSTOMER cust;            // application defined data
              }
                CONTRECORD, *PCONTRECORD;

MRESULT EXPENTRY CustWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
void FillWindow(HWND);
void AddCustomer(HWND, PCUSTOMER, ULONG, ULONG);
void RemoveCustomer(HWND, PCONTRECORD);
void PrintCustomer(PCONTRECORD, PPRINTDEST);



//***************************************************************************** */
//
//  Some global variables
//
//***************************************************************************** */

HAB  hab;                               /* PM anchor block handle           */
HPOINTER hptrCustIcon;                  /* Icon used for dragging customers */

//***************************************************************************** */
//
//  Main procedure - designed to create main (only) window and process messages
//
//***************************************************************************** */

INT main (VOID)
{ 
  HMQ  hmq;                                   /* Message queue handle         */
  QMSG qmsg;                                  /* Message from message queue   */
  HWND hwndMain;
  ULONG flCreate = FCF_STANDARD & ~FCF_MENU;  /* frame window creation flags */

  hab = WinInitialize(0);

  hmq = WinCreateMsgQueue(hab, 0);
 
  hptrCustIcon = WinLoadPointer(HWND_DESKTOP, 0, ID_CUSTPOINTER);

  WinRegisterClass(                       
       hab,                               
        (PSZ)"CustWindow",                
        (PFNWP)CustWindowProc,            
        CS_SIZEREDRAW,                    
        sizeof(PVOID)
        );

  hwndMain = WinCreateStdWindow(
                   HWND_DESKTOP,           
                   0,                      
                   &flCreate,              
                   "CustWindow",           
                   "Customer Window",
                   0,                      
                   (HMODULE)0L,            
                   ID_WINDOW,              
                   NULL
                   );


  while(WinGetMsg(hab, &qmsg, 0L, 0, 0))       // message processing loop
     WinDispatchMsg(hab, &qmsg);


  WinDestroyWindow(hwndMain);
  WinDestroyMsgQueue( hmq );             
  WinTerminate( hab );                   

  return(0);
} 


//***************************************************************************** */
//
//  Window procedure for main (only) window
//
//***************************************************************************** */

MRESULT EXPENTRY CustWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  typedef struct {
                   HWND hwndContainer;
                 } 
                   INSTANCE, *PINSTANCE;

  PINSTANCE wd;
  SWP swp;
  PCONTRECORD pcrec;
  PCNRDRAGINIT pcnrdinit;
  PCNRDRAGINFO pcnrdinfo;
  PDRAGINFO pdinfo;
  DRAGITEM ditem;
  DRAGIMAGE dimg;
  PDRAGTRANSFER pdxfer;
  PCUSTOMER pxfercust, pxferfromcust;
  PDRAGITEM pditem;
  char dragxfermemname[100];
  ULONG rc;
  char srceType[100], srceRMF[100];
  char srceFileName[100], srcePath[100];

  wd = (PINSTANCE)WinQueryWindowPtr(hwnd, 0);    // retrieve instance data pointer

  switch( msg )
  {
     case WM_CREATE:
       wd = (PINSTANCE)malloc(sizeof(INSTANCE));   // allocate instance data space
       WinSetWindowPtr(hwnd, 0, (PVOID)wd);        // and store in window words

       wd->hwndContainer = WinCreateWindow( hwnd, WC_CONTAINER, NULL, 
                                        CCS_AUTOPOSITION | WS_VISIBLE | CCS_EXTENDSEL, 
                                        0, 0, 0, 0, 
                                        hwnd, HWND_TOP, ID_CONTAINER,
                                        0, 0);

       FillWindow(wd->hwndContainer); 
     
       rc = WinSetWindowPos( WinQueryWindow(hwnd, QW_PARENT), 
                      HWND_TOP, 100, 90, 400, 300,
                      SWP_SIZE | SWP_ACTIVATE | SWP_SHOW | SWP_MOVE);
      break;


    case WM_CONTROL:
      switch (SHORT2FROMMP(mp1))
      {
        case CN_INITDRAG:
           pcnrdinit = (PCNRDRAGINIT)mp2;
           if((pcrec = (PCONTRECORD)(pcnrdinit->pRecord)) != NULL)
           {          
              pdinfo = DrgAllocDraginfo(1);

              ditem.hwndItem = hwnd;
              ditem.ulItemID = (ULONG)pcrec;
              ditem.hstrType = DrgAddStrHandle("DRT_CUSTOMER");    
              ditem.hstrRMF = DrgAddStrHandle("(DRM_SHAREMEM,DRM_PRINT,DRM_DISCARD)x(DRF_TEXT)");
              ditem.hstrContainerName = DrgAddStrHandle("Boca Raton Branch");
              ditem.hstrSourceName = DrgAddStrHandle(pcrec->cust.name);
              ditem.hstrTargetName = (HSTR)0;
              ditem.fsControl = 0;  ditem.fsSupportedOps = 0;

              rc = DrgSetDragitem(pdinfo, &ditem, (ULONG)sizeof(ditem), 0);

              dimg.cb = sizeof(DRAGIMAGE);
              dimg.cptl = 0;
              dimg.hImage = hptrCustIcon;
              dimg.sizlStretch.cx = 10L;
              dimg.sizlStretch.cy = 10L;
              dimg.fl = DRG_ICON;
              dimg.cxOffset = 0;
              dimg.cyOffset = 0;

              DrgDrag(hwnd, pdinfo, (PDRAGIMAGE)&dimg, 1, VK_ENDDRAG, NULL); 

              DrgFreeDraginfo(pdinfo);
           }
           break;


        case CN_DRAGOVER:
           pcnrdinfo = (PCNRDRAGINFO)mp2;
           pdinfo = pcnrdinfo->pDragInfo;
           pditem = DrgQueryDragitemPtr(pdinfo, 0);

           if(DrgVerifyRMF(pditem, "DRM_SHAREMEM", "DRF_TEXT"))
           {
             DrgFreeDraginfo(pdinfo);
             return(MPFROM2SHORT(DOR_DROP, DO_MOVE));
           }
           else
           {
             DrgFreeDraginfo(pdinfo);
             return(MPFROM2SHORT(DOR_NEVERDROP, 0));
           }
           break;


        case CN_DROP:
           pcnrdinfo = (PCNRDRAGINFO)mp2;
           pdinfo = pcnrdinfo->pDragInfo;
           pditem = DrgQueryDragitemPtr(pdinfo, 0);
           
           DrgQueryStrName(pditem->hstrSourceName, 100, srceFileName);
           DrgQueryStrName(pditem->hstrContainerName, 100, srcePath);
           DrgQueryStrName(pditem->hstrType, 100, srceType);
           DrgQueryStrName(pditem->hstrRMF, 100, srceRMF);


           // allocated a named shared memory object for a customer *pxfercust

           rc = DosAllocSharedMem((PPVOID)&pxfercust, DRAGXFERMEMNAME, sizeof(CUSTOMER), PAG_COMMIT | PAG_WRITE | PAG_READ);


           // allocate and fill the dxfer structure;

           pdxfer = DrgAllocDragtransfer(1);      

           pdxfer->cb = sizeof(DRAGTRANSFER);
           pdxfer->hwndClient = hwnd;
           pdxfer->pditem = pditem;
           pdxfer->hstrSelectedRMF = DrgAddStrHandle("DRM_CUSTOMER");
           pdxfer->hstrRenderToName = DrgAddStrHandle(DRAGXFERMEMNAME);
           pdxfer->ulTargetInfo = 0;
           pdxfer->usOperation = DO_MOVE;
 
           // This is like a WinSendMsg except that it also does a DosGiveMem for the dragtransfer struct
           rc = (ULONG)DrgSendTransferMsg(pdinfo->hwndSource, DM_RENDER, (MPARAM)pdxfer, NULL);

           
           if(rc == TRUE)
           {
             AddCustomer(wd->hwndContainer, pxfercust, 10, 10);
           }

           DrgFreeDraginfo(pdinfo);
           DrgFreeDragtransfer(pdxfer);
           DosFreeMem((PVOID)pxfercust);

           break;           

        default: ;
      }
      break;


    case DM_RENDER:
      pdxfer = (PDRAGTRANSFER)mp1;
      pxferfromcust = &(((PCONTRECORD)(pdxfer->pditem->ulItemID))->cust);   //pointer to recordcore for dragged item

      DrgQueryStrName(pdxfer->hstrRenderToName, 100, dragxfermemname);  // get shared mem name
       
      DosGetNamedSharedMem((PPVOID)&pxfercust, dragxfermemname, PAG_WRITE | PAG_READ);
      memcpy(pxfercust, pxferfromcust, sizeof(CUSTOMER));
      DosFreeMem((PVOID)pxfercust);

      if(pdxfer->usOperation == DO_MOVE)      // if it's a move, remove the customer
      {
        RemoveCustomer(wd->hwndContainer, (PCONTRECORD)(pdxfer->pditem->ulItemID));
      }

      return((MRESULT)TRUE);
      break;
   
    case DM_PRINTOBJECT:         /* mp1 points to a DRAGITEM, mp2 to a PRINTDEST structure */
      pditem = (PDRAGITEM)mp1;   /* PLEASE NOTE: there is an error in the online PM Reference */
                                 /* which says that mp1 points to a DRAGINFO structure      */

      PrintCustomer((PCONTRECORD)(pditem->ulItemID), ((PPRINTDEST)mp2));

      return((MRESULT)DRR_SOURCE);      /* indicates that we will do the printing, */
      break;                            /* rather than the printer object          */


    case DM_DISCARDOBJECT:                       /* mp1 points to a DRAGINFO structure */
      pdinfo = (PDRAGINFO)mp1;                   /* the item to be deleted             */
      pditem = DrgQueryDragitemPtr(pdinfo, 0);

      RemoveCustomer((HWND)wd->hwndContainer, (PCONTRECORD)(pditem->ulItemID));

      return((MRESULT)DRR_SOURCE);               /* indicates that we take responsibility */
      break;                                      /* for deleting the item                 */


    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1))          /* test the command value from mp1 */
      {
        case ID_EXITPROG:
          WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
          break;

        default:
          return WinDefWindowProc(hwnd, msg, mp1, mp2);
      }
      break;


    case WM_SIZE:
      WinQueryWindowPos(hwnd, &swp);
      WinSetWindowPos(wd->hwndContainer, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy,  
                      SWP_SIZE | SWP_SHOW); 
      break;

    case WM_CLOSE:
      WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );     
      break;
   
    case WM_DESTROY:
      free(wd);
      break;

    default:
      return WinDefWindowProc(hwnd, msg, mp1, mp2);
  }
  return (MRESULT)FALSE;
} 


//***************************************************************************** */
//
//  Three utility functions used in program
//
//***************************************************************************** */
void FillWindow(HWND hwndContainer)
{
  CUSTOMER cust;
  FILE *finput;

  if((finput = fopen("dmcust.dat", "r")) != NULL)
    while(fscanf(finput, "%s%s%s", cust.name, cust.address, cust.phone) == 3)
    {
      WinSendMsg(hwndContainer, CM_ALLOCRECORD,
                                       (MPARAM)(sizeof(CONTRECORD) - sizeof(RECORDCORE)), (MPARAM)1);
      AddCustomer(hwndContainer, &cust, 10, 10);
    }
}


void AddCustomer(HWND hwndContainer, CUSTOMER* pcustomer, ULONG x, ULONG y)
{
  PCONTRECORD pcrec;
  RECORDINSERT rci;

  pcrec = (PCONTRECORD)WinSendMsg(hwndContainer, CM_ALLOCRECORD,
                                   (MPARAM)(sizeof(CONTRECORD) - sizeof(RECORDCORE)), (MPARAM)1);
  pcrec->core.ptlIcon.x = x;  pcrec->core.ptlIcon.y = y;
  pcrec->core.preccNextRecord = NULL;
  
  memcpy(&pcrec->cust, pcustomer, sizeof(CUSTOMER));
  pcrec->core.pszIcon = pcrec->cust.name;
  pcrec->core.hptrIcon = hptrCustIcon;
  rci.cb = sizeof(RECORDINSERT);
  rci.pRecordOrder = (PRECORDCORE)CMA_FIRST;
  rci.pRecordParent = NULL;
  rci.zOrder = CMA_TOP;
  rci.cRecordsInsert = 1;
  rci.fInvalidateRecord = TRUE;

  (ULONG)WinSendMsg(hwndContainer, CM_INSERTRECORD, (MPARAM)pcrec, (MPARAM)&rci);
}

void RemoveCustomer(HWND container, PCONTRECORD pcrec)
{
  PRECORDCORE aprc[10];

  aprc[0] = &(pcrec->core);
  WinSendMsg(container, CM_ERASERECORD, (MPARAM)&(pcrec->core), 0L);
  WinSendMsg(container, CM_REMOVERECORD, (MPARAM)aprc, MPFROM2SHORT(1, CMA_FREE));
}

void PrintCustomer(PCONTRECORD pcrec, PPRINTDEST pdest)
{
  // Here one should extract the print queue name from the PRINTDEST structure
  // and use PM printing functions to print to that queue.  For sample PM
  // printing code, please see the PRTSAMP toolkit sample.
  //
  // To keep this drag/drop sample simple we will will just copy the data to LPT1
  // regardless of which print object the item was dropped on.

  FILE *fprt;
  fprt = fopen("LPT1", "w");
  fprintf(fprt, "\n\nCustomer name: %s\n         Address: %s\n         Phone: %s",
                 pcrec->cust.name, pcrec->cust.address, pcrec->cust.phone);
  fclose(fprt);
}
