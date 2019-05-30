 /*------------------------------------------
   SOMADMIN-- Admin ASpplication for SOMBank
  ------------------------------------------*/

#include <windows.h>
#include <somd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "adminwin.h"
#include "resource.h"
#include "resrc1.h"
#include "cldef.h"

/*********************************/
/* classes                       */
/*********************************/

#include "clientcl.h"
#include "bankdef.h"

/*********************************/
/* prototypes                    */
/*********************************/
short   processCreateCustomer(HWND);
short   processFindCustomer(HWND);
short   processDeleteCustomer(HWND);
long FAR PASCAL _export EditProc (HWND, UINT, UINT, LONG);  // Procedure for edit box

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export CreCustDlgProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export AboutDlgProc (HWND, UINT, UINT, LONG) ;
/*********************************/
/* static & globals              */
/*********************************/
static ClientClass clClass;
Environment   *e;
FARPROC lpfnEditProc;
FARPROC lpfnOldEdit;


int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "SOMAdmin" ;
     MSG         msg;
     HWND        hwnd ;
     WNDCLASS    wndclass ;

     SOM_MainProgram();

     lpfnEditProc  = MakeProcInstance((FARPROC) EditProc, hInstance);

     if (!hPrevInstance)
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "SOM Administration",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd);

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static FARPROC lpfnCreCustDlgProc ;
     static FARPROC lpfnAboutDlgProc ;
     static HANDLE  hInstance ;
     static   int                 ADMINStep;                 // State of ADMIN Application

     static unsigned short usUserOption;

     switch (message)
          {
          case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnCreCustDlgProc = MakeProcInstance ((FARPROC) CreCustDlgProc,
                                                    hInstance) ;

               lpfnAboutDlgProc = MakeProcInstance ((FARPROC) AboutDlgProc,
                                                    hInstance) ;


             e = somGetGlobalEnvironment();
             clClass=ClientClassNew();
             lpfnOldEdit  = (FARPROC) GetWindowLong (GetDlgItem(hwnd, IDC_EDIT_ID), GWL_WNDPROC);
             SetWindowLong (GetDlgItem(hwnd, IDC_EDIT_ID), GWL_WNDPROC, (LONG) lpfnEditProc);


               return 0 ;

          case WM_COMMAND:
               switch (wParam)
                    {

                     case IDM_CUST:
                         DialogBox (hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd,
                                    lpfnCreCustDlgProc) ;
                          break;
                     case IDM_ABOUT:
                         DialogBox (hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hwnd,
                                    lpfnAboutDlgProc) ;
                         break;
                     case IDM_EXIT:
                           SendMessage( hwnd, WM_DESTROY, 0, 0L);
                         return 0 ;
                     }
               break ;

          case WM_DESTROY :
                                     /*********************************/
                                     /* terminate the programs.*/
                                     /*********************************/
                _somFree(clClass);
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }



BOOL FAR PASCAL _export CreCustDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               SendDlgItemMessage( hDlg, IDC_COMBOBRANCH, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "B001");
               SendDlgItemMessage( hDlg, IDC_COMBOBRANCH, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "B002");
               SendMessage ( hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
               SendMessage ( hDlg, WM_USER, INITBUTTONS, 0L);
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDSAVE:
                         EnableWindow (GetDlgItem(hDlg, IDSAVE), FALSE);
                         processCreateCustomer (hDlg);
                         SendMessage (hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
                         SendMessage (hDlg, WM_USER, INITBUTTONS, 0L);
                         break;
                    case IDVIEW:
                         SendMessage ( hDlg, WM_USER, ENTER_CUSTOMERID, 0L);
                         break;
                    case IDSEARCH:
                         SendMessage ( hDlg, WM_USER, INITBUTTONS, 0L);
                         EnableWindow (GetDlgItem(hDlg, IDC_EDIT_ID), FALSE);
                         EnableWindow (GetDlgItem(hDlg, IDC_CUSTOMERID), FALSE);
                         if (processFindCustomer(hDlg) == 0)      /* 0 = ok, 1 = not found */
                             EnableWindow (GetDlgItem(hDlg, IDDELETE), TRUE);
                         break;
                    case IDCREATE:
                         SendMessage (hDlg, WM_USER, ENTER_CUSTOMER, 0L);
                         break;
                    case IDDELETE:
                         SendMessage (hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
                         SendMessage (hDlg, WM_USER, INITBUTTONS, 0L);
                         processDeleteCustomer(hDlg);
                         break;
                    case  IDC_CHECK1:
                         EnableWindow (GetDlgItem(hDlg, IDC_EDIT2), TRUE);
                         SetFocus(GetDlgItem(hDlg, IDC_EDIT2));
                         break;
                    case  IDC_CHECK2:
                         EnableWindow (GetDlgItem(hDlg, IDC_EDIT3), TRUE);
                         SetFocus(GetDlgItem(hDlg, IDC_EDIT3));
                         break;
                    case IDCANCEL:
                         SendMessage (hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
                         SendMessage (hDlg, WM_USER, INITBUTTONS, 0L);
                         break;
                    case IDCLOSE:
                         EndDialog (hDlg, 0) ;
                         return TRUE ;
//                    default:   /*switch wparam*/
  //                       if ((wParam>=KEY_0) && (wParam<=KEY_9))
    //                        SendMessage(GetDlgItem(hDlg, IDC_EDIT_ID),WM_CHAR,48+wParam-(KEY_0),0x00000001);
                    }
               break ;
          case WM_USER:
                switch (wParam)
                             {
                             case ENTER_CUSTOMER:
                                       SendMessage ( hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
                                       SendMessage ( hDlg, WM_USER, DISABLEBUTTONS, 0L);
                                       EnableWindow (GetDlgItem(hDlg, IDCLOSE), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDSAVE), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_CUSTOMERNAME), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_BRANCH), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_COMBOBRANCH), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_ACCOUNTTYPE), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_CHECK1), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_CHECK2), TRUE);
                                       SendDlgItemMessage( hDlg, IDC_COMBOBRANCH, CB_SELECTSTRING,(WPARAM) -1, (LPARAM) (LPSTR) "B001");
                                       SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
                                       break;
                              case ENTER_CUSTOMERID:
                                       SendMessage ( hDlg, WM_USER, CLEAR_DISABLE_FIELDS, 0L);
                                       SendMessage ( hDlg, WM_USER, DISABLEBUTTONS, 0L);
                                       EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
                                       EnableWindow (GetDlgItem(hDlg, IDSEARCH), TRUE);
                                       EnableWindow (GetDlgItem(hDlg, IDCLOSE), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_CUSTOMERID), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDC_EDIT_ID), TRUE);
                                       SetFocus(GetDlgItem(hDlg, IDC_EDIT_ID));
                                       break;
                             case INITBUTTONS:
                                       EnableWindow (GetDlgItem(hDlg, IDSEARCH), FALSE);
                                       EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
                                       EnableWindow(GetDlgItem(hDlg, IDCLOSE), TRUE);
                                       EnableWindow(GetDlgItem(hDlg, IDCREATE), TRUE);
                                       EnableWindow (GetDlgItem(hDlg, IDVIEW), TRUE);
                                       EnableWindow (GetDlgItem(hDlg, IDSAVE), FALSE);
                                       EnableWindow (GetDlgItem(hDlg, IDDELETE), FALSE);
                                       break;
                             case DISABLEBUTTONS:
                                       EnableWindow (GetDlgItem(hDlg, IDSEARCH), FALSE);
                                       EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
                                       EnableWindow(GetDlgItem(hDlg, IDCLOSE), FALSE);
                                       EnableWindow(GetDlgItem(hDlg, IDCREATE), FALSE);
                                       EnableWindow (GetDlgItem(hDlg, IDVIEW), FALSE);
                                       EnableWindow (GetDlgItem(hDlg, IDSAVE), FALSE);
                                       EnableWindow (GetDlgItem(hDlg, IDDELETE), FALSE);
                                       break;
                             case CLEAR_DISABLE_FIELDS:
                                    /* disables all textfields */
                                     EnableWindow(GetDlgItem(hDlg, IDC_CUSTOMERID), FALSE);
                                     EnableWindow(GetDlgItem(hDlg, IDC_PIN), FALSE);
                                     EnableWindow(GetDlgItem(hDlg, IDC_CUSTOMERNAME), FALSE);
                                     EnableWindow(GetDlgItem(hDlg, IDC_BRANCH), FALSE);
                                     EnableWindow(GetDlgItem(hDlg, IDC_BALANCE), FALSE);
                                     EnableWindow(GetDlgItem(hDlg, IDC_ACCOUNTTYPE), FALSE);

                                     /* clears and disables all editcontrols */

                                     EnableWindow (GetDlgItem(hDlg, IDC_EDIT1), FALSE);
                                     SendDlgItemMessage(hDlg, IDC_EDIT1,EM_SETSEL,0,MAKELONG(0,-1));
                                     SendDlgItemMessage(hDlg, IDC_EDIT1,WM_CLEAR,0,0L);

                                     EnableWindow (GetDlgItem(hDlg, IDC_EDIT2), FALSE);
                                     SendDlgItemMessage(hDlg, IDC_EDIT2,EM_SETSEL,0,MAKELONG(0,-1));
                                     SendDlgItemMessage(hDlg, IDC_EDIT2,WM_CLEAR,0,0L);

                                     EnableWindow (GetDlgItem(hDlg, IDC_EDIT3), FALSE);
                                     SendDlgItemMessage(hDlg, IDC_EDIT3,EM_SETSEL,0,MAKELONG(0,-1));
                                     SendDlgItemMessage(hDlg, IDC_EDIT3,WM_CLEAR,0,0L);

                                     EnableWindow (GetDlgItem(hDlg, IDC_EDIT_ID), FALSE);
                                     SendDlgItemMessage(hDlg, IDC_EDIT_ID,EM_SETSEL,0,MAKELONG(0,-1));
                                     SendDlgItemMessage(hDlg, IDC_EDIT_ID,WM_CLEAR,0,0L);

                                     EnableWindow (GetDlgItem(hDlg, IDC_EDIT_PIN), FALSE);
                                     SendDlgItemMessage(hDlg, IDC_EDIT_PIN,EM_SETSEL,0,MAKELONG(0,-1));
                                     SendDlgItemMessage(hDlg, IDC_EDIT_PIN,WM_CLEAR,0,0L);

                                    EnableWindow (GetDlgItem(hDlg, IDC_COMBOBRANCH), TRUE);
                                    SendDlgItemMessage(hDlg, IDC_COMBOBRANCH,CB_SETEDITSEL,0,MAKELONG(0,-1));
                                    SendDlgItemMessage(hDlg, IDC_COMBOBRANCH,WM_CLEAR,0,0L);
                                    EnableWindow (GetDlgItem(hDlg, IDC_COMBOBRANCH), FALSE);

                                    EnableWindow (GetDlgItem(hDlg, IDC_CHECK1), FALSE);
                                    EnableWindow (GetDlgItem(hDlg, IDC_CHECK2), FALSE);
                                    SendDlgItemMessage (hDlg, IDC_CHECK1, BM_SETCHECK, FALSE, 0);
                                    SendDlgItemMessage (hDlg, IDC_CHECK2, BM_SETCHECK, FALSE, 0);
                                    break;
                          };  /* switch adminstep */
          return 0;
          }
     return FALSE ;
     }


BOOL FAR PASCAL _export AboutDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
{
     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                         EndDialog (hDlg, 0) ;
                         return TRUE ;
                     };
                break ;
          return 0;
          }
     return FALSE ;
}






/**********************************************************************/
/* processCreateCustomer: Creates a Customer and its */
/*                   accounts                                                        */
/**********************************************************************/


short processCreateCustomer(HWND hDlg)
{

   CheckingAccount   aCheckingAccount=NULL;
   SavingsAccount   aSavingsAccount=NULL ;
   long retCode;
   long CustomerPIN;


   char szInput[40];
   char szBranch [5];
   char szBalanceSavings [100];
   char szBalanceChecking [100];
   static int i=0;
   int nCheck1=0, nCheck2=0;

   static double dBalanceSavings;
   static double dBalanceChecking;

   GetDlgItemText ( hDlg, IDC_EDIT1, (LPSTR) szInput, 40 );
   GetDlgItemText ( hDlg, IDC_COMBOBRANCH, (LPSTR) szBranch, 5 );
   GetDlgItemText ( hDlg, IDC_EDIT3, (LPSTR) szBalanceSavings, 100);
   GetDlgItemText ( hDlg, IDC_EDIT2, (LPSTR) szBalanceChecking, 100);


   dBalanceSavings=(double) atof(szBalanceSavings);
   dBalanceChecking=(double) atof(szBalanceChecking);


   retCode = _createCustomer(clClass, e, szInput, szBranch, &CustomerPIN);
   if (check(*e)) {
       somExceptionFree(e);
   }
    nCheck1 = (int) SendDlgItemMessage (hDlg, IDC_CHECK1, BM_GETCHECK, 0, 0);
    nCheck2 = (int) SendDlgItemMessage (hDlg, IDC_CHECK2, BM_GETCHECK, 0, 0);

   if ( nCheck1 == 1 ) {
         _createCheckingAccount(clClass, e, dBalanceChecking );
         if (check(*e)) {
             somExceptionFree(e);
         }
   }

   if ( nCheck2 == 1 ){
       _createSavingsAccount(clClass, e, dBalanceSavings );
         if (check(*e)) {
             somExceptionFree(e);
         }
   }

   SetDlgItemInt( hDlg, IDC_EDIT_ID, (int) __get_CustomerID(clClass, e),FALSE);
   SetDlgItemInt( hDlg, IDC_EDIT_PIN, (int) CustomerPIN, FALSE);

   MessageBox(hDlg,"New Customer created","SOMBank  News",MB_OK);

   return (short) retCode;

}



/**********************************************************************/
/* processFindCustomer: Finds a Customer and its */
/*                                         accounts                                  */
/**********************************************************************/

short processFindCustomer(HWND hDlg)
{
   char msgbuf[40];
   static int i=0;
   long retCode;
   long lCustomerID;

   double dBalanceSavings=0;
   double dBalanceChecking=0;

   SendDlgItemMessage(hDlg, IDC_EDIT2,EM_SETSEL,0,MAKELONG(0,-1));
   SendDlgItemMessage(hDlg, IDC_EDIT2,WM_CLEAR,0,0L);
   SendDlgItemMessage(hDlg, IDC_EDIT3,EM_SETSEL,0,MAKELONG(0,-1));
   SendDlgItemMessage(hDlg, IDC_EDIT3,WM_CLEAR,0,0L);
   SendDlgItemMessage (hDlg, IDC_CHECK1, BM_SETCHECK, FALSE, 0);
   SendDlgItemMessage (hDlg, IDC_CHECK2, BM_SETCHECK, FALSE, 0);


   lCustomerID = (long) GetDlgItemInt( hDlg, IDC_EDIT_ID, NULL ,FALSE);

   retCode = _restoreCustomer(clClass, e, lCustomerID);
         if (check(*e)) {
             somExceptionFree(e);
         }

   if (retCode)  {
      MessageBox(hDlg,"Customer not found","SOMBank  News",MB_OK);
      return 1;
   }

   SetDlgItemText(hDlg, IDC_EDIT1, __get_CustomerName(clClass,e));
   SendDlgItemMessage(hDlg, IDC_COMBOBRANCH, CB_SELECTSTRING,
                                       0, (LPARAM) (LPSTR) __get_CustomerBranchID(clClass,e));

     if ( __get_CheckingAccountID(clClass, e) != NULL)  {
         if (check(*e)) {
             somExceptionFree(e);
         }
        SendDlgItemMessage (hDlg, IDC_CHECK1, BM_SETCHECK, TRUE, 0);
        _inquire(clClass, e, __get_CheckingAccountID(clClass,e), 3, &dBalanceChecking);
         if (check(*e)) {
             somExceptionFree(e);
         }
        sprintf(msgbuf,"%12.2f",dBalanceChecking);
        SetDlgItemText(hDlg, IDC_EDIT2, msgbuf);
     }


   if (__get_SavingsAccountID(clClass, e) != NULL) {
         if (check(*e)) {
             somExceptionFree(e);
         }
        SendDlgItemMessage (hDlg, IDC_CHECK2, BM_SETCHECK, TRUE, 0);
        _inquire(clClass, e, __get_SavingsAccountID(clClass,e), 2, &dBalanceSavings);
         if (check(*e)) {
             somExceptionFree(e);
         }
        sprintf(msgbuf,"%12.2f",dBalanceSavings);
        SetDlgItemText(hDlg, IDC_EDIT3, msgbuf);
    }


   return 0;

}


/**********************************************************************/
/* processDeleteCustomer: Delete a Customer               */
/*                                                                                           */
/**********************************************************************/

short processDeleteCustomer(HWND hDlg)
{
   long retCode;
   long lCustomerID;

   lCustomerID = (long) GetDlgItemInt( hDlg, IDC_EDIT_ID, NULL ,FALSE);

   retCode = _deleteCustomer(clClass, e, lCustomerID);
   if (check(*e)) {
       somExceptionFree(e);
   }

   if (retCode == 1)
      MessageBox(hDlg,"Customer cannot be deleted","SOMBank  News",MB_OK);
   else
      MessageBox(hDlg,"Customer is deleted","SOMBank  News",MB_OK);
   return 0;

}


long FAR PASCAL _export EditProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
     {

     switch (message)
          {
          case WM_CHAR:
             if (!(((wParam>='0') && (wParam<='9')) || (wParam=='\r') || (wParam=='\b')))
                return 0;
               if (wParam == '\r')                     // Return key maps to OK
                   SendMessage(hwnd,WM_COMMAND,IDSEARCH,0L);
               break ;

          case WM_KEYUP:
               if (wParam==VK_ESCAPE)                  // Escape key maps to clear
                   SendMessage(hwnd,WM_COMMAND,IDCANCEL,0L);
               break;

          }

     return CallWindowProc (lpfnOldEdit, hwnd, message, wParam, lParam) ;
     }


