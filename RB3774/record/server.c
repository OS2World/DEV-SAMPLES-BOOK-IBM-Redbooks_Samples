/*************************************************************************/
/*                                                                       */
/* ITSC Redbook OS/2 v2.0 Sample Program                                 */
/*                                                                       */
/* SERVER.C  -  This is the server part of the WPS Record Class          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

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

#include "msgs.h"
#include "find.h"

#define ID_SRV_WND   200
#define ID_FRAME_WND 201
#define ID_MLE_WND   202
#define ID_VIEW_WND   203

#define DebugBox(title, text)  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP, \
                               (PSZ) text , (PSZ) title, 0, \
                               MB_OK | MB_INFORMATION )

#define Display(szMsg)   WinPostMsg(hwndMLEWindow, MLM_INSERT, (MPARAM)szMsg, (MPARAM)0 )


typedef struct {
        HWND hwndRequester;
        PID  pidRequester;
} SRV_DATA, *PSRV_DATA;

MRESULT EXPENTRY MainWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY ServerObjectWinProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

MRESULT EXPENTRY FindDlgProc(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);

VOID Message( PSZ szMsg);

void _Optlink ServerObjectThread (void *p);

HWND hwndServer;
HWND hwndCliArea;
HWND hwndMLEWindow;
HWND hwndFrame;

VOID _Optlink SearchThread(VOID *p);

BOOL WinCheck(HWND hwnd);
void removeUnwantedChars(CHAR * );

HAB  hab;
CHAR szMsgString[200];
CHAR param1[30];


void main(argc, argv)
   int argc;
   char *argv[];
{
  HMQ  hmq;
  QMSG qmsg;
  ULONG flCreate;

  strcpy (param1, argv[1]);

  hab = WinInitialize(0);

  hmq = WinCreateMsgQueue( hab, 0 );

  if ( !WinRegisterClass( hab, (PSZ)"ServerObjectWin",
                          (PFNWP)ServerObjectWinProc,
                          0L, sizeof(PSRV_DATA) )             )
  {
     DebugBox("Error","Register Server Class Failed");
  }


  if (!WinRegisterClass( hab, (PSZ)"MainWindow",
                        (PFNWP)MainWindowProc,
                        CS_SIZEREDRAW, 0 ))
  {
     DebugBox("Error","Register Server Frame Class Failed");
  }

  flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX | FCF_SHELLPOSITION ;

  hwndServer = WinCreateWindow(HWND_DESKTOP,
                                  "ServerObjectWin",
                                  "", 0L, 0,0,0,0,
                                  HWND_DESKTOP, HWND_TOP, ID_SRV_WND, NULL, NULL);


  hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0,
                                  &flCreate, "MainWindow",
                                  "Server", 0,
                                  (HMODULE)0L, ID_FRAME_WND,
                                  &hwndCliArea ) ;
  if(!hwndFrame)
  {
     DebugBox("Error","Create StdWindow Failed");
  }

  WinSetWindowText(hwndFrame, "Record Class Server");

  WinSetWindowPos( hwndFrame,  HWND_TOP, 0, 0, 250, 200,
                   SWP_SIZE | SWP_ZORDER | SWP_ACTIVATE | SWP_SHOW);

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
            _beginthread( ServerObjectThread, NULL, 9000, NULL);

            hwndMLEWindow = WinCreateWindow(hwnd, WC_MLE,
                                            "", MLS_READONLY | MLS_VSCROLL | MLS_HSCROLL | MLS_BORDER,
                                            0,0,0,0,
                                            hwnd, HWND_TOP,
                                            ID_MLE_WND, NULL, NULL);

            return WinDefWindowProc( hwnd, msg, mp1, mp2 );
        }
        break;

     case WM_SIZE:
        {
            RECTL  rc;
            WinQueryWindowRect(hwnd,&rc);

            rc.yTop -= 40L;

            /* Make the MLE follow the frame sizing */
            WinSetWindowPos( hwndMLEWindow,  HWND_TOP,
                             rc.xLeft,
                             rc.yBottom,
                             rc.xRight,
                             rc.yTop,
                             SWP_SIZE | SWP_MOVE | SWP_SHOW);


            return WinDefWindowProc( hwnd, msg, mp1, mp2 );
        }
        break;

    case WM_PAINT:
      {
         HPS    hps;
         RECTL  rc;

         hps = WinBeginPaint( hwnd, 0L, &rc );

         WinFillRect(hps, &rc, CLR_WHITE);
         WinQueryWindowRect(hwnd,&rc);

         rc.yBottom = rc.yTop - 40L;
         WinDrawText(hps, (LONG)strlen( szMsgString ),
                     szMsgString, &rc, CLR_BLUE,
                     CLR_WHITE, DT_CENTER | DT_VCENTER);

         WinEndPaint( hps );
      }
      break;

     case WM_CLOSE:
        {
            WinSendMsg(hwndServer, WM_QUIT, (MPARAM)0, (MPARAM)0 );
            return WinDefWindowProc( hwnd, msg, mp1, mp2 );
        }
        break;

     case WMP_DISPLAY_DATABASE_QUERY_DIALOG:
         {
           PVOID pCreateParam;
           pCreateParam = malloc(sizeof(ULONG));
           *((PULONG)pCreateParam) = (ULONG)mp1;

           WinLoadDlg(HWND_DESKTOP,   /* parent is desk top          */
                   hwnd,              /* owner                       */
                   FindDlgProc,       /* dialog procedure            */
                   0L,                /* load from resource file     */
                   ID_DLG_FIND,       /* dialog resource id          */
                   pCreateParam );    /* folder to return results    */
                                      /* gets passed in Create param */
         }
         break;

    default:
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)FALSE;
}

VOID Message(PSZ szMsg)
{
   strcpy( szMsgString, szMsg );
   WinInvalidateRect(hwndCliArea, NULL, TRUE);
}

VOID SearchThread(VOID *p)
{
   CHAR   *szSearch1, *szSearch2, *sptr;

   PVOID   pData;
   ULONG   ObjectSize;
   ULONG   AttributeFlags;

   CHAR    szTemp[500];

   APIRET  rc;
   USHORT  cFound;
   FILE    *fptr;
   USHORT  i;

   PQUERY_DATA pQuery;

   PRESULTS_DATA pResults;
   PPERSON_DATA  pPerson;


   pQuery = (PQUERY_DATA)p;

   sptr = szSearch1 = pQuery->szSearch;

   if( *(pQuery->szSearch) == '\0' )
   {
      WinPostMsg(hwndCliArea, WMP_DISPLAY_DATABASE_QUERY_DIALOG, (MPARAM)pQuery->folder, (MPARAM)0 );
      rc = DosFreeMem( (PVOID)p );
      if (rc)
      {
          DebugBox("Error","DosFreeMem Failed");
      }
   }
   else
   {
      Display ( "\nReceived Find Msg : ");
      Display ( pQuery->szSearch );

      strlwr(sptr);

      while(*sptr != '\0')
      {
         if(*sptr == '@')
         {
             *sptr = '\0';
             sptr++;
             szSearch2 = sptr;
         }
         if(*sptr == 42)
         {
             *sptr = '\0';
         }
         sptr++;
      }

      /* Allow up to 100 records to be returned. */
      ObjectSize     =  sizeof(RESULTS_DATA) + ( 100 * sizeof(PERSON_DATA) );

      AttributeFlags =  OBJ_GIVEABLE | PAG_WRITE | PAG_READ | PAG_COMMIT;

      rc = DosAllocSharedMem(&pData, NULL, ObjectSize,AttributeFlags);
      if (rc)
      {
         Display ( "Error : DosAllocSharedMem failed");
      }

      pResults = (PRESULTS_DATA) pData;
      pPerson  = (PPERSON_DATA) ( (ULONG)pData + (ULONG)sizeof(RESULTS_DATA) );

      pResults->cRecords = 0;

      pResults->folder = pQuery->folder;
      strcpy(pResults->szSearch, pQuery->szSearch);

      if( (fptr=fopen("C:\\NAMES.DAT","r")) != NULL)
      {
         for(i=0;i<100;i++)
         {
            fgets(pPerson->szName     , 99  , fptr);
            fgets(pPerson->szTelepnone, 39  , fptr);
            fgets(pPerson->szAddress  , 299 , fptr);

            if(feof(fptr))
               break;

            removeUnwantedChars(pPerson->szName     );
            removeUnwantedChars(pPerson->szTelepnone);
            removeUnwantedChars(pPerson->szAddress  );

            /*****************************/
            /* Perform simplified search */
            /*****************************/

            strcpy(szTemp, pPerson->szName      );
            strcat(szTemp, pPerson->szTelepnone );
            strlwr(szTemp);


            if( ( strstr(szTemp,szSearch1) != NULL ) &&  ( strstr(szTemp,szSearch2) ) )
            {
               pResults->cRecords++;
               pPerson++;
            }
         }
         fclose(fptr);
      }


      if(pResults->cRecords == 0)
      {
         strcpy( (CHAR *)pPerson,"Nothing Found");
      }

      rc = DosFreeMem( (PVOID)p );
      if (rc)
      {
          DebugBox("Error","DosFreeMem Failed");
      }
      WinPostMsg(hwndServer, WMP_RESULTS_FROM_SEARCH, (MPARAM)pResults, (MPARAM)0);
   }

}

BOOL WinCheck(HWND hwnd)
{
   if(! WinIsWindow(hab, hwnd) )
   {
      DosBeep(500,500);
      Display("\n<sys msg> Cannot Access Requester - Results data discarded");
      return(FALSE);
   }
   else
      return(TRUE);
}

void removeUnwantedChars(CHAR *string)
{
   CHAR *sptr;
   sptr = string;
   while(*sptr != '\0')
   {
      if(*sptr < 32 )
      {
         *sptr = '\0';
         break;
      }
      sptr++;
   }
}


/*****************************************************/
/*                Server Thread                      */
/*****************************************************/


void ServerObjectThread (void *p)
{
  HMQ  hmq;
  QMSG qmsg;
  HAB  hab;

  hab = WinInitialize(0);

  hmq = WinCreateMsgQueue( hab, 0 );

  if ( !WinRegisterClass( hab, (PSZ)"ServerObjectWin",
                          (PFNWP)ServerObjectWinProc,
                          0L, sizeof(PSRV_DATA) )             )
  {
     DebugBox("Error","Register Server Class Failed");
  }


  hwndServer = WinCreateWindow(HWND_OBJECT,
                                  "ServerObjectWin",
                                  "", 0L, 0,0,0,0,
                                  (HWND)0, HWND_TOP, 0L, NULL, NULL);


  while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
     WinDispatchMsg( hab, &qmsg );

  WinDestroyWindow(hwndServer);
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
}



MRESULT EXPENTRY ServerObjectWinProc( HWND hwnd,
                                      ULONG msg,
                                      MPARAM mp1,
                                      MPARAM mp2 )
{
   MRESULT mr;
   SRV_DATA *pServerData;

   mr = (MRESULT)FALSE;

   switch( msg )
   {
     case WM_CREATE:
       {
          HWND hwndReq;
          hwndReq = (HWND)atol(param1);
          WinPostMsg(hwndReq, WMP_SERVER_READY, (MPARAM)hwnd, (MPARAM)0 );
          WinPostMsg(hwnd, WMP_INITIALISE_SERVER, (MPARAM)hwndReq, (MPARAM)0 );
       }
       break;

    case WMP_INITIALISE_SERVER:
      {
         PID pid;
         TID tid;

         pServerData = (SRV_DATA *)malloc( sizeof(SRV_DATA) );

         pServerData->hwndRequester = (HWND)mp1;

         WinQueryWindowProcess((HWND)mp1,&pid,&tid);
         pServerData->pidRequester = pid;

         WinSetWindowPtr(hwnd,0,pServerData);

         Message("Ready");
         Display("<Note: In a real application this window would not be displayed>");

      }
      break;

    case WMP_RESULTS_FROM_SEARCH:
       {
         ULONG rc;
         pServerData = WinQueryWindowPtr(hwnd,0);

         if( WinCheck(pServerData->hwndRequester) )
         {
            rc = DosGiveSharedMem( (PVOID)mp1,
                                   pServerData->pidRequester,
                                   PAG_WRITE | PAG_READ);

            if (rc)
            {
                DebugBox("Error","DosGiveSharedMem Failed");
            }
            else
            {
               WinSendMsg(pServerData->hwndRequester, WMP_RESULTS_FROM_SEARCH, (MPARAM)mp1, (MPARAM)0);
               /* Free results memory from this process now it has been pass on */
               rc = DosFreeMem( (PVOID)mp1 );
               if (rc)
               {
                   DebugBox("Error","DosFreeMem Failed");
               }
            }
         }
         else
         {
            rc = DosFreeMem( (PVOID)mp1 );
            if (rc)
            {
                DebugBox("Error","DosFreeMem Failed");
            }
         }
       }
       break;

    case WMP_SEARCH_DATABASE:
       {
          _beginthread( SearchThread, NULL, 9000, (PVOID)mp1);
       }
       break;

    case WM_CLOSE:
       WinPostMsg( hwndFrame, WM_CLOSE, (MPARAM)0, (MPARAM)0 );
       mr = WinDefWindowProc( hwnd, msg, mp1, mp2 );
       break;

    case WM_QUIT:
       pServerData = WinQueryWindowPtr(hwnd,0);
       if(WinIsWindow(hab, pServerData->hwndRequester))
          WinPostMsg(pServerData->hwndRequester, WMP_SERVER_TERMINATED, (MPARAM)hwnd, (MPARAM)0 );
       mr = WinDefWindowProc( hwnd, msg, mp1, mp2 );
       break;

     default:
       mr = WinDefWindowProc( hwnd, msg, mp1, mp2 );
   }
   return mr;

}

MRESULT EXPENTRY FindDlgProc(HWND hwndDlg, ULONG msg,
                                  MPARAM mp1, MPARAM mp2)
{

 switch (msg)
 {

    case WM_INITDLG:

        WinSetWindowULong(hwndDlg,
                          QWL_USER,
                          *((PULONG)mp2) ); /* Store Folder pointer     */

        free(mp2);                          /* Free Create Param memory */
        break;

    case WM_COMMAND:
       {
          switch (SHORT1FROMMP(mp1))
          {
            case DID_OK:
              {
                 PQUERY_DATA pQuery;
                 PVOID       pData;
                 ULONG       ObjectSize;
                 ULONG       AttributeFlags;
                 CHAR        szTemp[100];
                 APIRET      rc;

                 ObjectSize     =  sizeof(QUERY_DATA);

                 AttributeFlags =  PAG_WRITE | PAG_READ | PAG_COMMIT;

                 rc = DosAllocMem(&pData, ObjectSize, AttributeFlags);
                 if (rc)
                 {
                    DebugBox("Error","DosAllocMem failed");
                 }

                 pQuery = (PQUERY_DATA)pData;

                 /* Get the stuff the user typed in */
                 WinQueryDlgItemText(hwndDlg,
                                     ID_EF_TELNUMBER,
                                     sizeof(szTemp),
                                     (PSZ)szTemp);

                 strcpy( pQuery->szSearch, szTemp);
                 strcat( pQuery->szSearch, "@");

                 WinQueryDlgItemText(hwndDlg,
                                     ID_EF_SURNAME,
                                     sizeof(szTemp),
                                     (PSZ)szTemp);
                 strcat( pQuery->szSearch, szTemp);

                 pQuery->folder = WinQueryWindowULong(hwndDlg, QWL_USER);

                 WinPostMsg(hwndServer, WMP_SEARCH_DATABASE, (MPARAM)pQuery, (MPARAM)0);
              }
              return (MRESULT) TRUE;
              break;

            case DID_CANCEL:
              WinDismissDlg(hwndDlg,DID_CANCEL);
              break;
          }
       }
       return (MRESULT) TRUE;
       break;
 }
 return (WinDefDlgProc(hwndDlg, msg, mp1, mp2) );
}
