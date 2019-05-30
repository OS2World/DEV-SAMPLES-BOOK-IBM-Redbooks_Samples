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
 * File : accmgr.c         Module : account.dll                         * 
 *                                                                      * 
 * Class : AccountsManager                                              * 
 *                                                                      * 
 *    Method(s):                 Public   Private                       * 
 *                                                                      * 
 *        findAccountsByOwner      X                                    * 
 *        deleteAccount            X                                    * 
 *                                                                      * 
 * -------------------------------------------------------------------- *
 * Description :                                                        * 
 *                                                                      * 
 * This module implements the AccountsManager class                     * 
 * which includes the findAccountsByOwner method.                       * 
 * By passing this method a customer ID, the accounts                   * 
 * database is searched and two sequences are                           * 
 * returned:                                                            * 
 *       1) A list of account ID's belonging to the                     * 
 *          customer                                                    * 
 *                                                                      * 
 *       2) A list of account types indicating                          * 
 *          the account types for each account                          * 
 *          in the list above.                                          * 
 *                                                                      * 
 * ******************************************************************** *
 */
#define AccountsManager_Class_Source
#define SOMClass_Class_Source
#include <string.h>
#include <somd.h>
#include "accmgr.ih"
#include "banksvr.h"
#include "bankdef.h"

/****************************************************************************/
/* Locates all account ids that belong to the given customer id.  It        */
/* queries the persistent service to do that.                               */
/****************************************************************************/

SOM_Scope long  SOMLINK findAccountsByOwner(AccountsManager somSelf,
                                            Environment *ev,
                                            long owner,
                                            _IDL_SEQUENCE_long* ids,
                                            _IDL_SEQUENCE_long* types)
{
    BankErr_EXCP           * Bank_Excp;
    long                   rc,  i, count;
    long                   idsFound[MAXIMUM_COUNT], typesFound[MAXIMUM_COUNT];
    BranchDB               theAccountDB;
    BranchDB_AccountRecord anAccountRecord;

    /* AccountsManagerData *somThis = AccountsManagerGetData(somSelf); */
    AccountsManagerMethodDebug("AccountsManager","findAccountsByOwner");

    theAccountDB = __get_theDB(SOMD_ServerObject, ev);
    if (check(*ev)) {
       somExceptionFree(ev);
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = AM_BDB_ACC_ERR;
        strcpy(Bank_Excp->reason,AM_BDB_ACC_ERR_TXT);
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return (SOM_ENVIRONMENT);
    }
    rc = _selectAccountsByCustomerId(theAccountDB, ev, owner, &count, idsFound);
    if (check(*ev)) {
        somExceptionFree(ev);
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = AM_BDB_SAF_ERR;
        strcpy(Bank_Excp->reason,AM_BDB_SAF_ERR_TXT);
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return (rc);
    }
    if ( count > MAXIMUM_COUNT ) count = MAXIMUM_COUNT;

    for ( i = 0; i < count; i++ ) {
        rc = _retrieveAccount(theAccountDB, ev, idsFound[i], &anAccountRecord);
        if (check(*ev)){
            somExceptionFree(ev);
        }
        typesFound[i] = anAccountRecord.type;
    }

    ids->_maximum = ids->_length  = count;
    types->_maximum = types->_length  = count;

    if ( count > 0 ) {
                            /* ********************* */
                            /* NOTE: Client must     */
                            /* SOMFree ids->_buffer  */
                            /* SOMFree types->_buffer  */
                            /* ********************* */

       ids->_buffer  = (long *) SOMMalloc((size_t)count*sizeof(long));
       if (!ids->_buffer) {
           return (SOM_ENVIRONMENT);
       } else {
           memcpy(ids->_buffer,idsFound,(size_t)count*sizeof(long));
       } /*end if*/

       types->_buffer  = (long *) SOMMalloc((size_t)count*sizeof(long));
       if (!types->_buffer) {
           return (SOM_ENVIRONMENT);
       } else {
           memcpy(types->_buffer,typesFound,(size_t)count*sizeof(long));
       } /*end if*/
    } /*end if*/
    ORBfree(&idsFound);
    ORBfree(&anAccountRecord);
    return (OK);
}

SOM_Scope long  SOMLINK deleteAccount(AccountsManager somSelf,
                                       Environment *ev, long accountID)
{
    BankErr_EXCP * Bank_Excp;
    long      rc;
    BranchDB  theBranchDB;

    AccountsManagerMethodDebug("AccountsManager","deleteAccount");
    theBranchDB=__get_theDB(SOMD_ServerObject,ev);

    rc = BranchDB_deleteFee(theBranchDB, ev, accountID);
    if (check(*ev)) {
        somExceptionFree(ev);
    }
    rc = BranchDB_deleteInterest(theBranchDB, ev, accountID);
    if (check(*ev)) {
        somExceptionFree(ev);
    }
    rc=(BranchDB_deleteAccount(theBranchDB, ev, accountID));
    if (check(*ev)) {
        somExceptionFree(ev);
    }
    return(rc);
}

