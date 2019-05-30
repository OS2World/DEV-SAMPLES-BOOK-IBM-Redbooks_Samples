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
 * File : check.c                                                       *
 *                                                                      *
 * Generic environment (error) testing function                         *
 *                                                                      *
 * ******************************************************************** *
 *
 */
#include <somd.h>

#include <sberr.h>

long check(Environment e)
{

   StExcep *aStdException ;
   BankErr_EXCP *aSBException;
   switch (e._major) {
       case SYSTEM_EXCEPTION:
            somPrintf("SYSTEM_EXCEPTION\n");
            somPrintf("somExceptionId  %s\n",somExceptionId(&e));
            aStdException = (StExcep *)somExceptionValue(&e);
            if (aStdException!=NULL) {
                somPrintf("somExceptionValue(&e).minor : %d\n",aStdException->minor);
                switch (aStdException->completed) {
                    case YES:
                         somPrintf("Completed \n");
                         break;
                    case NO:
                         somPrintf("Not Completed\n");
                         break;
                    case MAYBE:
                         somPrintf("May be completed\n");
                         break;
                    default:
                         somPrintf("completed is not set properly");
                         break;
                } /* endswitch */
            } else {
                somPrintf("somExceptionValue is NULL");
            }
            return((long)(e._major));
            break;
       case USER_EXCEPTION:
            somPrintf("USER_EXCEPTION\n");
            somPrintf("somExceptionId  %s \n",somExceptionId(&e));
            aSBException = (BankErr_EXCP *)somExceptionValue(&e);
            if (aSBException!=NULL) {
                somPrintf("Error Code : %d \n",aSBException->errCode);
                somPrintf("Reason : %s \n",aSBException->reason);
            }
            return((long) (e._major));
            break;
       case NO_EXCEPTION:
            return((long) (e._major));
            break;
   } /* endswitch */
}
