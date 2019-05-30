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
 * File : banksvr.c        Module : banksvr.dll                         *
 *                                                                      *
 * Class : BankSOMDServer                                               *
 *                                                                      *
 * ---------------------------------------------------                  *
 * Description :                                                        *
 *                                                                      *
 * Subclasses SOMDServer to add an attrivute which stores a             *
 * reference to the appropriate Database Server.                        *
 * ******************************************************************** *
 */

#ifndef SOM_Module_banksvr_Source
#define SOM_Module_banksvr_Source
#endif
#define BankSOMDServer_Class_Source

#include <somd.h>
#include "banksvr.ih"
#include "bankdef.h"

SOM_Scope void  SOMLINK somInit(BankSOMDServer somSelf)
{
    Environment ev;
    SOMDServer server;

    char *alias;
    char temp[MAX_ALIAS_SZ];

    BankSOMDServerData *somThis = BankSOMDServerGetData(somSelf);
    BankSOMDServerMethodDebug("BankSOMDServer","somInit");

    BankSOMDServer_parent_SOMDServer_somInit(somSelf);

    SOM_InitEnvironment(&ev);

    /*
     * get alias from the SOMD_ImplDefObject, and build the alias for
     * the access to the appropriate DBServer
     */

    alias = __get_impl_alias(SOMD_ImplDefObject, &ev);
    somPrintf("Initializing Server :  %s\n",alias);

    /*
     * retrieve the branch name of the string. it is before the hyphen
     */

    strcpy( temp, strtok(alias, "-"));

    /*
     * add DBServer to the name of the branch to get the name of branch
     * alias for the DBServer.
     */

    strcat( temp , "DBServer");

    server = _somdFindServerByName(SOMD_ObjectMgr, &ev, temp);
    if (check(ev)) return;   /* Failure */

    if (temp[0]=='B') {
         BranchDBNewClass(0,0);
        _theDB = _somdCreateObj(server, &ev, "BranchDB", "" );
        if (check(ev)) return;  /* Failure */
    } else {
         CentralDBNewClass(0,0);
        _theDB = _somdCreateObj(server, &ev, "CentralDB", "" );
        if (check(ev)) return; /* Failure */
    }

    /*
     * SOMFree to release the memory allocated by _get_impl_alias
     */

    SOMFree(alias);

    /*
     * Free the proxy to the server, but do not free the server !
     */
    if (server)
       _somdProxyFree(server,&ev);
    return;
}

SOM_Scope void  SOMLINK somUninit(BankSOMDServer somSelf)
{
    BankSOMDServerData *somThis = BankSOMDServerGetData(somSelf);
    BankSOMDServerMethodDebug("BankSOMDServer","somUninit");

    if (_theDB) {
       _somdProxyFree(_theDB,somGetGlobalEnvironment());
       _theDB=NULL;
     }
    BankSOMDServer_parent_SOMDServer_somUninit(somSelf);

}

