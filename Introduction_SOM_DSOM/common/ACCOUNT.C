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
 *                                                                      *
 * File : account.c        Module : account.dll                         *
 *                                                                      *
 * Class : Account                                                      *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        logTransaction                     X                          *
 *        withdrawalIsValid                  X                          *
 *        withdrawFrom             X                                    *
 *        depositTo                X                                    *
 *        inquireAbout             X                                    *
 *        addToBalance                       X                          *
 *        takeFromBalance                    X                          *
 *        save                     X                                    *
 *        delete                   X                                    *
 *                                                                      *
 *  Class : AccountMetaClass                                            *
 *                                                                      *
 *    Method(s):                                                        *
 *                                                                      *
 *        create                   X                                    *
 *        restore                  X                                    *
 * ******************************************************************** *
 */
#define Account_Class_Source
#define AccountMetaClass_Class_Source

#include <time.h>
#include <somd.h>
#include "account.ih"
#include "banksvr.h"
#include "bankdef.h"
/*
 ****************************************************************************
 * Private method to record the transaction time of all valid transactions. *
 * The time since the EPOCH (Midnight, 1/1/1970) is used and is expressed   *
 * in seconds.  Used by the atomic routines that change the account's       *
 * balance.                                                                 *
 ****************************************************************************
 */
SOM_Scope void  SOMLINK logTransaction(Account     somSelf,
                                       Environment *ev)
{
    time_t      seconds;
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","logTransaction");

    _lastTransactionTime = (long) time(&seconds);
}
/*
 ****************************************************************************
 * Private method that decides whether the balance AFTER the withdrawal is  *
 * still above the required minimum balance.                                *
 ****************************************************************************
 */
SOM_Scope boolean  SOMLINK withdrawalIsValid(Account     somSelf,
                                             Environment *ev,
                                             double      amount)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","withdrawalIsValid");

    if ( _balance - amount < _minimumBalance ) return (FALSE);
    else return (TRUE);
}

/*
 ****************************************************************************
 * Public method to withdraw a given amount from an account provided that   *
 * the withdrawal is permitted, this condition is checked for.              *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK withdrawFrom(Account     somSelf,
                                     Environment *ev,
                                     double      amount)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","withdrawFrom");

    if ( _withdrawalIsValid(somSelf, ev, amount) ) {
       _takeFromBalance(somSelf, ev, amount);
       return (OK);
    } else {
       return (OVERDRAWN);
    }
}
/*
 ****************************************************************************
 * Public method to deposit a given amount to the account. Absolutely no    *
 * checks are made.                                                         *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK depositTo(Account     somSelf,
                                  Environment *ev,
                                  double      amount)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","depositTo");

    _addToBalance(somSelf, ev, amount);

    return (OK);
}
/*
 ****************************************************************************
 * Public method to inquire about the type and balance of the account. As   *
 * is, it just returns the type and balance without any further formatting. *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK inquireAbout(Account       somSelf,
                                     Environment   *ev,
                                     Account_TYPES *type,
                                     double*       balance)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","inquireAbout");

    *type    = _type;
    *balance = _balance;

    return (OK);
}
/*
 ****************************************************************************
 * Private atomic method to add a given amount to the balance of an account.*
 * No checks are made, but the transaction time gets logged.                *
 ****************************************************************************
 */
SOM_Scope void  SOMLINK addToBalance(Account     somSelf,
                                     Environment *ev,
                                     double      amount)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","addToBalance");

    _balance += amount;
    _logTransaction(somSelf, ev);
}

/*
 ****************************************************************************
 * Private atomic method to take a given amount from the balance of an      *
 * account. No checks are made, but the transaction time gets logged.       *
 ****************************************************************************
 */
SOM_Scope void  SOMLINK takeFromBalance(Account     somSelf,
                                        Environment *ev,
                                        double      amount)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","takeFromBalance");

    _balance -= amount;
    _logTransaction(somSelf, ev);
}

/*
 ****************************************************************************
 * Public method to make the state of the Account object persistent.        *
 * Ideally, it should be immaterial, how persistency is implemented.        *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK save(Account     somSelf,
                             Environment *ev)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_AccountRecord AccountDBRecord;
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","saveInDataBase");

    AccountDBRecord.id                  = _accountID;
    AccountDBRecord.owner               = _owner;
    AccountDBRecord.type                = _type;
    AccountDBRecord.balance             = _balance;
    AccountDBRecord.minimumBalance      = _minimumBalance;
    AccountDBRecord.lastTransactionTime = _lastTransactionTime;

    theBranchDB = __get_theDB(SOMD_ServerObject, ev);

    rc = _updateAccount(theBranchDB, ev, &AccountDBRecord);
    if (check(*ev)) {
       somExceptionFree(ev);
    }
    if( rc == NOT_FOUND ) {
        rc = _insertAccount(theBranchDB, ev, &AccountDBRecord);
        if (check(*ev)) {
           somExceptionFree(ev);
        }
    }
    return (rc);
}

SOM_Scope long  SOMLINK delete(Account somSelf,  Environment *ev)
{
    AccountData *somThis = AccountGetData(somSelf);
    AccountMethodDebug("Account","delete");

    return _deleteAccount(__get_theDB(SOMD_ServerObject,ev), ev, _accountID);
}

/*
 ****************************************************************************
 * META CLASS METHOD:                                                       *
 *                                                                          *
 * Definition of the constructor of the Account objects. It also sets the   *
 * attributes (both public and private) of the object. The time of the      *
 * creation gets logged as the last transaction time. The minimum balance   *
 * is set to zero for all account types.                                    *
 ****************************************************************************
 */

SOM_Scope Account  SOMLINK create(AccountMetaClass somSelf,
                                  Environment      *ev,
                                  long             owner,
                                  double           balance)
{
    long accountID;
    Account theNewAccount;
    BranchDB theBranchDB;
    AccountMetaClassMethodDebug("AccountMetaClass","create");

    theNewAccount = _somNew(somSelf);               /* Not AccountNew()! */

    theBranchDB = __get_theDB(SOMD_ServerObject, ev);
    accountID  = BranchDB_getAvailableId(theBranchDB, ev);

    __set_accountID(theNewAccount, ev, accountID);
    __set_owner(theNewAccount, ev, owner);
    __set_balance(theNewAccount, ev, balance);
    __set_minimumBalance(theNewAccount, ev, 0.0);

    _logTransaction(theNewAccount, ev);

    return(theNewAccount);
}


SOM_Scope Account  SOMLINK restore(AccountMetaClass somSelf,
                                    Environment *ev, long accountID)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_AccountRecord AccountDBRecord;
    Account theAccount;

    AccountMetaClassMethodDebug("AccountMetaClass","restore");

    theAccount=_somNew(somSelf);
    theBranchDB = __get_theDB(SOMD_ServerObject, ev);
    if (check(*ev)){
        somExceptionFree(ev);
       return((Account)NULL);
    }

    rc = _retrieveAccount(theBranchDB, ev, accountID, &AccountDBRecord);
    if (check(*ev)) {
       somExceptionFree(ev);
    }
    if (accountID != AccountDBRecord.id ) {
        ORBfree(&AccountDBRecord);
        rc = READ_ERROR;
    }
    __set_accountID(theAccount,ev,AccountDBRecord.id);
    __set_owner(theAccount,ev,AccountDBRecord.owner);
    __set_type(theAccount,ev,AccountDBRecord.type);
    __set_balance(theAccount,ev,AccountDBRecord.balance);
    __set_minimumBalance(theAccount,ev,AccountDBRecord.minimumBalance);
    __set_lastTransactionTime(theAccount,ev,AccountDBRecord.lastTransactionTime);

    ORBfree(&AccountDBRecord);
    return (theAccount);
}

