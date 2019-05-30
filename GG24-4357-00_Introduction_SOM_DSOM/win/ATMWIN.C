/*
 *----------------------------------------
 * SOMATM.C -- Automated Teller Machine
 *             for SOMBank
 *----------------------------------------
 */
#include <windows.h>
/*
 ********************************
 * 'C' and SOMD                 *
 ********************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <somd.h>

/*
 *********************************
 * classes                       *
 *********************************
 */

#include "clientcl.h"

/*
 *********************************
 * SOMBank                       *
 *********************************
 */
#include "bankdef.h"
#include "cldef.h"  /* Defines for Windows Clients */

typedef struct serverRecord serverRecord;

/*********************************/
/* static & globals              */
/*********************************/

Environment        *e;
static ClientClass clClass;

int                 isommsg;
unsigned long       ulPin;
HWND                hwndATMWin[TOTWINS];
HINSTANCE           hinst;
HWND                hMwnd;                            // handle for Main Window
                                                      // Global because must be
                                                      // used by  procedures
/*********************************/
/* prototypes                    */
/*********************************/


                                                            // Forward Declarations
long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;  // Main Window procedure
long FAR PASCAL _export EditProc (HWND, UINT, UINT, LONG);  // Procedure for edit box
long FAR PASCAL _export ChildProc (HWND, UINT, UINT, LONG); // Procedure for push button
                                                            // group
char *LoadMsg(WORD wID);

FARPROC lpfnOldEdit;
FARPROC lpfnOldChild;
FARPROC lpfnEditProc;
FARPROC lpfnChildProc;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,  // Main Routine
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = APP_NAME ;

     MSG         msg ;
     WNDCLASS    wndclass ;

     SOM_MainProgram();

     lpfnEditProc  = MakeProcInstance((FARPROC) EditProc, hInstance);
     lpfnChildProc = MakeProcInstance((FARPROC) ChildProc, hInstance);

     if (!hPrevInstance)
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }
     hinst=hInstance;

     hMwnd = CreateWindow (szAppName, WIN_TITLE,
                           WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL) ;

     ShowWindow (hMwnd, nCmdShow) ;
     UpdateWindow (hMwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

/*

   W       W N   N DDD   PPP  RRR   OO  CCCC
   W   W   W NN  N D  D  P  P R  R O  O C
   W   W   W N N N D   D PPP  RRR  O  O C
    W W W W  N  NN D  D  P    R  R O  O C
     W   W   N   N DDD   P    R  R  OO  CCCC

*/
long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
long     retCode;
static   boolean             fChecking, fSavings;
static   char                szBuffer [10] ;          // Input buffer
static   unsigned char       szBranch[21];
static   char                msgbuf[40];

static   char               *AccountObjs[]={"CheckingAccount",
                                            "SavingsAccount"};
static   int                 i;                       // iterator
static   int                 cxChar, cyChar ;         // Pixels per Char
static   int                 ATMStep;                 // State of ATM Application
static   int                 SelectedAccount;
static   long                AccountID;
static   int                 SelectedWID;
static   LRESULT             nLength;                 // Length of returned bytes
static   long                ulCustomerId ;
static   double              dAmount;

         HDC                 hdc ;                  // Handle for Device Context
         TEXTMETRIC          tm ;                   // Text Metric Structure

struct tagMyWinParms {
                   char   *cntType;
                   char   *cntText;
                   DWORD   cntStyle;
                   int     xOffset;
                   int     yOffset;
                   int     xLength;
                   int     yLength;
                   int     parWin;
                   };

static struct tagMyWinParms SOMWin[] ={

/* Type       Text                        Style                     xPos  yPos xLen yLen Parent */

{"static", "SOMBank... A Distributed SOM Application",    WS_CHILD | WS_VISIBLE | SS_CENTER,       1 ,  1 , 62 ,  3 , MAIN}, //LOGO
{"button", "SOM ATM Keypad",  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,     2 ,  4 , 22 , 24 , MAIN}, //PAD
{"button", "SOM ATM Display", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,    26 ,  9 , 36 , 19 , MAIN}, //DISP
{"static", NULL,              WS_CHILD | WS_VISIBLE | SS_CENTER,      29 , 11 , 30 ,  2 , MAIN}, //DISPMSG1
{"static", NULL,              WS_CHILD | WS_VISIBLE | SS_CENTER,      29 , 13 , 30 ,  2 , MAIN}, //DISPMSG2
{"static", NULL,              WS_CHILD | WS_VISIBLE | SS_CENTER,      29 , 23 , 30 ,  2 , MAIN}, //DISPMSG3
{"static", NULL,              WS_CHILD | WS_VISIBLE | SS_CENTER,      28 , 5 , 30 ,  3 , MAIN}, //SSTATMSG
{"button", "0",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 , 15 , 15 ,  3 , MAIN}, //KEY_0
{"button", "1",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 ,  6 ,  5 ,  3 , MAIN}, //KEY_1
{"button", "2",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  10 ,  6 ,  5 ,  3 , MAIN}, //KEY_2
{"button", "3",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  15 ,  6 ,  5 ,  3 , MAIN}, //KEY_3
{"button", "4",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 ,  9 ,  5 ,  3 , MAIN}, //KEY_4
{"button", "5",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  10 ,  9 ,  5 ,  3 , MAIN}, //KEY_5
{"button", "6",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  15 ,  9 ,  5 ,  3 , MAIN}, //KEY_6
{"button", "7",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 , 12 ,  5 ,  3 , MAIN}, //KEY_7
{"button", "8",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  10 , 12 ,  5 ,  3 , MAIN}, //KEY_8
{"button", "9",               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  15 , 12 ,  5 ,  3 , MAIN}, //KEY_9
{"button", "OK",              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 , 19 ,  7 ,  3 , MAIN}, //OKB
{"button", "CLR",             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  13 , 19 ,  7 ,  3 , MAIN}, //CLR
{"button", "CANCEL",          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,   5 , 23 , 15 ,  3 , MAIN}, //CANCEL
{"edit",   NULL,              WS_CHILD | WS_BORDER  | ES_LEFT,        41 , 16 ,  6 ,  2 , MAIN}, //EDITBOX
{"button", NULL,              WS_CHILD | BS_GROUPBOX               ,  34 , 15 , 20 ,  8 , MAIN}, //BUTGRP
{"button", NULL,              WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,  1 ,  1 , 18 ,  2 , BUTGRP}, //RBUT1
{"button", NULL,              WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,  1 ,  3 , 18 ,  2 , BUTGRP}, //RBUT2
{"button", NULL,              WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,  1 ,  5 , 18 ,  2 , BUTGRP} //RBUT3
};

     switch (message)
          {
          case WM_CREATE:                                         // Creation of main window
               ATMStep=ENTERID;

               hdc = GetDC (hwnd) ;                               // Get text metrics
               SelectObject (hdc,                                 // of display for
                            GetStockObject (SYSTEM_FIXED_FONT)) ; // calculating text
               GetTextMetrics (hdc, &tm) ;                        // and window positions
               cxChar = tm.tmAveCharWidth ;                       //
               cyChar = tm.tmHeight + tm.tmExternalLeading ;      //
               ReleaseDC (hwnd, hdc) ;                            //

               MoveWindow(hwnd,10,10,cxChar*66,cyChar*32,TRUE);   // Size main window
                                                                  // to right size
               for (i=0 ; i < TOTWINS ; i++) {
                   hwndATMWin[i] = CreateWindow (SOMWin[i].cntType,
                                                 SOMWin[i].cntText,
                                                 SOMWin[i].cntStyle,
                                                 cxChar * SOMWin[i].xOffset,
                                                 cyChar * SOMWin[i].yOffset,
                                                 cxChar * SOMWin[i].xLength,
                                                 (cyChar * SOMWin[i].yLength),
                                                 (SOMWin[i].parWin == MAIN) ? hwnd : hwndATMWin[SOMWin[i].parWin],
                                                 i,
                                                 ((LPCREATESTRUCT) lParam) -> hInstance,
                                                 NULL) ;
               };
               SetWindowText(hwndATMWin[SSTATMSG],"SOMobjects...Making Reuse a Reality");

                                     /*********************************/
                                     /* SOM environment initialization*/
                                     /*********************************/
               e = somGetGlobalEnvironment();

               clClass=ClientClassNew();


               lpfnOldEdit  = (FARPROC) GetWindowLong (hwndATMWin[EDITBOX], GWL_WNDPROC);
               lpfnOldChild = (FARPROC) GetWindowLong (hwndATMWin[BUTGRP], GWL_WNDPROC);
               SetWindowLong (hwndATMWin[EDITBOX], GWL_WNDPROC, (LONG) lpfnEditProc);
               SetWindowLong (hwndATMWin[BUTGRP], GWL_WNDPROC, (LONG) lpfnChildProc);
               SendMessage(hwnd,WM_COMMAND,CANCEL,0L);
               return 0 ;

          case WM_COMMAND:

               switch (wParam)
                    {
                    case OKB:                    // OK Button
                                                 // Process Input
                         for (i = (KEY_0) ; i<= (RBUT3) ; i++)
                             EnableWindow(hwndATMWin[i],FALSE);

                         SetWindowText(hwndATMWin[DISPMSG3],NULL); // Clear message area
                         switch (ATMStep)                // Based on application step
                              {
                              case ENTERID:              // Intro screen
                                   szBuffer[0]=0x00;     // Sets max length of
                                   szBuffer[1]=0x04;     //  data to retreive
                                   nLength = SendMessage(hwndATMWin[EDITBOX],     //Retrieves data
                                                         EM_GETLINE,
                                                         0,
                                                         (DWORD) (LPSTR) szBuffer);

                                   szBuffer[nLength]=NULL;             // Make NULL terminated
                                   SendMessage(hwndATMWin[EDITBOX],EM_SETSEL,0,MAKELONG(0,-1)); //Clear edit
                                   SendMessage(hwndATMWin[EDITBOX],WM_CLEAR,0,0L);              // control

                                   if (nLength != 4) {
                                      SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_E_SHORTID));
                                      SendMessage(hwnd,WM_USER,ENTERID,0L);
                                      break;
                                   }
                                   ulCustomerId = atol(szBuffer);
                                   SendMessage(hwnd,WM_USER,ENTERPIN,0L);
                                   break;

                              case ENTERPIN:
                                   szBuffer[0]=0x00;
                                   szBuffer[1]=0x04;
                                   nLength = SendMessage(hwndATMWin[EDITBOX],
                                                         EM_GETLINE,
                                                         0,
                                                         (DWORD) (LPSTR) szBuffer);
                                   szBuffer[nLength]=NULL;
                                   SendMessage(hwndATMWin[EDITBOX],EM_SETSEL,0,MAKELONG(0,-1));
                                   SendMessage(hwndATMWin[EDITBOX],WM_CLEAR,0,0L);

                                   if (nLength != 4) {
                                      SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_E_SHORTPIN));
                                      SendMessage(hwnd,WM_USER,ENTERPIN,0L);
                                      break;
                                   }
                                   ulPin = atol(szBuffer);

                                   retCode = _restoreATMCustomer(clClass, e, ulCustomerId, ulPin);
                                   if (check(*e)) {
                                       somExceptionFree(e);
                                   }

                                   if ( retCode == NOT_FOUND) {
                                       SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_E_IDNF));
                                       SendMessage(hwnd,WM_COMMAND,CANCEL,0L);
                                       break;
                                   }
                                   if ( retCode == BAD_PIN)  {
                                      SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_E_BADPIN));
                                      SendMessage(hwnd,WM_COMMAND,CANCEL,0L);
                                      break;
                                   }

                                   fChecking = FALSE;
                                   fSavings = FALSE;
                                   if (__get_CheckingAccountID(clClass, e) != NULL)
                                       fChecking=TRUE;
                                   if (__get_SavingsAccountID(clClass, e) != NULL)
                                       fSavings=TRUE;


                                   if (! (fChecking || fSavings)) {
                                      SetWindowText(hwndATMWin[DISPMSG3],"No accounts exist.");
                                      SendMessage(hwnd,WM_COMMAND,CANCEL,0L);
                                      break;
                                   } /*end if*/
                                   SendMessage(hwnd,WM_USER,ENTERACCT,0L);
                                   break;

                              case ENTERACCT:
                                   switch (SelectedAccount)
                                   {
                                        case CHECKING :
                                          AccountID = __get_CheckingAccountID(clClass,e);
                                          break;
                                        case SAVINGS :
                                          AccountID = __get_SavingsAccountID(clClass,e);
                                          break;
                                   }
                                   SendMessage(hwnd,WM_USER,ENTERWID,0L);
                                   break;

                              case ENTERWID:

                                   switch (SelectedWID)
                                        {
                                        case INQUIRE :
                                             SetWindowText(hwndATMWin[DISPMSG2],LoadMsg(ATM_R_INQUIRE));
                                             SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_PRESS_OK));
                                             _inquire(clClass, e, AccountID, SelectedAccount, &dAmount);
                                             if (check(*e)) {
                                                 somExceptionFree(e);
                                             }
                                             SendMessage(hwnd,WM_USER,DISP_RESULTS,0L);
                                             sprintf(msgbuf,"Current Balance : %12.2f",dAmount);
                                             SetWindowText(hwndATMWin[DISPMSG2],msgbuf);
                                             SetWindowText(hwndATMWin[DISPMSG3],"Another Transaction ?");
                                             break;
                                        case WITHDRAW :
                                        case DEPOSIT :
                                             SendMessage(hwnd,WM_USER,ENTERAMT,0L);
                                             break;
                                       };  /*switch - SelectedWID*/
                                   break;

                              case ENTERAMT:
                                   szBuffer[0]=0x00;szBuffer[1]=0x06;
                                   nLength = SendMessage(hwndATMWin[EDITBOX],EM_GETLINE,0, (DWORD) (LPSTR) szBuffer);
                                   szBuffer[nLength]=NULL;
                                   SendMessage(hwndATMWin[EDITBOX],EM_SETSEL,0,MAKELONG(0,-1));
                                   SendMessage(hwndATMWin[EDITBOX],WM_CLEAR,0,0L);

                                   if (szBuffer[0]==NULL) {
                                      SetWindowText(hwndATMWin[DISPMSG3],LoadMsg(ATM_E_NODATA));
                                      SendMessage(hwnd,WM_USER,ENTERAMT,0L);
                                      break;
                                   } /*end if*/
                                   dAmount=(double) atof(szBuffer);

                                   switch (SelectedWID)
                                        {
                                        case DEPOSIT:
                                             _deposit(clClass, e, AccountID, SelectedAccount, dAmount);
                                             if (check(*e)) {
                                                 somExceptionFree(e);
                                             }
                                             break;

                                        case WITHDRAW:
                                             _withdraw(clClass, e, AccountID, SelectedAccount, dAmount);
                                             if (check(*e)) {
                                                 somExceptionFree(e);
                                             }
                                             break;
                                        } /*switch*/

                                    _inquire(clClass, e, AccountID, SelectedAccount, &dAmount);
                                    if (check(*e)) {
                                        somExceptionFree(e);
                                    }
                                   SendMessage(hwnd,WM_USER,DISP_RESULTS,0L);
                                   sprintf(msgbuf,"New Balance : %12.2f",dAmount);
                                   SetWindowText(hwndATMWin[DISPMSG2],msgbuf);
                                   SetWindowText(hwndATMWin[DISPMSG3],"Another Transaction ?");
                                   break;

                              case DISP_RESULTS:
                                   SendMessage(hwnd,WM_USER,ENTERACCT,0L);
                                   SelectedWID=WITHDRAW;
                                   break;
                              } /*switch - ATMStep*/
                         break;

                    case CLR :
                         SendMessage(hwndATMWin[EDITBOX],EM_SETSEL,0,MAKELONG(0,-1));
                         SendMessage(hwndATMWin[EDITBOX],WM_CLEAR,0,0L);
                         if (ATMStep==ENTERID)
                             SetWindowText(hwndATMWin[DISPMSG3],NULL);
                         break;

                    case CANCEL:
                         SendMessage(hwnd,WM_COMMAND,CLR,0L);
                         SelectedAccount=CHECKING;
                         SelectedWID=WITHDRAW;
                         AccountID= (long) NULL;
                         SendMessage(hwnd,WM_USER,ENTERID,0L);
                         break;

                    case RBUT1:
                         SendMessage(hwndATMWin[RBUT2], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT3], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT1], BM_SETCHECK, 1, 0L);
                         SetFocus(hwndATMWin[RBUT1]);
                         switch (ATMStep)
                              {
                              case ENTERACCT:
                                   SelectedAccount=CHECKING;
                                   break;

                              case ENTERWID:
                                   SelectedWID=WITHDRAW;
                                   break;
                              }; /*switch ATMStep*/
                         break;

                    case RBUT2:
                         SendMessage(hwndATMWin[RBUT1], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT3], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT2], BM_SETCHECK, 1, 0L);
                         SetFocus(hwndATMWin[RBUT2]);
                         switch (ATMStep)
                              {
                              case ENTERACCT:
                                   SelectedAccount=SAVINGS;
                                   break;
                              case ENTERWID:
                                   SelectedWID=DEPOSIT;
                                   break;
                              }; /*switach ATMStep*/
                         break;

                    case RBUT3:
                         SendMessage(hwndATMWin[RBUT1], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT2], BM_SETCHECK, 0, 0L);
                         SendMessage(hwndATMWin[RBUT3], BM_SETCHECK, 1, 0L);
                         SetFocus(hwndATMWin[RBUT3]);
                         switch (ATMStep)
                              {
                              case ENTERWID:
                                   SelectedWID=INQUIRE;
                                   break;
                              };
                         break;

                    default:   /*switch wparam*/
                         if ((wParam>=KEY_0) && (wParam<=KEY_9) &&
                             ((ATMStep==ENTERID)||
                              (ATMStep==ENTERPIN||
                              (ATMStep==ENTERAMT))))   // if numerical button
                            SendMessage(hwndATMWin[EDITBOX],WM_CHAR,48+wParam-(KEY_0),0x00000001);
                    } /*switch wparam*/

               if (((ATMStep==ENTERID) || (ATMStep==ENTERPIN) || (ATMStep==ENTERAMT)) && !(wParam == EDITBOX))         //Return Focus to edit control
                    SetFocus(hwndATMWin[EDITBOX]);

               return 0 ;
               break; /*WM_Command*/

          case WM_USER:                          // Change of state message
                                                 // ATMStep is current state
               switch (ATMStep)                  // wParam contains desired state
                    {
                    case ENTERID:
                    case ENTERPIN:               // Hide control for current state
                    case ENTERAMT:
                         ShowWindow(hwndATMWin[EDITBOX],SW_HIDE);
                         break;

                    case ENTERACCT:
                    case ENTERWID:
                         ShowWindow(hwndATMWin[BUTGRP],SW_HIDE);
                         break;
                    case DISP_RESULTS:
                         SetWindowText(hwndATMWin[DISPMSG3],NULL);
                         break;
                    } /*switch - ATMStep*/

               ATMStep=wParam;                  // Update state to desired state

               switch (ATMStep)                 // Processeing to set up new state
                    {                           // including modifying edit controls
                    case ENTERID:
                         SendMessage(hwndATMWin[EDITBOX],EM_LIMITTEXT,4,0L); //Set Entry Limits
                         SendMessage(hwndATMWin[EDITBOX],EM_SETPASSWORDCHAR,0,0L);
                         ShowWindow(hwndATMWin[EDITBOX],SW_SHOWNORMAL);
                         for (i = (KEY_0) ;  i<= (EDITBOX) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);
                         SetFocus(hwndATMWin[EDITBOX]);
                         break;

                    case ENTERPIN:
                         SendMessage(hwndATMWin[EDITBOX],EM_LIMITTEXT,4,0L); //Set Entry Limits
                         SendMessage(hwndATMWin[EDITBOX],EM_SETPASSWORDCHAR,(WORD) '*',0L);
                         ShowWindow(hwndATMWin[EDITBOX],SW_SHOWNORMAL);
                         for (i = (KEY_0) ;  i<= (EDITBOX) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);
                         SetFocus(hwndATMWin[EDITBOX]);
                         break;

                    case ENTERACCT:
                         for (i = (OKB) ; i <= (CANCEL) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);
                         EnableWindow(hwndATMWin[BUTGRP],TRUE);

                         if (fChecking){
                             SendMessage(hwndATMWin[RBUT1],BM_SETCHECK,1,0L);
                             SendMessage(hwndATMWin[RBUT2],BM_SETCHECK,0,0L);
                             SelectedAccount=CHECKING;
                             SetFocus(hwndATMWin[RBUT1]);
                         } else {
                             SendMessage(hwndATMWin[RBUT2],BM_SETCHECK,1,0L);
                             SendMessage(hwndATMWin[RBUT1],BM_SETCHECK,0,0L);
                             SelectedAccount=SAVINGS;
                             SetFocus(hwndATMWin[RBUT2]);
                         };

                         SetWindowText(hwndATMWin[SAVBUT],LoadMsg(ATM_SAVINGS));
                         SetWindowText(hwndATMWin[CHKBUT],LoadMsg(ATM_CHECKING));

                         EnableWindow(hwndATMWin[CHKBUT],fChecking);
                         EnableWindow(hwndATMWin[SAVBUT],fSavings);
                         ShowWindow(hwndATMWin[RBUT3],SW_HIDE);
                         ShowWindow(hwndATMWin[BUTGRP],SW_SHOWNORMAL);
                         if (SelectedAccount=CHECKING)
                             SetFocus(hwndATMWin[RBUT1]);
                         else
                             SetFocus(hwndATMWin[RBUT2]);

                         break;

                    case ENTERWID:
                         for (i = (OKB) ; i <= (CANCEL) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);
                         for (i = (BUTGRP) ; i <= (RBUT3) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);

                         SetWindowText(hwndATMWin[INQBUT],LoadMsg(ATM_INQUIRE));
                         SetWindowText(hwndATMWin[DEPBUT],LoadMsg(ATM_DEPOSIT));
                         SetWindowText(hwndATMWin[WTHBUT],LoadMsg(ATM_WITHDRAW));
                         for (i=0;i<3;i++){
                             SendMessage(hwndATMWin[RBUT1+i], BM_SETCHECK, ((i==0) ? 1 : 0), 0L);
                             EnableWindow(hwndATMWin[RBUT1+i],TRUE);
                         };  /*for*/
                         ShowWindow(hwndATMWin[RBUT3],SW_SHOWNORMAL);
                         ShowWindow(hwndATMWin[BUTGRP],SW_SHOWNORMAL);
                         SetFocus(hwndATMWin[RBUT1]);
                         break;

                    case ENTERAMT:
                         SendMessage(hwndATMWin[EDITBOX],EM_LIMITTEXT,6,0L); //Set Entry Limits
                         SendMessage(hwndATMWin[EDITBOX],EM_SETPASSWORDCHAR,0,0L);
                         ShowWindow(hwndATMWin[EDITBOX],SW_SHOWNORMAL);
                         for (i = (KEY_0) ;  i<= (EDITBOX) ; i++)
                             EnableWindow(hwndATMWin[i],TRUE);
                         SetFocus(hwndATMWin[EDITBOX]);
                         break;

                    case DISP_RESULTS:
                         EnableWindow(hwndATMWin[OKB],TRUE);
                         EnableWindow(hwndATMWin[CANCEL],TRUE);
                         break;
                    }; /*switch ATMStep*/

               SetWindowText(hwndATMWin[DISPMSG1],LoadMsg(ATM_I_ID+ATMStep)); //Set Prompts for new state
               SetWindowText(hwndATMWin[DISPMSG2],LoadMsg(ATM_P_ID+ATMStep));     // XXXXXXXXXXXX

               return 0;
               break;

          case WM_DESTROY:

               /*********************************/
               /* terminate the programs.       */
               /*********************************/
                _somFree(clClass);

               PostQuitMessage (0) ;
               return 0 ;
          } /*switch message*/
     return DefWindowProc (hwnd, message, wParam, lParam) ;  // Default processing of
     }                                                       // message

long FAR PASCAL _export EditProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
     {

     switch (message)
          {
          case WM_CHAR:
             if (!(((wParam>='0') && (wParam<='9')) || (wParam=='\r') || (wParam=='\b')))
                return 0;
               if (wParam == '\r')                     // Return key maps to OK
                   SendMessage(hMwnd,WM_COMMAND,OKB,0L);
               break ;

          case WM_KEYUP:
               if (wParam==VK_ESCAPE)                  // Escape key maps to clear
                   SendMessage(hMwnd,WM_COMMAND,CLR,0L);
               break;

          }

     return CallWindowProc (lpfnOldEdit, hwnd, message, wParam, lParam) ;
     }

long FAR PASCAL _export ChildProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
     {
     short n = GetWindowWord(hwnd,GWW_ID);
     switch (message)
            {
          //  case WM_KEYDOWN:
          //                                              // Need to make hwnd..
          //       if (wParam == VK_TAB)                // global for this to work
          //          {
          //            SetFocus(hwndATMWin[RBUT1+(((n-(RBUT1))+1)%3)]);
          //           };


            case WM_COMMAND:
                 SendMessage(hMwnd,message,wParam,lParam);
                 break;

            case WM_CHAR:
               if (wParam == '\r')
                   SendMessage(hMwnd,WM_COMMAND,OKB,0L);
               break ;

            }
     return CallWindowProc (lpfnOldChild, hwnd, message, wParam, lParam) ;

     }


char *LoadMsg (WORD wID)
{
   static char szBuffer [40] ;

   LoadString(hinst, wID, szBuffer, 40);
   return szBuffer;
}
