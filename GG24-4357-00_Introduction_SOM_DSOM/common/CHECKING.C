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
 * File : checking.c       Module : account.dll                         *
 *                                                                      *
 * Class : CheckingAccount                                              *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        defineMinimumBalance     X                                    *
 *        defineFee                X                                    *
 *        computeFee                         X                          *
 *        applyFee                           X                          *
 *        withdrawalIsValid                  X                          *
 *        withdrawFrom             X                                    *
 *        depositTo                X                                    *
 *        save                     X                                    *
 *        restore                  X                                    *
 *        displaySelf              X                                    *
 *        somInit                  X                                    *
 *        somUninit                X                                    *
 *                                                                      *
 * Class : CheckingAccountMetaClass                                     *
 *                                                                      *
 *    Method(s):                Public     Private                      *
 *                                                                      *
 *        openAccount              X                                    *
 * ******************************************************************** *
 */
#define CheckingAccount_Class_Source
#define CheckingAccountMetaClass_Class_Source
#include "checking.ih"
#include "bankdef.h"
#include "banksvr.h"
/*
 ****************************************************************************
 * This public method changes the value of an attribute defined in the      *
 * parent class. Since the default value supplied there might not be        *
 * desired in this child class, we provide a method to change it.           *
 ****************************************************************************
 */

SOM_Scope long  SOMLINK defineMinimumBalance(CheckingAccount somSelf,
                                             Environment     *ev,
                                             double minimum)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","defineMinimumBalance");

    __set_minimumBalance(somSelf, ev, minimum);

    return(OK);
}

/*
 ****************************************************************************
 * Public method to change the contents of the fee object instantiated in   *
 * the child class.                                                         *
 ****************************************************************************
 */
SOM_Scope long SOMLINK defineFee(CheckingAccount somSelf,
                                 Environment     *ev,
                                 Fee             fee)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","defineFee");

    __set_fee(somSelf, ev, fee );

    return(OK);
}

/*
 ****************************************************************************
 * Private method to compute the fee associated with a certain balance      *
 * given in the parameter amount. It just passes the request to the Fee     *
 * object that can handle it.                                               *
 ****************************************************************************
 */
SOM_Scope double  SOMLINK computeFee(CheckingAccount somSelf,
                                     Environment     *ev,
                                     double          amount)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","computeFee");

    return (Fee_computeFee(_fee, ev, amount));
}

/*
 ****************************************************************************
 * Private method not only computes but also applies the fee to the current *
 * balance.                                                                 *
 ****************************************************************************
 */
SOM_Scope void  SOMLINK applyFee(CheckingAccount somSelf,
                                 Environment     *ev)
{
    double balance, fee;
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","applyFee");

    balance = __get_balance(somSelf, ev);
    fee     = CheckingAccount_computeFee(somSelf, ev, balance);

    _takeFromBalance(somSelf, ev, fee);
}
/*
 ****************************************************************************
 * This private method overrides the one of the parent class. It computes   *
 * the fee that will be applied if the transaction completes successfully   *
 * and checks whether the balance reduced by both the amount withdrawn and  *
 * the fee incurred is still above the required minimum balance.            *
 ****************************************************************************
 */
SOM_Scope boolean  SOMLINK withdrawalIsValid(CheckingAccount somSelf,
                                             Environment     *ev,
                                             double          amount)
{
    double balance, fee, minimum;
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","withdrawalIsValid");

    balance  = __get_balance(somSelf, ev);
    fee      = CheckingAccount_computeFee(somSelf, ev, balance-amount);
    minimum  = __get_minimumBalance(somSelf, ev);

    if ( (balance-fee-amount) < minimum )
        return (FALSE);
    else
        return (TRUE);
}

SOM_Scope long  SOMLINK delete(CheckingAccount somSelf,  Environment *ev)
{
    long rc;

    CheckingAccountMethodDebug("CheckingAccount","delete");

    rc= CheckingAccount_parent_Account_delete(somSelf, ev);
    if (rc) return rc;
    return _deleteFee(__get_theDB(SOMD_ServerObject,ev), ev, __get_accountID(somSelf,ev));
}


/*
 ****************************************************************************
 * Public method to withdraw a given amount from an account. Works as in    *
 * the parent class, except that it applies the fee, if any,                *
 * subsequently.                                                            *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK withdrawFrom(CheckingAccount somSelf,
                                     Environment     *ev,
                                     double          amount)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","withdrawFrom");

    if ( _withdrawalIsValid(somSelf, ev, amount) ) {
        _takeFromBalance(somSelf, ev, amount);
        _applyFee(somSelf, ev);
        return(OK);
    } else {
        somPrintf("\n==> Withdrawal request denied.\n");
        return(OVERDRAWN);
    }
}
/*
 ****************************************************************************
 * Public method to deposit a given amount to an account. Works as in the   *
 * parent class, except that it also applies the fee, if any,               *
 * subsequently.                                                            *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK depositTo(CheckingAccount somSelf,
                                  Environment     *ev,
                                  double          amount)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","depositTo");

    CheckingAccount_parent_Account_depositTo(somSelf, ev, amount);
    _applyFee(somSelf, ev);

    return(OK);
}
/*
 ****************************************************************************
 * This public method overrides the one provided by the parent class. It    *
 * does call it, however, to save the instance data defined by the parent   *
 * class. If successful,  it proceeds to store the instance data specific   *
 * to the child class. The return codes are passed on to the caller. Note   *
 * the way we use the atomic routines provided by the persistency service   *
 * to ensure that the object is saved irrespective whether or not it has    *
 * been saved before.                                                       *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK save(CheckingAccount somSelf,
                             Environment     *ev)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_FeeRecord FeeDBRecord;
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","save");

    rc = CheckingAccount_parent_Account_save(somSelf, ev);
    if ( rc != OK ) return (rc);

    FeeDBRecord.id         = __get_accountID(somSelf, ev);
    FeeDBRecord.fee        = __get_sum(_fee, ev);
    FeeDBRecord.limit      = __get_limit(_fee, ev);
    FeeDBRecord.count      = __get_count(_fee, ev);

   theBranchDB = __get_theDB(SOMD_ServerObject, ev);

    rc = _updateFee(theBranchDB, ev, &FeeDBRecord);
    if (check(*ev)) {
       somExceptionFree(ev);
    }
    if( rc == NOT_FOUND ) {
        rc = _insertFee(theBranchDB, ev, &FeeDBRecord);
        if (check(*ev)) {
           somExceptionFree(ev);
        }
    }
    return (rc);
}


SOM_Scope void  SOMLINK somInit(CheckingAccount somSelf)
{
    Fee fee;
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","somInit");

    CheckingAccount_parent_Account_somInit(somSelf);

    fee = FeeNew();
    _defineFee(somSelf, somGetGlobalEnvironment(), fee);
}

SOM_Scope void  SOMLINK somUninit(CheckingAccount somSelf)
{
    CheckingAccountData *somThis = CheckingAccountGetData(somSelf);
    CheckingAccountMethodDebug("CheckingAccount","somUninit");

    if (_fee){
       _somFree(_fee);
       _fee=NULL;
       }

    CheckingAccount_parent_Account_somUninit(somSelf);
}

/*
 ****************************************************************************
 * META CLASS METHOD:                                                       *
 *                                                                          *
 * This public constructor method overrides the one provided by the parent  *
 * class (which is itself a meta class). It does call it, however, to       *
 * create the instance and initialize instance data defined by the parent   *
 * class. Then it proceeds to create the instance data specific to the      *
 * child class. The required minimum balance is set to a default value of   *
 * $-1000.00. The default value of the fee object, set at its               *
 * initialization is kept and passed on by the defineFee() method.          *
 ****************************************************************************
 */
SOM_Scope Account  SOMLINK create(CheckingAccountMetaClass somSelf,
                                   Environment *ev, long owner,
                                  double balance)
{

    CheckingAccount     theNewAccount;

    /* CheckingAccountMetaClassData *somThis = CheckingAccountMetaClassGetData(somSelf); */
    CheckingAccountMetaClassMethodDebug("CheckingAccountMetaClass","create");

    theNewAccount= CheckingAccountMetaClass_parent_AccountMetaClass_create(somSelf,
                                                                    ev,
                                                                    owner,
                                                                    balance);
    __set_type(theNewAccount,ev,Account_CHECKING);
    _defineMinimumBalance(theNewAccount, ev, -1000.0);

    return (theNewAccount);
}
/*
 ****************************************************************************
 * This pubic method overrides the one provided by the parent class. It     *
 * does call it, however, to restore the instance data defined by the       *
 * parent class. If successful,  it proceeds to restore the instance data   *
 * specific to the child class. The return codes are passed on to the       *
 * caller.                                                                  *
 ****************************************************************************
 */

SOM_Scope Account  SOMLINK restore(CheckingAccountMetaClass somSelf,
                                    Environment *ev, long accountID)
{
    long rc;
    BranchDB theBranchDB;
    BranchDB_FeeRecord FeeDBRecord;
    CheckingAccount theAccount;

    CheckingAccountMetaClassMethodDebug("CheckingAccountMetClass","restore");

    theAccount = CheckingAccountMetaClass_parent_AccountMetaClass_restore(somSelf, ev, accountID);

    theBranchDB = __get_theDB(SOMD_ServerObject, ev);
    if (check(*ev)) somExceptionFree(ev);

    rc = _retrieveFee(theBranchDB, ev, accountID, &FeeDBRecord);
    if (check(*ev)) somExceptionFree(ev);

    __set_sum(__get_fee(theAccount,ev), ev, FeeDBRecord.fee);
    __set_limit(__get_fee(theAccount,ev), ev, FeeDBRecord.limit);
    __set_count(__get_fee(theAccount,ev), ev, FeeDBRecord.count);

    return (theAccount);
}
