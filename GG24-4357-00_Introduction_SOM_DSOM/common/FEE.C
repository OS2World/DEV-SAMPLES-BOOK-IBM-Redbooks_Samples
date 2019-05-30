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
 * File : fee.c            Module : account.dll                         *
 *                                                                      *
 * Class : Fee                                                          *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        setFee                   X                                    *
 *        computeFee               X                                    *
 *        displaySelf              X                                    *
 *        somInit                  x                                    *
 *                                                                      *
 * ******************************************************************** *
 */
#define Fee_Class_Source
#include <fee.ih>

/*
 ****************************************************************************
 * Method to install a (fee,limit) pair. This implementation uses just a    *
 * single such pair, in a more general case one would have several levels   *
 * (with perhaps increasing fee with decreasing balance). The instance      *
 * variable 'count' denotes the number of installed pairs.                  *
 ****************************************************************************
 */
SOM_Scope long  SOMLINK setFee(Fee         somSelf,
                               Environment *ev,
                               double      fee,
                               double      limit)
{
    FeeData *somThis = FeeGetData(somSelf);
    FeeMethodDebug("Fee","addFee");

    _sum = fee; _limit = limit; _count = 1;

    return (_count);
}
/*
 ****************************************************************************
 * If this fee object is active (count>1), this method computes the amount  *
 * of fee due for the specified balance. Obviously, a more complex,         *
 * multiple level fee hierarchy is easily implemented.                      *
 ****************************************************************************
 */

SOM_Scope double  SOMLINK computeFee(Fee         somSelf,
                                     Environment *ev,
                                     double      balance)
{
    FeeData *somThis = FeeGetData(somSelf);
    FeeMethodDebug("Fee","computeFee");

    if ( _count < 1 ) return (0.0);
    else if ( balance < _limit ) return (_sum);
    else return (0.0);
}

/*
 ****************************************************************************
 * We override somInit() to provide basic default initialization for every  *
 * fee object created. We impose $0.20 fee on every transaction, if the     *
 * balance drops below $400.00 after the transaction is completed.          *
 ****************************************************************************
 */
SOM_Scope void  SOMLINK somInit(Fee somSelf)
{
    FeeData *somThis = FeeGetData(somSelf);
    FeeMethodDebug("Fee","somInit");

    Fee_parent_SOMObject_somInit(somSelf);

    _count = 1; _sum = 0.20; _limit = 400.0;
}

