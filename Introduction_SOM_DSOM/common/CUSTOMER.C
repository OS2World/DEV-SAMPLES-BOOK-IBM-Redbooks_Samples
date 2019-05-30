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
 * File : customer.c       Module : customer.dll                        *
 *                                                                      *
 * Class : Customer                                                     *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        isValidPin               X                                    *
 *        save                     X                                    *
 *        delete                   X                                    *
 *        somUninit                X                                    *
 *                                                                      *
 * Class : CustomerMetaClass                                            *
 *                                                                      *
 *    Method(s):                                                        *
 *                                                                      *
 *        create                   X                                    *
 *        restore                  X                                    *
 *                                                                      *
 * -------------------------------------------------------------------- *
 * Description :                                                        *
 *                                                                      *
 * CustomerMetaClass provides constructors to create                    *
 * instances of the class Customer with specific                        *
 * variable values.                                                     *
 * The constructors access the customer DBObject                        *
 * to get all the needed values.                                        *
 *                                                                      *
 * Customer class  provides the methods of the customer                 *
 * objects created by the CustomerMetaClass                             *
 * constructor.                                                         *
 *                                                                      *
 ********************************************************************** *
 */
#define Customer_Class_Source
#define CustomerMetaClass_Class_Source

#include <stdlib.h>
#include <stdio.h>
#include <somd.h>
#include <cdb.h>
#include <customer.ih>
#include <banksvr.h>
#include <bankdef.h>
/*
 **********************************************************************
 *                                                                    *
 *  method : isValidPin                                               *
 *                                                                    *
 *  purpose: it validates the PIN number of the customer              *
 *                                                                    *
 *  input  : in long pin - PIN identifier                             *
 *                                                                    *
 *  returns: TRUE  - PIN is valid                                     *
 *           FALSE - PIN is not valid                                 *
 *                                                                    *
 **********************************************************************
 */
SOM_Scope boolean  SOMLINK isValidPin(Customer    somSelf,
                                      Environment *ev,
                                      long        pin)
{
    CustomerData *somThis = CustomerGetData(somSelf);
    CustomerMethodDebug("Customer","isValidPin");

   if (_pin == pin)
        return(TRUE);
    else
        return(FALSE);

}
/*
 **********************************************************************
 *                                                                    *
 *  method : save                                                     *
 *                                                                    *
 *  purpose: it saves customer data on the central file               *
 *                                                                    *
 **********************************************************************
 */

SOM_Scope long  SOMLINK save(Customer    somSelf,
                             Environment *ev)
{

    CustomerRecord     CentralDB_aCustomerRecord;
    CustomerMetaClass  CustomerClass;
    CustomerData       *somThis = CustomerGetData(somSelf);
    CentralDB          theCentralDB;
    unsigned long      retCode;

    CustomerMethodDebug("Customer","save");
                                     /*
                                      *********************************
                                      * assign customer data central  *
                                      * file                          *
                                      *********************************
                                      */
    CentralDB_aCustomerRecord.id  = _identifier;
    CentralDB_aCustomerRecord.pin = _pin;
    memcpy(CentralDB_aCustomerRecord.name,_name,strlen(_name)+1);
    memcpy(CentralDB_aCustomerRecord.branch,_branch,strlen(_branch)+1);
                                     /*
                                      *********************************
                                      * get DBObject                  *
                                      *********************************
                                      */

    CustomerClass = _somGetClass(somSelf);
    theCentralDB = __get_theDB(SOMD_ServerObject,ev);
    if (check(*ev)) {
       somExceptionFree(ev);
       return (SOM_ENVIRONMENT);
    }
                                     /*
                                      *********************************
                                      * insert customer data on DB    *
                                      * Server                        *
                                      *********************************
                                      */
    retCode = _insertCustomer(theCentralDB, ev, &CentralDB_aCustomerRecord);
    check(*ev);
    return (retCode);
}

SOM_Scope long  SOMLINK delete(Customer somSelf,  Environment *ev)
{
    CustomerData *somThis = CustomerGetData(somSelf);
    CustomerMethodDebug("Customer","delete");

    return _deleteCustomer(__get_theDB(SOMD_ServerObject,ev), ev, _identifier);
}

/*
 *Method from the IDL attribute statement:
 *"attribute string name"
 */

SOM_Scope void  SOMLINK _set_name(Customer somSelf,  Environment *ev,
                                  string name)
{
    CustomerData *somThis = CustomerGetData(somSelf);
    CustomerMethodDebug("Customer","_set_name");

    if (_name) {
        SOMFree(_name);
    }
    _name = SOMMalloc(strlen(name)+1);
    strcpy(_name,name);
    return;
}


SOM_Scope void  SOMLINK _set_branch(Customer somSelf,  Environment *ev,
                                    string branch)
{
    CustomerData *somThis = CustomerGetData(somSelf);
    CustomerMethodDebug("Customer","_set_branch");

    if (_branch) {
        SOMFree(_branch);
    }
    _branch = SOMMalloc(strlen(branch)+1);
    strcpy(_branch,branch);
    return;
}
/*
 **********************************************************************
 *                                                                    *
 *  method : somUninit                                                *
 *                                                                    *
 *  purpose: it frees the allocated customer string data              *
 *                                                                    *
 *  notes  : override                                                 *
 *                                                                    *
 **********************************************************************
 */
SOM_Scope void  SOMLINK somUninit(Customer somSelf)
{
    CustomerData *somThis = CustomerGetData(somSelf);
    CustomerMethodDebug("Customer","somUninit");

    _identifier=0;
    _pin=0;

    if (_name) {
        SOMFree(_name);
        _name=NULL;
    }
    if (_branch) {
        SOMFree(_branch);
        _branch=NULL;
    }
    Customer_parent_SOMObject_somUninit(somSelf);
}


SOM_Scope Customer  SOMLINK create(CustomerMetaClass somSelf,
                                    Environment *ev, string name,
                                   string branch)
{
    CustomerRecord CentralDB_aCustomerRecord;
    Customer       theNewCustomer;
    CentralDB      theCentralDB;
    long           identifier;
    long           pin;

    /* CustomerMetaClassData *somThis = CustomerMetaClassGetData(somSelf); */
    CustomerMetaClassMethodDebug("CustomerMetaClass","create");

    theNewCustomer = _somNew(somSelf);
    theCentralDB = __get_theDB(SOMD_ServerObject,ev);
    if (check(*ev)) {
       return((Customer)NULL);
    }
                                     /*
                                      *********************************
                                      * get a new Id                  *
                                      *********************************
                                      */
    identifier =CentralDB_getAvailableId(theCentralDB,ev);
    if (identifier == 0L) {
        return((Customer)NULL);
    }
                                     /*
                                      *********************************
                                      * assign data to customer       *
                                      *********************************
                                      */
    pin =  identifier;

    if (strlen(name) > sizeof(CentralDB_aCustomerRecord.name) -1) {
        name[sizeof(CentralDB_aCustomerRecord.name) - 1] = '\0';
    }
    if (strlen(branch) > sizeof(CentralDB_aCustomerRecord.branch) -1) {
        branch[sizeof(CentralDB_aCustomerRecord.branch) - 1] = '\0';
    }

    __set_identifier(theNewCustomer,ev,identifier);
    __set_pin(theNewCustomer,ev,pin);
    __set_name(theNewCustomer,ev,name);
    __set_branch(theNewCustomer,ev,branch);
    return(theNewCustomer);
}
/*
 **********************************************************************
 *                                                                    *
 *  method : restore   (constructor)                                  *
 *                                                                    *
 *  purpose: it restores a customer object from central files         *
 *                                                                    *
 *  input  : in long identifier - customer identifier                 *
 *                                                                    *
 *  returns: a customer object                                        *
 *                                                                    *
 **********************************************************************
 */
SOM_Scope Customer  SOMLINK restore(CustomerMetaClass somSelf,
                                    Environment       *ev,
                                    long              id)
{

    CentralDB      theCentralDB;
    CustomerRecord aCustomerRecord;
    Customer       theCustomer ;
    unsigned long  rc;

    CustomerMetaClassMethodDebug("CustomerMetaClass","restore");
                                     /*
                                      *********************************
                                      * assign data to customer       *
                                      *********************************
                                      */
    theCustomer = _somNew(somSelf);
    theCentralDB = __get_theDB(SOMD_ServerObject,ev);
    if (check(*ev)) {
       return((Customer)NULL);
    }
                                     /*
                                      *********************************
                                      * retrieve customer data ...    *
                                      *********************************
                                      */
    rc =  _retrieveCustomer(theCentralDB,ev,id, &aCustomerRecord);
    if (check(*ev)) {
       return((Customer)NULL);
    }
    if (rc == OK) {
        __set_identifier(theCustomer,ev,aCustomerRecord.id);
        __set_pin(theCustomer,ev,aCustomerRecord.pin);
        __set_name(theCustomer,ev,aCustomerRecord.name);
        __set_branch(theCustomer,ev,aCustomerRecord.branch);
    } else {
        __set_identifier(theCustomer,ev,0);
        __set_pin(theCustomer,ev,0);
        __set_name(theCustomer,ev,"");
        __set_branch(theCustomer,ev,"");
    }
    ORBfree(&aCustomerRecord);
    return (theCustomer);
}
