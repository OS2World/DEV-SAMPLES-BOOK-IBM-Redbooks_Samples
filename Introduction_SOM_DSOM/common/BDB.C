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
 * File : bdb.c            Module : bdb.dll                             *
 *                                                                      *
 * Class : BranchDB                                                     *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        insertAccount            X                                    *
 *        retrieveAccount          X                                    *
 *        updateAccount            X                                    *
 *        deleteAccount            X                                    *
 *        getAvailableID           X                                    *
 *        selectAccountsByCustomerID  X                                 *
 *        insertFee                X                                    *
 *        retrieveFee              X                                    *
 *        updateFee                X                                    *
 *        deleteFee                X                                    *
 *        insertInterest           X                                    *
 *        retrieveInterest         X                                    *
 *        updateInterest           X                                    *
 *        deleteInterest           X                                    *
 *        somInit                  X                                    *
 *        somUninit                x                                    *
 *                                                                      *
 * ******************************************************************** *
 */
#define BranchDB_Class_Source
#define SOMMSingleInstance_Class_Source

#include <bdb.ih>
#include <somd.h>
#include "bankdef.h"

/*
 * method to record an Account Record in the Database.
 */

SOM_Scope long  SOMLINK insertAccount(BranchDB               somSelf,
                                      Environment            *ev,
                                      BranchDB_AccountRecord *anAccount)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","insertAccount");


    _accountAny._value = (void *)anAccount;
    return _fileUtilityInsert(_fp1, ev, &_accountAny);
}

/*
 * method to retrieve a Account information given its Id.
 */


SOM_Scope long  SOMLINK retrieveAccount(BranchDB               somSelf,
                                        Environment            *ev,
                                        long                   AccountId,
                                        BranchDB_AccountRecord *aRecord)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","retrieveAccount");
    _accountAny._value = (void *)aRecord;
    return _fileUtilityRetrieve(_fp1, ev, AccountId, &_accountAny);
}

/*
 * method to update a record.
 */

SOM_Scope long  SOMLINK updateAccount(BranchDB               somSelf,
                                      Environment            *ev,
                                      BranchDB_AccountRecord *anAccount)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","updateAccount");
    _accountAny._value = (void *)anAccount;
    return _fileUtilityUpdate(_fp1, ev, &_accountAny);
}

/*
 * method to delete a record.
 */

SOM_Scope long  SOMLINK deleteAccount(BranchDB somSelf,  Environment *ev,
                                      long AccountId)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","deleteAccount");

    return _fileUtilityDelete(_fp1, ev, AccountId);
}

/*
 * returns to the caller an available Id for a new Account
 */

SOM_Scope long  SOMLINK getAvailableId(BranchDB somSelf,
                                       Environment *ev)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","getAvailableId");

    if (_lastId==0) {
        /*
         * first call to getAvailableId since initialization
         */
        return _lastId = _fileUtilityGiveLastKey(_fp1, ev)+1;
    } else {
        return ++_lastId ;
    } /* endif */
}

/*
 * searches the account table for accounts matching the
 * given customerId. returns NOT_FOUND if there is no match.
 * sets count to the number of entries found in the file and
 * copies the information found in arrayOfAccounts
 * This function calling style was prefered to the cursor-select
 * style of SQL as we will not deal with large amounts of
 * accounts.
 */

SOM_Scope long  SOMLINK selectAccountsByCustomerId(BranchDB    somSelf,
                                                   Environment *ev,
                                                   long        aCustomerId,
                                                   long        *count,
                                                   long        arrayOfAccountIds[MAX_ACCTS])
{

    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","selectAccountsByCustomerId");

    return _fileUtilitySelect(_fp1, ev, "owner", (void *)&aCustomerId, count, arrayOfAccountIds);
}

/*
 * method to record a Feein the file.
 */

SOM_Scope long  SOMLINK insertFee(BranchDB           somSelf,
                                  Environment        *ev,
                                  BranchDB_FeeRecord *aFee)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","insertFee");

    _feeAny._value = (void *)aFee;
    return _fileUtilityInsert( _fp2, ev, &_feeAny );
}

/*
 * method to retrieve a Fee record  given the corresponding
 * Account Id.
 */

SOM_Scope long  SOMLINK retrieveFee(BranchDB           somSelf,
                                    Environment        *ev,
                                    long               AccountId,
                                    BranchDB_FeeRecord *aRecord)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","retrieveFee");

    _feeAny._value = (void *)aRecord;
    return _fileUtilityRetrieve( _fp2, ev, AccountId, &_feeAny);
}

/*
 * method to update a record.
 */

SOM_Scope long  SOMLINK updateFee(BranchDB           somSelf,
                                  Environment        *ev,
                                  BranchDB_FeeRecord *aFee)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","updateFee");

    _feeAny._value = (void *)aFee;
    return _fileUtilityUpdate( _fp2, ev, &_feeAny);
}

/*
 * method to delete a Fee record.
 */

SOM_Scope long  SOMLINK deleteFee(BranchDB somSelf,  Environment *ev,
                                  long AccountId)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","deleteFee");

    return _fileUtilityDelete(_fp2, ev, AccountId);
}

/*
 * method to record an interest object in the file.
 */

SOM_Scope long  SOMLINK insertInterest(BranchDB                somSelf,
                                       Environment             *ev,
                                       BranchDB_InterestRecord *anInterest)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","insertInterest");

    _interestAny._value = (void *)anInterest;
    return _fileUtilityInsert( _fp3, ev, &_interestAny );
}

/*
 * method to retrieve an Interest record  given the corresponding
 * Account Id.
 */

SOM_Scope long  SOMLINK retrieveInterest(BranchDB                somSelf,
                                         Environment             *ev,
                                         long                    AccountId,
                                         BranchDB_InterestRecord *aRecord)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","retrieveInterest");

    _interestAny._value = (void *)aRecord;
    return _fileUtilityRetrieve( _fp3, ev, AccountId, &_interestAny);
}

/*
 * method to update a record. returns NOT_FOUND if no record
 * exits with the given Account Id.
 */

SOM_Scope long  SOMLINK updateInterest(BranchDB                somSelf,
                                       Environment             *ev,
                                       BranchDB_InterestRecord *anInterest)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","updateInterest");

    _interestAny._value = (void *)anInterest;
    return _fileUtilityUpdate( _fp3, ev, &_interestAny) ;
}

/*
 * method to delete a record. returns NOT_FOUND if no record
 * exits with the given Account Id.
 */

SOM_Scope long  SOMLINK deleteInterest(BranchDB somSelf,  Environment *ev,
                                       long AccountId)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","deleteInterest");

    return _fileUtilityDelete(_fp3, ev, AccountId);
}

/*
 * somInit is overriden to open the needed file given the
 * specialisation of the server
 * It will raise an exception if the file cannot be accessed
 */

SOM_Scope void  SOMLINK somInit(BranchDB somSelf)
{
    char branch[13]="";
    Environment *ev;
    char name1[13];
    char name2[13];
    char name3[13];

    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","somInit");

    ev = somGetGlobalEnvironment();

    BranchDB_parent_SOMObject_somInit(somSelf);
    /*
     * initialise the TypeCode for the static variables that will be
     * passed to the fileUtility methods.
     */
    _accountAny._type   = TC_BranchDB_AccountRecord;
    _feeAny._type       = TC_BranchDB_FeeRecord;
    _interestAny._type  = TC_BranchDB_InterestRecord;

    /*
     * prepare the file name by using the implementation alias to pick
     * different files for each different server
     */

    strncat(branch , __get_impl_alias(SOMD_ImplDefObject, ev),8);
    strcpy(name1, branch);
    strcpy(name2, branch);
    strcpy(name3, branch);

    somPrintf("Initializing Server :  %s\n", __get_impl_alias(SOMD_ImplDefObject,ev));

    _fp1 = FileUtilityNew();
    if (_fp1!=NULL) {
        _fileUtilityOpen( _fp1, ev, strcat(name1,".ACC"), TC_BranchDB_AccountRecord);
        if (check(*ev)) return;
    }
    _fp2 = FileUtilityNew();
    if (_fp2!=NULL) {
        _fileUtilityOpen( _fp2, ev, strcat(name2,".FEE"), TC_BranchDB_FeeRecord);
        if (check(*ev)) return;
    }
    _fp3 = FileUtilityNew();
    if (_fp3!=NULL) {
        _fileUtilityOpen( _fp3, ev, strcat(name3,".INT"), TC_BranchDB_InterestRecord);
        if (check(*ev)) return;
    }
return;
}

/*
 * somUninit closes the files
 */

SOM_Scope void  SOMLINK somUninit(BranchDB somSelf)
{
    BranchDBData *somThis = BranchDBGetData(somSelf);
    BranchDBMethodDebug("BranchDB","somUninit");

    if (_fp1) {
        _somFree(_fp1);
        _fp1=NULL;
    }
    if (_fp2) {
        _somFree(_fp2);
        _fp2=NULL;
    }
    if (_fp3) {
        _somFree(_fp3);
        _fp3=NULL;
    }

    BranchDB_parent_SOMObject_somUninit(somSelf);
}
