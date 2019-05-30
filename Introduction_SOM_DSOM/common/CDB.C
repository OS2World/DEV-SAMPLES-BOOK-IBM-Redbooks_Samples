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
 * File : cdb.c            Module : cdb.dll                             *
 *                                                                      *
 * Class : CentralDB                                                    *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        insertCustomer           X                                    *
 *        retrieveCustomer         X                                    *
 *        updateCustomer           X                                    *
 *        deleteCustomer           X                                    *
 *        getAvailableID           X                                    *
 *        somInit                  X                                    *
 *        somUninit                x                                    *
 *                                                                      *
 * ******************************************************************** *
 */
#define CentralDB_Class_Source
#define SOMMSingleInstance_Class_Source
#include <cdb.ih>

#include <somd.h>
#include "bankdef.h"


/*
 * method to record a customer in the file.
 *
 * the function must insert records ordered by customerId, but the clients
 * can ask for an available ID and insert in the file in a non-sequential
 * order. So a write operation may cause a reordering of the last inserted
 * records
 */
SOM_Scope long  SOMLINK insertCustomer(CentralDB   somSelf,
                                       Environment *ev,
                                       CentralDB_CustomerRecord *aCustomer)
{
    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","insertCustomer");

   _customerAny._value = (void *)aCustomer;
   return _fileUtilityInsert( _fp1, ev, &_customerAny);

}

/*
 * method to retrieve a customer information given its Id.
 */

SOM_Scope long  SOMLINK retrieveCustomer(CentralDB   somSelf,
                                         Environment *ev,
                                         long        customerId,
                                          CentralDB_CustomerRecord* aRecord)
{
    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","retrieveCustomer");

    _customerAny._value = (void *)aRecord;
    return _fileUtilityRetrieve( _fp1, ev, customerId, &_customerAny);
}

/*
 * method to update a record.
 */

SOM_Scope long  SOMLINK updateCustomer(CentralDB   somSelf,
                                       Environment *ev,
                                       CentralDB_CustomerRecord* aCustomer)
{
    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","updateCustomer");

    _customerAny._value = (void *)aCustomer;
    return _fileUtilityUpdate( _fp1, ev, &_customerAny);

}

/*
 * method to delete a record.
 */
SOM_Scope long  SOMLINK deleteCustomer(CentralDB somSelf,  Environment *ev,
                                       long customerId)
{
    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","deleteCustomer");

    return _fileUtilityDelete(_fp1, ev, customerId);
}

/*
 * returns to the caller an available Id for a new customer
 */

SOM_Scope long  SOMLINK getAvailableId(CentralDB   somSelf,
                                       Environment *ev)
{

    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","getAvailableId");


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
 * somInit is overriden to open the needed file given the
 * specialization of the server
 * It will raise an exception if the file cannot be accessed
 */

SOM_Scope void  SOMLINK somInit(CentralDB somSelf)
{
    /*
     * define the Central id used in file names. will be updated
     */
    char branch[13]="";
    Environment *ev;

    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","somInit");
    ev = somGetGlobalEnvironment();

    CentralDB_parent_SOMObject_somInit(somSelf);

    /*
     * initialize the TypeCode for the static variables that will be
     * passed to the fileUtility methods.
     */
    _customerAny._type = TC_CentralDB_CustomerRecord;

    /*
     * prepare the file name by using the implementation alias to pick
     * the central id. The file is linited to 8 + 3 to support FAT files
     */
    strncat(branch, __get_impl_alias( SOMD_ImplDefObject, ev ), 8 );
    strcat(branch, ".BIN");

    somPrintf("Initializing Server :  CentralDBServer\n");

    _fp1 = FileUtilityNew();
    if (_fp1!=NULL) {
        _fileUtilityOpen( _fp1, ev, branch, TC_CentralDB_CustomerRecord);
        if (check(*ev)) return;
    }
    return;
}

/*
 * somUninit closes the files
 */

SOM_Scope void  SOMLINK somUninit(CentralDB somSelf)
{
    CentralDBData *somThis = CentralDBGetData(somSelf);
    CentralDBMethodDebug("CentralDB","somUninit");

    if (_fp1) {
        _somFree(_fp1);
        _fp1=NULL;
    }
    CentralDB_parent_SOMObject_somUninit(somSelf);

}
