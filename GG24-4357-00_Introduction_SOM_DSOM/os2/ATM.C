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
 *       atm.c      'C' code for the ATM client program                 *
 *                                                                      *
 ********************************************************************** *
 */
/********************************/
/* 'C' and SOMD                 */
/********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <somd.h>

/********************************/
/* program constant             */
/********************************/
#define CUSTOMER_OK     1
#define CUSTOMER_NOT_OK 2
#define CONTINUE        3
#define EXIT            4
#define QUIT            5
#define EXECUTE         6
#define SAVINGS         2
#define CHECKING        3

/*********************************/
/* classes                       */
/*********************************/
#include "clientcl.h"
/*********************************/
/* env test routine              */
/*********************************/
#include "bankdef.h"

/*********************************/
/* prototypes                    */
/*********************************/
void         processAccounts(long accountType);
short        processAtmMenu(void);
short        getUserOptions(void);
void         displayLogo(void);
short        processAccount(Customer );
SOMDServer   getBranchServer(char *alias);
void         freeServerArray(void);

/*********************************/
/* static & globals              */
/*********************************/

static ClientClass               clClass;
static boolean                   fTrace;
Environment                     * e;
static serverRecord             * firstServerRecord = NULL;

/**********************************************************************/
/* main program                                                       */
/**********************************************************************/
void main(int argc, char *argv[])
{
   string newCentralAlias=NULL;
   unsigned short usUserOptions;


                                     /*********************************/
                                     /* Display logo                  */
                                     /*********************************/
   displayLogo();

                                     /*********************************/
                                     /* SOM environment initialization*/
                                     /*********************************/
   e = somGetGlobalEnvironment();
   clClass=ClientClassNew();

   while( processAtmMenu() == CONTINUE);

                                     /*********************************/
                                     /* release the allocated servers */
                                     /*********************************/
   _somFree(clClass);
   exit(0);
}

/**********************************************************************/
/* processAtmMenu: it is the application main loop.                   */
/*                   Exit only on user 'Quit' request                 */
/**********************************************************************/
short processAtmMenu()
{
   short  usProcessResult;
   char szInput[200];
   unsigned long ulPin;
   unsigned long ulCustomerCode;
   short i;
   char cInput;

   Customer  anExistingCustomer = NULL;

   somPrintf("\n*********** WELCOME TO SOMBank *************\n");

   somPrintf("\n  Please specify one of the following options:\n\n");
   somPrintf("        - your customer ID (4 digits)\n");
   somPrintf("        - 'Q' to quit.\n\n");
   somPrintf("  and press enter ==> ");
   fflush(stdout);
   scanf("%s",szInput);
   cInput = szInput[0];
   switch (toupper(cInput))
   {

      case 'Q':
         return(QUIT);
      break;

      default:
      break;
   }
                                     /*********************************/
                                     /* check customer ID is 4 digits */
                                     /*********************************/
   for (i=0;i<4 & isdigit(szInput[i]) != 0 ; i++);

   if (  i < 4 | strlen(szInput) > 4)
   {
      somPrintf("\n  Error: Invalid customer code: 4 digits required.\n");
      somPrintf("\n  Transacation discarded.\n");
      return(CONTINUE);
   }

   ulCustomerCode = atol(szInput);

                                     /*********************************/
                                     /* get and check PIN is 4 digits */
                                     /*********************************/

   somPrintf("\n  Enter your PIN (4 digits) ==> ");
   fflush(stdout);
   scanf("%s",szInput);
   fflush(stdin);
   for (i=0;i<4 & isdigit(szInput[i]) != 0 ; i++);

   if (  i < 4 | strlen(szInput) > 4)
   {
      somPrintf("\n  Error: Invalid PIN: 4 digits required.\n");
      somPrintf("\n  Transacation discarded.\n");
      return(CONTINUE);
   }

   ulPin = atol(szInput);

                                     /*********************************/
                                     /* check if customer is on       */
                                     /* central files                 */
                                     /*********************************/

   if(_restoreATMCustomer(clClass,e,ulCustomerCode,ulPin)){
      somPrintf("\n  Customer not found.\n");
      return(CONTINUE);
   }

   usProcessResult = processAccount(anExistingCustomer);
   switch (usProcessResult)
   {

      case QUIT:
         return(QUIT);
      break;

      default:
        return(CONTINUE);
      break;
   }
}


/**********************************************************************/
/* process account: this routine creates the account objects and      */
/*                  performs the customer selected options on them    */
/**********************************************************************/
short processAccount(Customer anExistingCustomer)
{
   boolean fChecking, fSavings;
   char szInput[200];
   long ulCustomerId ;
    char cInput;
    short i;
   unsigned char szBranch[21];
   string newBranchAlias=NULL;

   fChecking = __get_CheckingAccountID(clClass,e);
   fSavings = __get_SavingsAccountID(clClass,e);

   if (! (fChecking || fSavings) )
   {
      somPrintf("  No accounts exists.\n");
      somPrintf("\n  Transacation discarded.\n");
      return;
   }

                                     /*********************************/
                                     /* display account menu          */
                                     /*********************************/

   cInput = ' ';
   while (cInput == ' ')
   {
      somPrintf("\n  Specify account type:\n\n");
      if (fChecking)
      {
         somPrintf("       C - checking account\n");
      }
      if (fSavings)
      {
         somPrintf("       S - saving account:\n");
      }
      somPrintf("       X - Exit\n");
      somPrintf("\n  ==> ");
      fflush(stdout);
      cInput = getchar();
      fflush(stdin);
      if ( (fChecking && toupper(cInput) == 'C')
              | (fSavings && toupper(cInput) == 'S')
              | (toupper(cInput) == 'X')    )
      {
      }
      else
      {
         somPrintf("\n   Invalid selection option: %c.\n",cInput);
         somPrintf("   Please re-enter a valid one.\n");
         cInput = ' ';
      }
   }
                                     /*********************************/
                                     /* process selected account      */
                                     /*********************************/
   switch (toupper(cInput))
   {
     case 'C':
        processAccounts(CHECKING);
     break;

     case 'S':
        processAccounts(SAVINGS);

     break;

     case 'X':

     break;

     default:

     break;

                                     /*********************************/
                                     /* free customer and account     */
                                     /* objects and release branch    */
                                     /* server                        */
                                     /*********************************/
   }

}
/**********************************************************************/
/* processCheckingAccount: routine to perform deposit, withdrawal     */
/*                         or inquiry on cheking accounts             */
/**********************************************************************/
void processAccounts(long accountType)
{
   char cInput;
   char szInput[200];
   boolean fAmountOK;
   double dAmount;
   double dTraceAmount;
   short i;
   long retCode;

                                     /*********************************/
                                     /* display menu                  */
                                     /*********************************/

   cInput = ' ';
   while (cInput == ' ')
   {
      somPrintf("\n\n********** Account selection menu **********\n\n");
      somPrintf("  Please select an option:\n\n");
      somPrintf("       D - Deposit\n");
      somPrintf("       I - Inquiry\n");
      somPrintf("       W - Withdraw\n");
      somPrintf("       X - Exit\n\n");
      somPrintf("   ==> ");
      fflush(stdout);
      cInput = getchar();
      fflush(stdin);
                                     /*********************************/
                                     /* process selected option       */
                                     /*********************************/
      switch (toupper(cInput))
      {
        case 'D':
                                     /*********************************/
                                     /* deposit...                    */
                                     /*********************************/
           somPrintf("\n\n  Enter the amount to deposit ==> ");
           fAmountOK =FALSE;
           while( !fAmountOK)
           {
              fflush(stdout);
              scanf("%s",szInput);
              for (i=0;i<strlen(szInput) && (isdigit(szInput[i]) != 0); i++);
              if (i == strlen(szInput))
              {
                 fAmountOK =TRUE;
              }
              else
              {
                 somPrintf("\n\n  Invalid amount, please ");
                 somPrintf("re-enter a valid one ==>");
              }
           }


           dAmount = (double) atof(szInput);

           if (accountType==CHECKING) {
               _deposit(clClass,e,
                        __get_CheckingAccountID(clClass,e),
                        CHECKING,dAmount);
           }else {
              _deposit(clClass,e,
                       __get_SavingsAccountID(clClass,e),
                       SAVINGS,dAmount);
           } /* endif */

        break;
                                     /*********************************/
                                     /* inquiry...                    */
                                     /*********************************/

        case 'I':

           if (accountType==CHECKING) {
               _inquire(clClass,e,__get_CheckingAccountID(clClass,e),
                                 CHECKING,&dAmount);
           }else {
              _inquire(clClass,e,__get_SavingsAccountID(clClass,e),
                                SAVINGS,&dAmount);
           } /* endif */
           somPrintf("\n  Checking account balance =%12.2f\n",
                       dAmount);

        break;
                                     /*********************************/
                                     /* withdrawal...                 */
                                     /*********************************/

        case 'W':
           somPrintf("\n  Enter the amount to withdraw ");
           somPrintf("in multiples of $10 ==> ");
           fAmountOK =FALSE;
           while( !fAmountOK)
           {
              fflush(stdout);
              scanf("%s",szInput);
              for (i=0;i<strlen(szInput) && (isdigit(szInput[i]) != 0); i++);
              if (i == strlen(szInput) && szInput[strlen(szInput)-1] == '0')
              {
                 fAmountOK =TRUE;
              }
              else
              {
                 somPrintf("\n\n  Invalid amount, please ");
                 somPrintf(" re-enter a valid one ==>");
              }
           }


           dAmount = (double) atof(szInput);

           if (accountType==CHECKING) {
               _withdraw(clClass,e,__get_CheckingAccountID(clClass,e),
                                   CHECKING,dAmount);
           }else {
               _withdraw(clClass,e,__get_SavingsAccountID(clClass,e),
                                 SAVINGS,dAmount);
           } /* endif */

        break;

        default:
            somPrintf("\n   Invalid selection option:%c.\n",cInput);
            somPrintf("   Please re-enter a valid one.\n");
            cInput = ' ' ;
        break;
      }
   }
}
/**********************************************************************/
/* displayLogo routine                                                */
/**********************************************************************/
void displayLogo()
{
   somPrintf("*******************************************\n");
   somPrintf("*                                         *\n");
   somPrintf("*       SOMBank Sample Application        *\n");
   somPrintf("*                                         *\n");
   somPrintf("*  Documented in ITSO Technical Bulletin: *\n");
   somPrintf("*                                         *\n");
   somPrintf("*   SOMobjects: A Practical Introduction  *\n");
   somPrintf("*            to SOM and DSOM              *\n");
   somPrintf("*              (GG24-4357)                *\n");
   somPrintf("*                                         *\n");
   somPrintf("*     Copyright IBM Corp. 1993,1994       *\n");
   somPrintf("*******************************************\n");
}
