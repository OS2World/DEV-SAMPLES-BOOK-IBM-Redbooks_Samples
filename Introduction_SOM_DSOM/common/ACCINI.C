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
 * File : accini.c              Module : account.dll                    *
 *                                                                      *
 * Initialization for account.dll                                       *
 *                                                                      *
 * ******************************************************************** *
 */

#include <accmgr.h>
#include <checking.h>
#include <fee.h>
#include <savings.h>

#ifdef __IBMC__
#pragma linkage(SOMInitModule, system)
#endif

void SOMLINK SOMInitModule(integer4 majorVersion, integer4 minorVersion)
{
   AccountsManagerNewClass(0,0);
   CheckingAccountNewClass(0,0);
   FeeNewClass(0,0);
   SavingsAccountNewClass(0,0);
}

#ifdef _WIN16_
#include <windows.h>
int CALLBACK LibMain (HINSTANCE inst, WORD ds, WORD heapSize, LPSTR cmdLine)
{
   SOM_IgnoreWarning (inst);
   SOM_IgnoreWarning (ds);
   SOM_IgnoreWarning (heapSize);
   SOM_IgnoreWarning (cmdLine);

   SOM_ClassLibrary ("account.dll");
   return 1;
}
#endif /*_WIN16_*/
