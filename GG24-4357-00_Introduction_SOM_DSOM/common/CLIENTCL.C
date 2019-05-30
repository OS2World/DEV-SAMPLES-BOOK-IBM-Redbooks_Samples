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
 * -------------------------------------------------------------------- *
 * SOMBank Sample Application                                           *
 *                                                                      *
 * Documented in:                                                       *
 *  IBM International Technical Support Organization Bulletin (Redbook) *
 *  "SOMobjects: A Practical Introduction to SOM and DSOM" (GG24-4357)  *
 * -------------------------------------------------------------------- *
 */

#ifndef SOM_Module_clientcl_Source
#define SOM_Module_clientcl_Source
#endif
#define ClientClass_Class_Source
#define SOMMSingleInstance_Class_Source
#include <somd.h>
#include "svrbrokr.h"
#include "savings.h"
#include "checking.h"
#include "customer.h"
#include "clientcl.ih"
#include "account.h"
#include "accmgr.h"
#include "bankdef.h"

SOM_Scope void  SOMLINK somInit(ClientClass somSelf)
{

    Environment *e;
    SOMDServer svr;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","somInit");

    ClientClass_parent_SOMObject_somInit(somSelf);

    e=somGetGlobalEnvironment();
    SOMD_Init(e);


    ServerBrokerNewClass(0,0);
    CustomerNewClass(0,0);
    CheckingAccountNewClass(0,0);
    SavingsAccountNewClass(0,0);

                                     /*********************************/
                                     /* Create the ServerBroker server*/
                                     /*********************************/

    svr = _somdFindServerByName(SOMD_ObjectMgr,e,"ServerBroker");
    if (svr) {
        _theSVRBroker = _somdCreateObj(svr,e,"ServerBroker","");
        _somdProxyFree(svr,e);
        svr=NULL;
    } /* endif */

                                     /*********************************/
                                     /* Create the CentralServer      */
                                     /* server with a dedicated       */
                                     /* server alias and init         */
                                     /* Customer class                */
                                     /*********************************/

    if (_theSVRBroker) {
        _nameOfServer(_theSVRBroker, e, "Central", &_newCentralAlias);
    } /* endif */
    if (check(*e)) return;

    svr = _somdFindServerByName(SOMD_ObjectMgr, e, _newCentralAlias);
    if (svr) {
        _theCustomerClass =  _somdGetClassObj (svr,e,"Customer");
        _somdProxyFree(svr,e);
        svr=NULL;
    } /* endif */
    if (check(*e)) {
        _theCustomerClass=NULL;
        return;
    }

    _firstServerRecord = NULL;
    _CheckingAccountID = (long) NULL;
    _SavingsAccountID = (long) NULL;
    _CustomerID = (long) NULL;
    _aCustomer = NULL;
    return;

}


SOM_Scope void  SOMLINK somUninit(ClientClass somSelf)
{

    Environment *e;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","somUninit");

    ClientClass_parent_SOMObject_somUninit(somSelf);

    e=somGetGlobalEnvironment();
                                     /*********************************/
                                     /* release the allocated servers */
                                     /*********************************/

    _freeClient(somSelf,e);
    if (strlen(_newCentralAlias)) {
       _releaseServer(_theSVRBroker, e, _newCentralAlias);
       ORBfree(_newCentralAlias);
       _newCentralAlias=NULL;
    }
    _freeBranchServerArray(somSelf, e);
    if (check(*e)) return;

                                     /*********************************/
                                     /* free proxies to classes       */
                                     /*********************************/
    if (_theCustomerClass){
        _somdProxyFree(_theCustomerClass,e);
    }
                                     /*********************************/
                                     /* free proxy to ServerBroker*/
                                     /*********************************/
    if (_theSVRBroker) {
        _somdProxyFree(_theSVRBroker,e);
    }
    SOMD_Uninit(e);
}

/**********************************************************************/
/* getBranchServer function is used to find a server for a branch     */
/* If the server was already used, reuse the server started, if no    */
/* server was used for the branch, starts a new one with the alias    */
/* provided by the ServerBroker                                       */
/**********************************************************************/


SOM_Scope SOMDServer  SOMLINK getBranchServer(ClientClass somSelf,
                                               Environment *ev,
                                               string branchId)
{
    serverRecord * aRecord;
    string newAlias = NULL;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","getBranchServer");

    aRecord = _firstServerRecord;

    while (_firstServerRecord != NULL)  {
        if ( strcmp(aRecord->alias, branchId)==0 ) {
           _theAccountsManager= aRecord->theAccountsManager ;
           _theCheckingAccountClass = aRecord->theCheckingAccountClass;
           _theSavingsAccountClass =  aRecord->theSavingsAccountClass;
           return aRecord->server;
        }
        if (aRecord->next == NULL ) {
            break;
        } else {
           aRecord = aRecord->next;
        } /* endif */
     } /* endwhile */
       /* no matching entry was found. create a new one */
     if (_firstServerRecord != NULL) {
        aRecord->next = (serverRecord *)SOMMalloc( sizeof (serverRecord) );
        aRecord = aRecord->next;
     } else {
        _firstServerRecord = (serverRecord *)SOMMalloc( sizeof (serverRecord) );
        aRecord=_firstServerRecord;
     } /* endif */
   aRecord->next = NULL;
   aRecord->alias= (char *)SOMMalloc (strlen(branchId) + 1);
   strcpy( aRecord->alias, branchId);
   _nameOfServer(_theSVRBroker, ev, branchId, &newAlias);
    if (check(*ev)) {
        return((SOMDServer) NULL);
    }
   aRecord->newAlias= (char *)SOMMalloc (strlen(newAlias) +1);
   strcpy( aRecord->newAlias, newAlias);
   ORBfree(newAlias);
   newAlias=NULL;
   aRecord->server = _somdFindServerByName(SOMD_ObjectMgr, ev, aRecord->newAlias);
    if (check(*ev)) {
        aRecord->server = NULL;
        return((SOMDServer) NULL);
    }
   aRecord->theAccountsManager = _somdCreateObj(aRecord->server,  ev,"AccountsManager", " ");
   if (check(*ev)){
       aRecord->theAccountsManager = NULL;
       return((SOMDServer) NULL);
   }
   aRecord->theSavingsAccountClass = _somdGetClassObj(aRecord->server,
                                               ev,
                                              "SavingsAccount");
    if (check(*ev)) {
       aRecord->theSavingsAccountClass = NULL;
       return((SOMDServer) NULL);
   }
   aRecord->theCheckingAccountClass = _somdGetClassObj(aRecord->server,
                                               ev,
                                              "CheckingAccount");
    if (check(*ev)) {
       aRecord->theCheckingAccountClass = NULL;
       return((SOMDServer) NULL);
   }
   _theAccountsManager = aRecord->theAccountsManager;
   _theCheckingAccountClass = aRecord->theCheckingAccountClass;
   _theSavingsAccountClass =  aRecord->theSavingsAccountClass;
   return aRecord->server;
}

SOM_Scope void  SOMLINK freeBranchServerArray(ClientClass somSelf,
                                               Environment *ev)
{
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","freeBranchServerArray");

   if (_firstServerRecord != NULL) {
       _freeServerRecord(somSelf, ev, _firstServerRecord);
       check(*ev);
   } /* endif */
}


SOM_Scope void  SOMLINK freeServerRecord(ClientClass somSelf,
                                          Environment *ev, ClientClass_serverRecord* aRecord)
{
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","freeServerRecord");

   if (aRecord->next != NULL) {
      _freeServerRecord( somSelf, ev, aRecord->next );
   }

   if (aRecord->theAccountsManager){
      _somdDestroyObject(SOMD_ObjectMgr,ev,aRecord->theAccountsManager);
   }

   if (aRecord->theSavingsAccountClass) {
       _somdProxyFree(aRecord->theSavingsAccountClass, ev);
   }

   if (aRecord->theCheckingAccountClass){
       _somdProxyFree(aRecord->theCheckingAccountClass, ev);
   }

   if (aRecord->server){
      _somdProxyFree(aRecord->server,ev);
   }

   if (aRecord->newAlias) {
      _releaseServer(_theSVRBroker, ev, aRecord->newAlias);
   }

   SOMFree( aRecord->alias);
   aRecord->alias=NULL;
   SOMFree( aRecord->newAlias);
   aRecord->newAlias=NULL;
   SOMFree( aRecord);
   aRecord=NULL;
}

SOM_Scope long  SOMLINK createSavingsAccount(ClientClass somSelf,
                                             Environment *ev,
                                             double balance)
{
    SavingsAccount   aSavingsAccount;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","createSavingsAccount");

    if (_aCustomer == NULL) {
        return( BAD_CUSTOMER);
    } else {
        _getBranchServer(somSelf, ev, _CustomerBranchID);
        if (check(*ev)) {
            somExceptionFree(ev);
            return(SOM_ENVIRONMENT) ;
        }
        aSavingsAccount = SavingsAccountMetaClass_create (_theSavingsAccountClass,
                                                          ev,
                                                          _CustomerID,
                                                          balance);
        if (check(*ev)) {
            somExceptionFree(ev);
            aSavingsAccount=NULL;
            return(CREATE_ERROR) ;
        }
        if (aSavingsAccount != NULL) {
            Account_save(aSavingsAccount, ev);
            _somdDestroyObject(SOMD_ObjectMgr, ev, aSavingsAccount);
        }
    }
    return OK;
}

SOM_Scope long  SOMLINK createCheckingAccount(ClientClass somSelf,
                                               Environment *ev,
                                              double balance)
{
    CheckingAccount   aCheckingAccount;
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","createCheckingAccount");

    if (_aCustomer == NULL) {
        return(BAD_CUSTOMER);
    } else {
        _getBranchServer(somSelf, ev, _CustomerBranchID);
        if (check(*ev)){
            somExceptionFree(ev);
            return(SOM_ENVIRONMENT) ;
        }
        aCheckingAccount = CheckingAccountMetaClass_create (_theCheckingAccountClass,
                                                            ev,
                                                            _CustomerID,
                                                            balance);
        if (check(*ev)) {
            somExceptionFree(ev);
            aCheckingAccount = NULL;
            return(CREATE_ERROR);
        }
        if (aCheckingAccount != NULL){
            Account_save(aCheckingAccount, ev);
            _somdDestroyObject(SOMD_ObjectMgr, ev, aCheckingAccount);
        }
    }
    return OK;
}


SOM_Scope long SOMLINK createCustomer(ClientClass somSelf,
                                              Environment *ev,
                                              string customerName,
                                              string branchID,
                                              long *customerPIN)
{
    long retCode;
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","createCustomer");

    _freeClient(somSelf, ev);

    _aCustomer = CustomerMetaClass_create (_theCustomerClass,
                                           ev,
                                           customerName,
                                           branchID);
    if (check(*ev)){
        somExceptionFree(ev);
        _aCustomer = NULL;
        return(CREATE_ERROR);
    }

    retCode = Customer_save(_aCustomer, ev);
    if (check(*ev)){
        somExceptionFree(ev);
        return(retCode) ;
    }
    _CheckingAccountID = (long) NULL;
    _SavingsAccountID = (long) NULL;

    if (_CustomerBranchID) {
        SOMFree(_CustomerBranchID);
    } /* endif */
    _CustomerBranchID = __get_branch(_aCustomer, ev);
    if (check(*ev)){
        somExceptionFree(ev);
        _CustomerBranchID=NULL;
        return(BAD_CUSTOMER) ;
    }

    if (_CustomerName) {
        SOMFree(_CustomerName);
    } /* endif */
    _CustomerName = __get_name(_aCustomer, ev);
    if (check(*ev)){
        somExceptionFree(ev);
        _CustomerName=NULL;
        return(BAD_CUSTOMER) ;
    }

    _CustomerID = __get_identifier(_aCustomer, ev);
    if (check(*ev)) {
        somExceptionFree(ev);
        _CustomerID = (long) NULL;
        return(BAD_CUSTOMER) ;
    }
    *customerPIN = __get_pin(_aCustomer,ev);
    if (check(*ev)){
        somExceptionFree(ev);
        *customerPIN = (long) NULL;
        return(BAD_CUSTOMER) ;
    }
    return OK;
}


SOM_Scope long  SOMLINK restoreCustomer(ClientClass somSelf,
                                         Environment *ev, long CustomerID)
{

    int i;
    static   _IDL_SEQUENCE_long  accountIds, accountTypes;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","findCustomer");

    _freeClient(somSelf, ev);
    _aCustomer = CustomerMetaClass_restore ( _theCustomerClass,
                                              ev,
                                              CustomerID);
    if (check(*ev)) {
        somExceptionFree(ev);
        _aCustomer=NULL;
        return(RESTORE_ERROR);
    }
    if (__get_identifier(_aCustomer, ev) == 0L)
       return(RESTORE_ERROR);

    _CustomerID = __get_identifier(_aCustomer, ev);
    _CustomerName = __get_name(_aCustomer,ev);
    _CustomerBranchID = __get_branch(_aCustomer,ev);

    _getBranchServer(somSelf, ev, _CustomerBranchID);
    if (check(*ev)) {
        somExceptionFree(ev);
        return(SOM_ENVIRONMENT);
    }

   /*********************************/
   /* get the accounts            */
   /*********************************/
   accountIds._maximum = accountIds._length  = 0;
   accountTypes._maximum = accountTypes._length  = 0;
   accountIds._buffer=NULL;
   accountTypes._buffer=NULL;
   if (_theAccountsManager) {
        _findAccountsByOwner(_theAccountsManager, ev, _CustomerID, &accountIds, &accountTypes);
        if (check(*ev)) {
            somExceptionFree(ev);
            return(RESTORE_ERROR);
        }
   } else {
            return(RESTORE_ERROR);
   } /* endif */
   for (i=0;i<(int) accountIds._length ;i++ ) {
         switch (accountTypes._buffer[i])
         {
         case Account_CHECKING:
                  _CheckingAccountID = accountIds._buffer[i];
                  break;
          case Account_SAVINGS:
                   _SavingsAccountID = accountIds._buffer[i];
                   break;
          } /*switch*/
   } /*end for*/
   ORBfree(&accountIds);
   ORBfree(&accountTypes);
   return OK;
}


SOM_Scope long  SOMLINK deleteCustomer(ClientClass somSelf,  Environment *ev,
                                       long CustomerID)
{
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","deleteCustomer");

    if (_aCustomer) {
        if (__get_identifier(_aCustomer, ev) != CustomerID) {
            return (BAD_CUSTOMER);
        }
    } else {
        _restoreCustomer(somSelf,ev,CustomerID);
        if (check(*ev)) {
            somExceptionFree(ev);
            return(CREATE_ERROR) ;
        }
    }

    if (_CheckingAccountID) {
        ClientClass_deleteAccount(somSelf,ev,
                                  CustomerID,
                                   _CheckingAccountID);
        if (check(*ev)) {
            somExceptionFree(ev);
            return(DELETE_ERROR) ;
        }
     }
     if (_SavingsAccountID) {
         ClientClass_deleteAccount(somSelf,ev,
                                   CustomerID,
                                   _SavingsAccountID);
         if (check(*ev)) {
            somExceptionFree(ev);
            return(DELETE_ERROR) ;
         }
     }
     if (_aCustomer) {
          Customer_delete(_aCustomer,ev);
          if (check(*ev)) {
              somExceptionFree(ev);
              return(DELETE_ERROR) ;
          }
     }
    _freeClient(somSelf, ev);
    return OK;

}


SOM_Scope void  SOMLINK freeClient(ClientClass somSelf,
                                              Environment *ev)
{

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","freeClient");

    if (_aCustomer != NULL) {
       _somdTargetFree(_aCustomer,ev);
       _somdProxyFree(_aCustomer,ev);
       _aCustomer=NULL;
       _CheckingAccountID = (long) NULL;
       _SavingsAccountID = (long) NULL;
       _CustomerID = (long) NULL;
       SOMFree(_CustomerName);
       _CustomerName=NULL;
       SOMFree(_CustomerBranchID);
       _CustomerBranchID=NULL;
    }
   return ;

}


SOM_Scope long  SOMLINK restoreATMCustomer(ClientClass somSelf,
                                           Environment *ev,
                                           long CustomerID, long CustomerPIN)
{
    int i;
    static   _IDL_SEQUENCE_long  accountIds, accountTypes;

    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","createATMCustomer");

    _freeClient(somSelf, ev);
    _aCustomer = CustomerMetaClass_restore ( _theCustomerClass,
                                                ev,
                                                CustomerID);
    if (check(*ev)) {
        somExceptionFree(ev);
        _aCustomer=NULL;
        return(NOT_FOUND) ;
    }
    if (__get_identifier(_aCustomer, ev) == 0L)
        return(BAD_CUSTOMER);
    if (!Customer_isValidPin(_aCustomer, ev, CustomerPIN))
        return (BAD_PIN);

    _CustomerID = __get_identifier(_aCustomer, ev);
    _CustomerName = __get_name(_aCustomer,ev);
    _CustomerBranchID = __get_branch(_aCustomer,ev);

    _getBranchServer(somSelf, ev,_CustomerBranchID);
    if (check(*ev)) {
        somExceptionFree(ev);
        return(SOM_ENVIRONMENT) ;
    }
    /*********************************/
    /* get the accounts            */
    /*********************************/
    accountIds._maximum = accountIds._length  = 0;
    accountTypes._maximum = accountTypes._length  = 0;
    accountIds._buffer=NULL;
    accountTypes._buffer=NULL;
    _findAccountsByOwner(_theAccountsManager, ev, CustomerID, &accountIds, &accountTypes);
    if (check(*ev)){
        somExceptionFree(ev);
        return(NOT_FOUND) ;
    }

   for (i=0;i<(int) accountIds._length ;i++ ) {
         switch (accountTypes._buffer[i])
         {
         case Account_CHECKING:
                  _CheckingAccountID = accountIds._buffer[i];
                  break;
         case Account_SAVINGS:
                  _SavingsAccountID = accountIds._buffer[i];
                   break;
        } /*switch*/
   } /*end for*/
   ORBfree(&accountIds);
   ORBfree(&accountTypes);
   return OK;
}

SOM_Scope long  SOMLINK deleteAccount(ClientClass somSelf,  Environment *ev,
                                      long customerID, long accountID)
{
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","deleteAccount");

    if (_aCustomer) {
        if (__get_identifier(_aCustomer, ev) != customerID) {
            return (BAD_CUSTOMER);
        }
    } else {
        _restoreCustomer(somSelf,ev,customerID);
         if (check(*ev)){
             somExceptionFree(ev);
             return(RESTORE_ERROR) ;
         }
    }
    if (_CheckingAccountID==accountID) {
        AccountsManager_deleteAccount(_theAccountsManager,ev,accountID);
        if (check(*ev)){
            somExceptionFree(ev);
            return(DELETE_ERROR) ;
         }
    }
    if (_SavingsAccountID==accountID) {
        AccountsManager_deleteAccount(_theAccountsManager,ev,accountID);
        if (check(*ev)){
            somExceptionFree(ev);
            return(DELETE_ERROR) ;
        }
    }
   return OK;
}



SOM_Scope long  SOMLINK deposit(ClientClass somSelf,  Environment *ev,
                                long AccountID, long AccountType,
                                double dAmount)
{
     static   Account             anAccount;

     ClientClassData *somThis = ClientClassGetData(somSelf);
     ClientClassMethodDebug("ClientClass","deposit");

     anAccount=NULL;
     switch (AccountType)
                 {
     case Account_CHECKING:
          anAccount=CheckingAccountMetaClass_restore(_theCheckingAccountClass,
                                                     ev,
                                                     AccountID);
          if (check(*ev)) {
              somExceptionFree(ev);
              anAccount=NULL;
              return(RESTORE_ERROR) ;
          }
          break;
     case Account_SAVINGS:
          anAccount=SavingsAccountMetaClass_restore(_theSavingsAccountClass,
                                                    ev,
                                                    AccountID);

          if (check(*ev)) {
              somExceptionFree(ev);
              anAccount=NULL;
              return(RESTORE_ERROR) ;
          }
          break;
     }

     _depositTo(anAccount, ev, dAmount);
     if (check(*ev)) {
         somExceptionFree(ev);
         return(DEPOSIT_ERROR) ;
     }
     Account_save(anAccount, ev);
     if (check(*ev)){
         somExceptionFree(ev);
         return(DEPOSIT_ERROR) ;
     }
     if (anAccount != NULL)
         _somdDestroyObject(SOMD_ObjectMgr, ev, anAccount);
     return(OK) ;
}


SOM_Scope long  SOMLINK withdraw(ClientClass somSelf,  Environment *ev,
                                 long AccountID, long AccountType,
                                 double dAmount)
{
     static   Account             anAccount;

     ClientClassData *somThis = ClientClassGetData(somSelf);
     ClientClassMethodDebug("ClientClass","withdraw");

     anAccount = NULL;
     switch (AccountType) {
     case Account_CHECKING:
          anAccount=CheckingAccountMetaClass_restore(_theCheckingAccountClass, ev, AccountID);
          if (check(*ev)) {
              somExceptionFree(ev);
              anAccount=NULL;
              return(RESTORE_ERROR) ;
          }
          break;
     case Account_SAVINGS:
          anAccount=SavingsAccountMetaClass_restore(_theSavingsAccountClass, ev, AccountID);
          if (check(*ev)) {
              somExceptionFree(ev);
              anAccount=NULL;
              return(RESTORE_ERROR) ;
          }
          break;
     }

     _withdrawFrom(anAccount, ev, dAmount);
     if (check(*ev)) {
         somExceptionFree(ev);
         return(WITHDRAW_ERROR) ;
     }
     Account_save(anAccount, ev);
     if (check(*ev)) {
         somExceptionFree(ev);
         return(WITHDRAW_ERROR) ;
     }

     if (anAccount != NULL)
         _somdDestroyObject(SOMD_ObjectMgr, ev, anAccount);

     return(OK) ;
}


SOM_Scope long  SOMLINK inquire(ClientClass somSelf,  Environment *ev,
                                long AccountID, long AccountType,
                                double* dCurrentBalance)
{
    Account             anAccount;
    ClientClassData *somThis = ClientClassGetData(somSelf);
    ClientClassMethodDebug("ClientClass","inquire");

    anAccount = NULL;
    switch (AccountType) {
    case Account_CHECKING:
         anAccount=CheckingAccountMetaClass_restore(_theCheckingAccountClass,
                                                    ev,
                                                    AccountID);
         if (check(*ev)) {
             somExceptionFree(ev);
             anAccount=NULL;
             return(RESTORE_ERROR) ;
         }
         break;
    case Account_SAVINGS:
         anAccount=SavingsAccountMetaClass_restore(_theSavingsAccountClass,
                                                   ev,
                                                   AccountID);
         if (check(*ev)) {
             somExceptionFree(ev);
             anAccount=NULL;
             return(RESTORE_ERROR) ;
         }
         break;
    }
    *dCurrentBalance = __get_balance(anAccount, ev);

    if (anAccount != NULL)
       _somdDestroyObject(SOMD_ObjectMgr, ev, anAccount);
    return(OK) ;
}
