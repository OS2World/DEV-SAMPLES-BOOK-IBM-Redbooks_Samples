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
 *  OS/2 and AIX text based version of ADMIN client                     *
 *                                                                      *
 * ******************************************************************** *
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
#define CONTINUE        1
#define EXIT            2
#define QUIT            3
#define EXECUTE         4

/*********************************/
/* classes                       */
/*********************************/
#include <clientcl.h>

/*********************************/
/* env test routine              */
/*********************************/
#include "bankdef.h"

/*********************************/
/* prototypes                    */
/*********************************/
short   getUserOption  (void);
short   processAdminMenu(char *);
void    openAccounts (Customer );
void    createCustomer(Customer ,char * );
void    displayLogo(void);
void    deleteCustomer(void);
void    inquireAboutCustomer(long);

/*********************************/
/* static & globals              */
/*********************************/
static CustomerMetaClass  theCustomerClass;
static CheckingAccountMetaClass theCheckingAccountClass;
static SavingsAccountMetaClass theSavingsAccountClass;
static boolean fTrace = FALSE;
Environment   *e;
ClientClass clClass;

/**********************************************************************/
/* main program                                                       */
/**********************************************************************/
void main(int argc, char *argv[])
{
   unsigned short usUserOption;
   char szBranch[9];

                                     /*********************************/
                                     /* Display logo                  */
                                     /*********************************/
   displayLogo();

                                     /*********************************/
                                     /* SOM environment initialization*/
                                     /*********************************/

   e = somGetGlobalEnvironment();
   clClass=ClientClassNew();

   if (argc ==2 )
   {
      if (strlen(argv[1]) <= 8)
         strcpy(szBranch,argv[1]);
   }
   else
      strcpy(szBranch,"B001");


                                     /*********************************/
                                     /* Application menu loop: exit   */
                                     /* on user quit request          */
                                     /*********************************/

   while( processAdminMenu(szBranch) == CONTINUE);

                                     /*********************************/
                                     /* release the allocated servers */
                                     /*********************************/
   _somFree(clClass);
   exit(0);
}

/**********************************************************************/
/* processAdminMenu: it is the application main loop.                 */
/*                   Exit only on user 'Quit' request                 */
/**********************************************************************/
short processAdminMenu(char * szBranch)
{
   short  usVerifyResult;
   char cInput;
   Customer  aNewCustomer = NULL;
   unsigned char szInput[200];

   somPrintf("\n\n*********** WELCOME TO SOMBank *************\n\n");
   somPrintf("\n\n************* Administration ***************\n\n");
   somPrintf("   Please select an option:\n\n");
   somPrintf("     N - Create a new customer\n");
   somPrintf("     D - Delete a customer\n");
   somPrintf("     I - Inquire about a customer\n");
   somPrintf("     Q - Quit\n\n");
   somPrintf("   ==> ");
   fflush(stdout);
   cInput = getchar();
   fflush(stdin);
   switch (toupper(cInput))
   {
     case 'N':
        createCustomer(aNewCustomer,szBranch);
        return(CONTINUE);
     break;

     case 'D':
        deleteCustomer();
        return(CONTINUE);
     break;

     case 'I':
        inquireAboutCustomer(0);
        return(CONTINUE);
     break;

     case 'Q':
        return(QUIT);
     break;

     default:
        somPrintf("\n  Invalid selection code: %c\n",cInput);
        return(CONTINUE);
     break;

   }
}

/**********************************************************************/
/* createCustomer: this routine will create a new customer object     */
/*                 and the related accounts ( calling the openAccount */
/*                 routine) and save the data on user reuqest         */
/**********************************************************************/

void createCustomer(Customer aNewCustomer,char * szBranch)
{
   unsigned char szInput[200];
   char cInput;
   long retCode;
   long CustomerPIN;

   CheckingAccount   aCheckingAccount=NULL;
   SavingsAccount   aSavingsAccount=NULL ;
                                     /*********************************/
                                     /* display customer menu         */
                                     /*********************************/

   somPrintf("\n\n********** NEW CUSTOMER CREATION ***********\n\n");
   somPrintf("   Enter the customer name (max 49 chars).\n\n");
   somPrintf("   Customer name ==> ");
   scanf("%s",szInput);
   fflush(stdin);
   if ( strlen(szInput) > 49 )
   {
      somPrintf("\n   Name truncated to 49 characters.\n");
      szInput[49] = '\0';
                                     /*********************************/
                                     /* create a new customer         */
                                     /*********************************/
   }

   retCode = _createCustomer(clClass, e, szInput, szBranch,&CustomerPIN);
                                     /*********************************/
                                     /* open accounts for this        */
                                     /* customer...                   */
                                     /*********************************/

   openAccounts(aNewCustomer);

   somPrintf("Customer Created :\n");
   inquireAboutCustomer(__get_CustomerID(clClass,e));
   return;
}
/**********************************************************************/
/* openAccount   : this routine will create new account objects       */
/*                 returning them to the customer routine             */
/**********************************************************************/
void openAccounts(Customer  aNewCustomer)
{
   unsigned char szInput[200];
   char cInput;
   long retCode;
   unsigned long ulCustomerId;
   boolean  fBalanceOK;
   boolean Checking = FALSE;
   boolean Savings = FALSE;
   short i;
   double balance=0;

   while (1) {
       somPrintf("\n********** ACCOUNT SELECTION MENU **********\n");
       somPrintf("\n   Please select an option:\n\n");
       
       if ( !Checking)
          somPrintf("     C - Checking account\n");
       if ( !Savings)
          somPrintf("     S - Savings account\n");
       
       somPrintf("     X - Exit\n\n");
       somPrintf("   ==> ");
       fflush(stdout);
       cInput = getchar();
       fflush(stdin);
       
       if (  ((!Checking) && toupper(cInput) == 'C')
           || ((!Savings) && toupper(cInput) == 'S')
           || ( toupper(cInput) == 'X') )
       {
          switch (toupper(cInput))
          {
              case 'C':
       
                fBalanceOK = FALSE;
                while( !fBalanceOK)
                {
                   somPrintf("\n\n  Please, specify the opening balance ==> ");
                   fflush(stdout);
                   scanf("%s",szInput);
                   fflush(stdin);
                   for (i=0;i<strlen(szInput) && (isdigit(szInput[i]) != 0); i++);
                   if (i == strlen(szInput))
                   {
                      fBalanceOK =TRUE;
                   }
                }
       
                _createCheckingAccount(clClass,e, (double) atof(szInput));
                Checking=TRUE;
       
            break;
       
            case 'S':
                fBalanceOK = FALSE;
                while( !fBalanceOK)
                {
                   somPrintf("\n\n  Please, specify the opening balance ==> ");
                   fflush(stdout);
                   scanf("%s",szInput);
                   fflush(stdin);
                   for (i=0;i<strlen(szInput) && (isdigit(szInput[i]) != 0); i++);
                   if (i == strlen(szInput))
                   {
                      fBalanceOK =TRUE;
                   }
                }
       
                _createSavingsAccount(clClass,e, (double) atof(szInput));
                Savings=TRUE;
            break;
       
            case 'X':
               return;
            break;
       
       
            default:
               return;
            break;
       
          }
       }
   } /* endwhile */
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

void deleteCustomer(void)
{
   char szInput[200];
   unsigned long ulPin;
   unsigned long ulCustomerCode;
   short i;
   char cInput;

   somPrintf("\n  Please specify one of the following :\n\n");
   somPrintf("        - ID of Customer to delete (4 digits)\n");
   somPrintf("        - 'A' to abort.\n\n");
   somPrintf("  and press enter ==> ");
   fflush(stdout);
   scanf("%s",szInput);
   cInput = szInput[0];
   fflush(stdin);
   switch (toupper(cInput))
   {

      case 'A':
         return;
      break;

      default:
      break;
   }

                                     /*********************************/
                                     /* check customer ID is 4 digits */
                                     /*********************************/
   for (i=0;i<4 & isdigit(szInput[i]) != 0 ; i++);

   if (  i < 4 || strlen(szInput) > 4)
   {
      somPrintf("\n  Error: Invalid customer code: 4 digits required.\n");
      somPrintf("\n  Transacation discarded.\n");
      return;
   }

   ulCustomerCode = atol(szInput);

   inquireAboutCustomer(ulCustomerCode);


   somPrintf("\n  Delete this customer? (Y/N) :\n\n");
   somPrintf("  and press enter ==> ");
   fflush(stdout);
   scanf("%s",szInput);
   cInput = szInput[0];
   fflush(stdin);
   switch (toupper(cInput))
   {

      case 'Y':
          if ( _deleteCustomer(clClass,e,ulCustomerCode) ) {
             somPrintf("ID Not Found\n");
          } else {
             somPrintf("Customer Deleted\n");
          } /* endif */
      break;

      default:
      break;
   }

return;
}


void inquireAboutCustomer(long ulCustomerCode)
{

   char szInput[200];
   char cInput;
   short i;

   if (ulCustomerCode==0) {
   somPrintf("\n  Please specify one of the following :\n\n");
   somPrintf("        - ID of Customer to inquire about (4 digits)\n");
   somPrintf("        - 'A' to abort.\n\n");
   somPrintf("  and press enter ==> ");
   fflush(stdout);
   scanf("%s",szInput);
   cInput = szInput[0];
   fflush(stdin);
   switch (toupper(cInput))
   {

      case 'A':
         return;
      break;

      default:
      break;
   }
                                     /*********************************/
                                     /* check customer ID is 4 digits */
                                     /*********************************/
   for (i=0;i<4 & isdigit(szInput[i]) != 0 ; i++);

   if (  i < 4 || strlen(szInput) > 4)
   {
      somPrintf("\n  Error: Invalid customer code: 4 digits required.\n");
      somPrintf("\n  Transacation discarded.\n");
      return;
   }

   ulCustomerCode = atol(szInput);

   } /* endif */
   if (_restoreCustomer(clClass, e, ulCustomerCode)==0){
       somPrintf("Customer ID : %04u\n",__get_CustomerID(clClass,e));
       somPrintf("Customer Name : %s\n",__get_CustomerName(clClass,e));
       somPrintf("Customer Branch : %s\n",__get_CustomerBranchID(clClass,e));
       somPrintf("Customer Checking Account ID : %04u\n",__get_CheckingAccountID(clClass,e));
       somPrintf("Customer Savings Account ID : %04u\n",__get_SavingsAccountID(clClass,e));
       return;
    } else {
       somPrintf("Customer not found\n");
    }
}

