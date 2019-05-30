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
 * File : custmgr.c        Module : customer.dll                        * 
 *                                                                      * 
 * Class : CustomersManager                                             * 
 *                                                                      * 
 *    Method(s):                 Public   Private                       * 
 *                                                                      * 
 *        verifyCustomer           X                                    * 
 *        deleteCustomer           X                                    * 
 *                                                                      * 
 * -------------------------------------------------------------------- *
 * Description :                                                        * 
 *                                                                      * 
 * CustomersManager provides a method to verify if a                    * 
 * customer exists in the Central Database and a                        * 
 * method to delete a customer from the database.                       * 
 *                                                                      * 
 ********************************************************************** *
 */
#define CustomersManager_Class_Source
#define CentralDBMetaClass_Class_Source
#include <somd.h>
#include <custmgr.ih>
#include <banksvr.h>
#include <bankdef.h>
/**********************************************************************
/*                                                                    *
/*  method : verifyCustomer                                           *
/*                                                                    *
/*  purpose: it retrieves a customer on the central files             *
/*                                                                    *
/*  input  : in long id - customer identifier                         *
/*                                                                    *
/**********************************************************************/

SOM_Scope long  SOMLINK verifyCustomer(CustomersManager somSelf,
                                       Environment *ev,
                                       long identifier)
{
    CentralDB theCentralDB;
    CentralDB_CustomerRecord aCustomerRecord;
    unsigned long rc;

    /* CustomersManagerData *somThis = CustomersManagerGetData(somSelf); */
    CustomersManagerMethodDebug("CustomersManager","verifyCustomer");

    theCentralDB = __get_theDB(SOMD_ServerObject,ev);
    rc = _retrieveCustomer(theCentralDB,ev,identifier, &aCustomerRecord);
    check(*ev);
    ORBfree(&aCustomerRecord);
    return(rc);
}


SOM_Scope long  SOMLINK deleteCustomer(CustomersManager somSelf,
                                        Environment *ev, long identifier)
{
    CentralDB theCentralDB;
    unsigned long rc;

    CustomersManagerMethodDebug("CustomersManager","deleteCustomer");

    theCentralDB = __get_theDB(SOMD_ServerObject,ev);
    rc = CentralDB_deleteCustomer(theCentralDB,ev,identifier);
    check(*ev);
    return(rc);
}

