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
 * File : savings.c        Module : account.dll                         *
 *                                                                      *
 * Class : SavingsAccount                                               *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        defineInterestRate       X                                    *
 *        computeInterestRate                X                          *
 *        applyInterest            X                                    *
 *        withdrawalIsValid                  X                          *
 *        withdrawFrom             X                                    *
 *        depositTo                X                                    *
 *        save                     X                                    *
 *        restore                  X                                    *
 *                                                                      *
 * Class : SavingsAccountMetaClass                                      *
 *                                                                      *
 *    Method(s):                                                        *
 *                                                                      *
 *        openAccount              X                                    *
 * ******************************************************************** *
 */

#define SavingsAccount_Class_Source
#define SavingsAccountMetaClass_Class_Source
#include <time.h>
#include <somd.h>
#include "savings.ih"
#include "banksvr.h"
#include "bankdef.h"
#include "sberr.h"
/*
 ****************************************************************************
 * Public method to set the interest rate for a savings account. It will be *
 * available to the clients.                                                *
 ****************************************************************************
 */

SOM_Scope long SOMLINK defineInterestRate(SavingsAccount somSelf,
                                          Environment    *ev,
                                          double         rate)
{
    BankErr_EXCP *Bank_Excp;

    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "defineInterestRate");

    if (rate < 0.0 || rate > 0.5) {
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = SA_INT_RATE_ERR;
        strcpy(Bank_Excp->reason,SA_INT_RATE_ERR_TXT);
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
        return INT_RATE;
    } else {
        _set_interestRate(somSelf, ev, rate);
        return (OK);
    }
}
/*
 ****************************************************************************
 * This private method computes but does not apply the interest             *
 * earned on the savings account since the last transaction. The interest   *
 * rate is expressed as a yearly rate but is compounded daily. Note the     *
 * comment below about the way we rescale the time to compute the number of *
 * days passed.                                                             *
 ****************************************************************************
 */

SOM_Scope double SOMLINK computeInterest(SavingsAccount somSelf,
                                         Environment    *ev)
{
    time_t atThisTime, atThatTime;
    long i, numberOfDays;
    double dailyRate,
    interest = 1.0;

    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "computeInterest");

    atThisTime   = time(&atThisTime);
    atThatTime   = __get_lastTransactionTime(somSelf, ev);
    /*
     *----------------------------------------------------------------------*
     * We are cheating here. In order to have appreciable interest,         *
     * we rescale the time by a factor such that one minute is seen as      *
     * one day.                                                             *
     *----------------------------------------------------------------------*
     */
    numberOfDays = (atThisTime - atThatTime)/60;
    dailyRate    = __get_interestRate(somSelf, ev) / 365.0;

    for (i = 0; i < numberOfDays; i++)
         interest *= 1.0 + dailyRate;

    return (__get_balance(somSelf, ev) * (interest - 1.0));
}

/*
 ****************************************************************************
 * Private  method to apply the interest rate computed by                   *
 * computeInterest() to the current balance of the account.                 *
 ****************************************************************************
 */

SOM_Scope void SOMLINK applyInterest(SavingsAccount somSelf,
                                     Environment    *ev)
{
    double earnedInterest;
    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "applyInterest");

    if (earnedInterest=_computeInterest(somSelf,ev)) {

       _addToBalance(somSelf, ev, earnedInterest);   /* Only if interest has been earned
                                                      * This avoids last transaction time
                                                      * from being updated when one time
                                                      * unit has not been passed.
                                                      */
    } /* endif */
}

/*
 ****************************************************************************
 * Private method overrides the one of the parent class. It computes the    *
 * interest earned to see if the requested new balance possibly drops       *
 * below the required minimum balance even if interest earned is            *
 * compounded first.                                                        *
 ****************************************************************************
 */

SOM_Scope boolean SOMLINK withdrawalIsValid(SavingsAccount somSelf,
                                            Environment    *ev,
                                            double          amount)
{
    double balance, interest, minimum;

    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "withdrawalIsValid");

    balance  = __get_balance(somSelf, ev);
    interest = _computeInterest(somSelf, ev);
    minimum  = __get_minimumBalance(somSelf, ev);

    if ((balance + interest - amount) < minimum)
        return (FALSE);
    else
        return (TRUE);
}
SOM_Scope long  SOMLINK delete(SavingsAccount somSelf,  Environment *ev)
{
    long rc;
    SavingsAccountMethodDebug("SavingsAccount","delete");

    rc = SavingsAccount_parent_Account_delete(somSelf, ev);
    if (rc) return rc;
    return _deleteInterest(__get_theDB(SOMD_ServerObject,ev),
                           ev,
                           __get_accountID(somSelf,ev));
}

/*
 ****************************************************************************
 * Public method to withdraw a given amount from an account. Works as in    *
 * the parent class, except that it compounds the interest earned first.    *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK withdrawFrom(SavingsAccount somSelf,
                                     Environment    *ev,
                                     double         amount)
{
    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "withdrawFrom");

    if (_withdrawalIsValid(somSelf, ev, amount)) {
        _applyInterest(somSelf, ev);
        _takeFromBalance(somSelf, ev, amount);
        return (OK);
    } else {
        return (OVERDRAWN);
    }
}
/*
 ****************************************************************************
 * Public method to deposit a given amount to an account. Works as in the   *
 * parent class, except that it compounds the interest earned first.        *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK depositTo(SavingsAccount somSelf,
                                  Environment    *ev,
                                  double         amount)
{
    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount", "depositTo");

    _applyInterest(somSelf, ev);
    SavingsAccount_parent_Account_depositTo(somSelf, ev, amount);


    return (OK);
}
/*
 ****************************************************************************
 * This public  method overrides the one provided by the parent class. It   *
 * does call it, however, to save the instance data defined by the parent   *
 * class. If successful,  it proceeds to store the instance data specific   *
 * to the child class. The return codes are passed on to the caller.        *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK save(SavingsAccount somSelf,
                             Environment    *ev)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_InterestRecord InterestDBRecord;
    SavingsAccountData *somThis = SavingsAccountGetData(somSelf);
    SavingsAccountMethodDebug("SavingsAccount","saveInDataBase");

    rc = SavingsAccount_parent_Account_save(somSelf, ev);
    if ( rc != OK ) return (rc);

    InterestDBRecord.id           = __get_accountID(somSelf, ev);
    InterestDBRecord.interestRate = _interestRate;
    theBranchDB = __get_theDB(SOMD_ServerObject, ev);

    rc = _updateInterest(theBranchDB, ev, &InterestDBRecord);
    if (check(*ev)) somExceptionFree(ev);
    if( rc == NOT_FOUND ) {
       rc = _insertInterest(theBranchDB, ev, &InterestDBRecord);
       if (check(*ev)) somExceptionFree(ev);
    }

    return (rc);
}
/*
 ****************************************************************************
 * META CLASS METHOD:                                                       *
 *                                                                          *
 * This public constructor method overrides the one provided by the parent  *
 * class (which is itself a meta class). It does call it, however, to       *
 * create the instance and initialize instance data defined by the parent   *
 * class. Then it proceeds to create the instance data specific to the      *
 * child class.                                                             *
 *                                                                          *
 * This method could have been implemented to accept an interest rate       *
 * paramter and set the rate here.  However, we chose to implement the      *
 * setting of the interest rate in a different method.                      *
 ****************************************************************************
 */
SOM_Scope Account  SOMLINK create(SavingsAccountMetaClass somSelf,
                                   Environment *ev, long owner,
                                  double balance)
{
    double defaultInterestRate = 0.05;
    SavingsAccount theNewAccount;

    /* SavingsAccountMetaClassData *somThis = SavingsAccountMetaClassGetData(somSelf); */
    SavingsAccountMetaClassMethodDebug("SavingsAccountMetaClass","create");

    theNewAccount=SavingsAccountMetaClass_parent_AccountMetaClass_create(somSelf,
                                                                   ev,
                                                                   owner,
                                                                   balance);
    __set_type(theNewAccount,ev,Account_SAVINGS);
    _defineInterestRate(theNewAccount, ev, defaultInterestRate);

    return (theNewAccount);
}

SOM_Scope Account  SOMLINK restore(SavingsAccountMetaClass somSelf,
                                    Environment *ev, long accountID)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_InterestRecord InterestDBRecord;
    SavingsAccount theAccount;

    SavingsAccountMetaClassMethodDebug("SavingsAccountMetaClass","restore");

    theAccount = SavingsAccountMetaClass_parent_AccountMetaClass_restore(somSelf, ev, accountID);

    theBranchDB = __get_theDB(SOMD_ServerObject, ev);
    if (check(*ev)) somExceptionFree(ev);

    rc = _retrieveInterest(theBranchDB, ev, accountID, &InterestDBRecord);
    if (check(*ev)) somExceptionFree(ev);

    __set_interestRate(theAccount,ev,InterestDBRecord.interestRate);

    ORBfree(&InterestDBRecord);
    return(theAccount);
}
