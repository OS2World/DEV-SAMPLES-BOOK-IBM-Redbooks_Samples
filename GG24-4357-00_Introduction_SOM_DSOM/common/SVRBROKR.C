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
 * File : svrbrokr.c       Module : svrbrokr.dll                        *
 *                                                                      *
 * Class : ServerBroker                                                 *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        nameOfServer             X                                    *
 *        releaseServer            X                                    *
 *        save                     X                                    *
 *        somInit                  X                                    *
 *                                                                      *
 * ---------------------------------------------------------------------*
 * Description :                                                        *
 *                                                                      *
 * This module is the "Name Broker" function for the                    *
 * SOMBANK sample application, but is in fact a model                   *
 * for a generic Name Broker.                                           *
 *                                                                      *
 * This function provides services such that each                       *
 * client can have access to a unqiue server process.                   *
 *                                                                      *
 * The Name Broker itself is a single instance class                    *
 * and will queue all requests to it in a FIFO queue.                   *
 * The Name Broker transactions should be relatively                    *
 * short in duration and therefore we do not expect                     *
 * much impact on overall application performance by                    *
 * having this single server process.                                   *
 *                                                                      *
 ********************************************************************** *
 */
#ifndef SOM_Module_svrbrokr_Source
#define SOM_Module_svrbrokr_Source
#endif
#define ServerBroker_Class_Source
#define SOMMSingleInstance_Class_Source

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <somd.h>
#include "svrbrokr.ih"
#include "bankdef.h"
#include "sberr.h"

/*******************************************************************/
/*    METHOD NAME: nameOfServer                                    */
/*                                                                 */
/*      This method is called by a client thats wants a template   */
/*      to be brokered for it. This method maintains a pool of     */
/*      id's that are used to form new alias names that are unique */
/*      in the implementation repository. The technique used here  */
/*      is rather simplistic and may need to be adapted for a      */
/*      production level application.                              */
/*                                                                 */
/*                                                                 */
/*      INPUT                                                      */
/*                                                                 */
/*        (in string templateAlias)                                */
/*                                                                 */
/*          The template alias to be brokered.                     */
/*                                                                 */
/*      OUTPUT                                                     */
/*                                                                 */
/*        (out string newAlias)                                    */
/*                                                                 */
/*          The new alias name to use.                             */
/*                                                                 */
/*      RETURN CODES                                               */
/*                                                                 */
/*          This method returns whatever SOM exceptions it         */
/*          encounters in the client's Environment structure.      */
/*                                                                 */
/*******************************************************************/
SOM_Scope void  SOMLINK nameOfServer(ServerBroker somSelf,  Environment *ev,
                                     string template, string* newAlias)
{
    BankErr_EXCP *Bank_Excp;
    StExcep *aStdException;
    unsigned long i,j;
    ServerBrokerData *somThis = ServerBrokerGetData(somSelf);
    ServerBrokerMethodDebug("ServerBroker","nameOfServer");

    i=j=0L;
    _mustAddImplDef=FALSE;
/*_________________________________________________________________*/
/*    Reset the newAlias method parameter in case this method      */
/*    returns in error.                                            */
/*_________________________________________________________________*/

    *newAlias=NULL;

/*_________________________________________________________________*/
/*  |  Search the pool of Id's (_idPool) for a free Id.            */
/*_________________________________________________________________*/
    while (
            (_serverStateArray[i].alias[0] !=(char) NULL) &&
            ((strncmp(_serverStateArray[i].alias,template,strlen(template))) ||
            _serverStateArray[i].inUse ) &&
            i<MAXIMUM_IDS)
    {
       i++;
    } /* endwhile */


    if (i>=MAXIMUM_IDS) {
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = 99;
        strcpy(Bank_Excp->reason,"ServerBroker: Exceeded max aliases.");
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return;
    } else {
       if (_outNewAlias) {
          SOMFree(_outNewAlias);
          _outNewAlias=NULL;
       } /* endif */

       _outNewAlias=(string)SOMMalloc(strlen(template) +4+2);
       /* The 4 is a hardcoded maximum ascii size of id  */
       /* for the sake of simplicity. 2 provides for the */
       /* hyphen and x'00' end of string indicator.      */

       if (_serverStateArray[i].alias[0]== (char)NULL) {

          sprintf(_outNewAlias, "%s-%04ld", template, i);
          _mustAddImplDef=TRUE;

       } else {

          if (_serverStateArray[i].inUse==FALSE) {
             strcpy(_outNewAlias,_serverStateArray[i].alias);

          } /* endif */
       } /* endif */
    } /* endif */


if (_mustAddImplDef) {

   /*_________________________________________________________________*/
   /*  Search the implementation repository for the requested         */
   /*    template name and return an implementation definition object */
   /*    for that entry in the repository.                            */
   /*  if an exception was raised then                                */
   /*     return to the caller;                                       */
   /*  endif                                                          */
   /*_________________________________________________________________*/
   /*  NOTE 1: ImplRepository is a class that this object inherits    */
   /*  from (as well as class SOMObject). The _find_impldef_by_alias  */
   /*  method returns an object. The data that comprises              */
   /*  the implementation definition are attributes of that object    */
   /*  and can be read/changed by the attribute-generated __get_xxx   */
   /*  and __set_xxx methods.                                         */
   /*_________________________________________________________________*/
       _foundImplDef = _find_impldef_by_alias(SOMD_ImplRepObject,
                                              ev, template);
       if (check(*ev)) {
          return;
       } /* endif */

   /*_________________________________________________________________*/
   /*  List all the classes that have been associated with this       */
   /*    template in order to associate them with the new alias       */
   /*    that will be created.                                        */
   /*  If an exception was raised then                                */
   /*    free the storage for the found implementation definition     */
   /*      (_foundImplDef instance variable)                          */
   /*    set the _foundImplDef instance variable to NULL (to enable   */
   /*      testing of its allocation status)                          */
   /*    return to the caller                                         */
   /*  endif                                                          */
   /*_________________________________________________________________*/
       _listOfImplDefClasses = _find_classes_by_impldef(SOMD_ImplRepObject,
                                 ev, __get_impl_id(_foundImplDef, ev));

       if (check(*ev)) {
          _somFree(_foundImplDef);
          _foundImplDef=NULL;
          return;
       } /* endif */


   /*_________________________________________________________________*/
   /*  |  Set the new alias name in the found implementation          */
   /*  |    definition object                                         */
   /*  |  Add a new implementation definition to the implementation   */
   /*  |    repository using the altered found implementation         */
   /*  |    definition object                                         */
   /*  enddo - while the alias added is already in the the            */
   /*          implementation repository and there are id's left      */
   /*_________________________________________________________________*/
   /*  Note 1: a duplicate alias raises a som exception.              */
   /*_________________________________________________________________*/

          __set_impl_alias(_foundImplDef, ev, _outNewAlias);
          _add_impldef(SOMD_ImplRepObject, ev, _foundImplDef);

   /*_________________________________________________________________*/
   /*  If an exception was set then                                   */
   /*    free the storage for the found implementation definition     */
   /*      (_foundImplDef instance variable)                          */
   /*    set the _foundImplDef instance variable to NULL (to enable   */
   /*      testing of its allocation status)                          */
   /*    return to the caller                                         */
   /*  endif                                                          */
   /*_________________________________________________________________*/
       if (check(*ev)) {
          _somFree(_foundImplDef);
          _foundImplDef=NULL;

          aStdException = (StExcep *)somExceptionValue(ev);
          if ((aStdException==NULL) || (aStdException->minor != 30087)) {
             return;
          }
          somExceptionFree(ev);
       } else {

   /*_________________________________________________________________*/
   /*  Free the storage for the found implementation definition       */
   /*    object (_foundImplDef instance variable)                     */
   /*  Set the _foundImplDef instance variable to NULL (to enable     */
   /*    testing of its allocation status)                            */
   /*_________________________________________________________________*/
          _somFree(_foundImplDef);
          _foundImplDef=NULL;

   /*_________________________________________________________________*/
   /*  If there were classes associated with the found implementation */
   /*    definition (the template) then                               */
   /*    search the implementation repository for the new alias       */
   /*      and return an implementation definition object for that    */
   /*      entry in the repository                                    */
   /*    if an exception was raised                                   */
   /*       return to the caller                                      */
   /*    endif                                                        */
   /*    set the _foundImplid instance variable to be the             */
   /*      implementation id for the found implementation definition  */
   /*      (the new alias created by this method)                     */
   /*    for the number of classes associated with the template       */
   /*      add each class to the new alias in the implementation      */
   /*        repository                                               */
   /*      if an exception was set then                               */
   /*        return to the caller                                     */
   /*      endif                                                      */
   /*    endfor                                                       */
   /*    Free the storage for the found implementation definition     */
   /*      object (_foundImplDef instance variable)                   */
   /*    Set the _foundImplDef instance variable to NULL (to enable   */
   /*      testing of its allocation status)                          */
   /*  endif                                                          */
   /*_________________________________________________________________*/
          if (_listOfImplDefClasses._length>0) {
              _foundImplDef=_find_impldef_by_alias(SOMD_ImplRepObject,
              ev, _outNewAlias);

            if (check(*ev)) {
               return;
            } /* endif */

            _foundImplid=__get_impl_id(_foundImplDef, ev);

            for (j=0L; j<_listOfImplDefClasses._length; j++) {
               _add_class_to_impldef(SOMD_ImplRepObject, ev,
                    _foundImplid, _listOfImplDefClasses._buffer[j]);

              if (check(*ev)) {
                 return;
              } /* endif */

            } /* endfor */

            _somFree(_foundImplDef);
            _foundImplDef=NULL;

          } /* endif */

   /*_________________________________________________________________*/
   /*  for the number of class associations found                     */
   /*    free the storage for each class                              */
   /*    set its entry to NULL (to enable testing of its              */
   /*      allocation status)                                         */
   /*  endfor                                                         */
   /*  reset the number of class associations found and the           */
   /*    maximum fields                                               */
   /*_________________________________________________________________*/
   /*  NOTE 1: the _listOfImplDefClass is a sequence of strings. A    */
   /*  sequence is a structure that contains a _length field, a       */
   /*  _maximum field, and a _buffer.                                 */
   /*_________________________________________________________________*/

          for (j=0L; j<_listOfImplDefClasses._length; j++) {
             SOMFree(_listOfImplDefClasses._buffer[j]);
             _listOfImplDefClasses._buffer[j]=NULL;
          } /* endfor */
          _listOfImplDefClasses._length=_listOfImplDefClasses._maximum=0;
      } /* endif */
} /*endif */

/*_________________________________________________________________*/
/*  Set this pool id to be in use (TRUE)                           */
/*_________________________________________________________________*/

_serverStateArray[i].inUse=TRUE;
strcpy(_serverStateArray[i].alias,_outNewAlias);

/*_________________________________________________________________*/
/*  call the _save method for this object's persistence            */
/*_________________________________________________________________*/

_save(somSelf, ev);

/*_________________________________________________________________*/
/*  allocate storage for the newAlias method parameter (this is    */
/*    used to return the name of the new alias to the caller)      */
/*  copy the new alias name to the newAlias method parameter       */
/*_________________________________________________________________*/
/*  NOTE 1: the newAlias method parameter is an "out string". This */
/*  is a pointer to a pointer.                                     */
/*_________________________________________________________________*/
    *newAlias=(string)SOMMalloc(strlen(_outNewAlias)+1);
    strcpy(*newAlias, _outNewAlias);

/*_________________________________________________________________*/
/*  free the storage for the _outNewAlias instance variable        */
/*  and set it to NULL                                             */
/*_________________________________________________________________*/
     SOMFree(_outNewAlias);
     _outNewAlias=NULL;

/*_________________________________________________________________*/
/*  return to the caller                                           */
/*_________________________________________________________________*/
    return;
}

/*******************************************************************/
/*    METHOD NAME: releaseServer                                   */
/*                                                                 */
/*      This method is called by a client to release the alias     */
/*      that was brokered for it by the nameOfServer method.       */
/*      The inUse variable in the id pool is set to FALSE.         */
/*                                                                 */
/*      INPUT                                                      */
/*                                                                 */
/*       (in string releaseAlias)                                  */
/*                                                                 */
/*         The alias name that was brokered by the nameOfServer    */
/*         method. The exact name that was brokered MUST be passed */
/*         to this method and MUST be of the form template-id.     */
/*                                                                 */
/*      RETURN CODES                                               */
/*                                                                 */
/*          This method returns whatever SOM exceptions it         */
/*          encounters in the client's Environment structure.      */
/*                                                                 */
/*******************************************************************/
SOM_Scope void  SOMLINK releaseServer(ServerBroker somSelf,  Environment *ev,
                                      string releaseAlias)
{
    BankErr_EXCP *Bank_Excp;
    char *id;
    ServerBrokerData *somThis = ServerBrokerGetData(somSelf);
    ServerBrokerMethodDebug("ServerBroker","releaseServer");
/*_________________________________________________________________*/
/*  set the id pointer to NULL                                     */
/*  find the hyphen in the alias name to be released               */
/*  if the hyphen was not found (the id pointer is NULL) then      */
/*    ==> here set a user exception???                             */
/*    ==> this is not a valid name                                 */
/*    return to the caller                                         */
/*  endif                                                          */
/*  set the id pointer to the first digit of the id for alias      */
/*    to be released                                               */
/*_________________________________________________________________*/
/*  NOTE 1: the alias names built by nameOfServer are of the form  */
/*  template-id. For example template B005 becomes B005-23.        */
/*_________________________________________________________________*/
    id=NULL;
    id=strchr(releaseAlias, '-');
    if (id==NULL) {
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = 99;
        strcpy(Bank_Excp->reason,"ServerBroker: Invalid alias.");
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return;
    } /* endif */
    id++;

/*_________________________________________________________________*/
/*  Set the entry in the id pool to available (FALSE)              */
/*_________________________________________________________________*/
    _serverStateArray[atoi(id)].inUse=FALSE;

/*_________________________________________________________________*/
/*  call the _save method for this object's persistence            */
/*_________________________________________________________________*/
    _save(somSelf, ev);

/*_________________________________________________________________*/
/*  return to the caller                                           */
/*_________________________________________________________________*/
    return;
}

/*******************************************************************/
/*    METHOD NAME: save                                            */
/*                                                                 */
/*      This method is called internally by nameOfServer and       */
/*      releaseAlias for object persistence.                       */
/*                                                                 */
/*      The file system is used for persistence. The id pool       */
/*      (array) is written to a file using the C file i/o          */
/*      functions. Saving the id pool means that the NameBroker    */
/*      can fail (the system it is running on might become         */
/*      unavailable) and restart without forcing all clients       */
/*      stop/restart (to ensure that the id name space is correct) */
/*                                                                 */
/*******************************************************************/
SOM_Scope void  SOMLINK save(ServerBroker somSelf,  Environment *ev)
{
    BankErr_EXCP *Bank_Excp;
    ServerBrokerData *somThis = ServerBrokerGetData(somSelf);
    ServerBrokerMethodDebug("ServerBroker","save");

    if ((_datFp=fopen("svrbrokr.dat", "wb"))==NULL) {
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = 99;
        strcpy(Bank_Excp->reason,"ServerBroker: Can't open svrbrokr.dat.");
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return;
    } else {
       if (fwrite(&(_serverStateArray), sizeof(_serverStateArray), 1, _datFp)!=1) {
          Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
          Bank_Excp->errCode = 99;
          strcpy(Bank_Excp->reason,"ServerBroker: Error writing svrbrokr.dat.");
          somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       }
       fclose(_datFp);
       return;
    } /* endif */
}

/*******************************************************************/
/*    OVERRIDEN METHOD NAME: somInit                               */
/*                                                                 */
/*      This method is called the first time the ServerBroker is   */
/*      instantiated by a client. Basic initialisation of instance */
/*      varables is performed. The id pool (array _idPool) is      */
/*      restored from the file system to maintain this object's    */
/*      persistence.                                               */
/*                                                                 */
/*******************************************************************/
SOM_Scope void  SOMLINK somInit(ServerBroker somSelf)
{
    Environment *ev;
    boolean outOfSync = FALSE;
    long i=0L;

    ServerBrokerData *somThis = ServerBrokerGetData(somSelf);
    ServerBrokerMethodDebug("ServerBroker","somInit");

    ServerBroker_parent_SOMObject_somInit(somSelf);

    somPrintf("Initializing Server : ServerBroker...\n");
    somPrintf("   Maximum aliases: %d\n",MAXIMUM_IDS);
    ev=somGetGlobalEnvironment();
    _mustAddImplDef=FALSE;

    for (i=0L; i<MAXIMUM_IDS; i++) {
       strcpy(_serverStateArray[i].alias,"");
       _serverStateArray[i].inUse=FALSE;

    } /* endfor */

    _outNewAlias=NULL;
    _foundImplid=NULL;

    _listOfImplDefClasses._buffer=NULL;
    _listOfImplDefClasses._length=_listOfImplDefClasses._maximum=0;

    if ((_datFp=fopen("svrbrokr.dat", "rb"))==NULL) return;

    if (fread(&(_serverStateArray), sizeof(_serverStateArray), 1, _datFp)!=1) {
       somPrintf("Error reading svrbrokr.dat file\n");
       somPrintf("- persistence lost.\n");
       for (i=0L; i<MAXIMUM_IDS; i++) {
          strcpy(_serverStateArray[i].alias,"");
          _serverStateArray[i].inUse=FALSE;
       } /* endfor */
       fclose(_datFp);
       _save(somSelf, ev);
    } else {
       fclose(_datFp);
    }/*end if*/

    somPrintf("   Resynching ServerBroker persistant store with\n");
    somPrintf("     Implementation Repository...\n");
    somPrintf("   Aliases believed to be currently in-use:\n");
    for (i=0L; i<MAXIMUM_IDS; i++) {
       if (outOfSync) {
          strcpy(_serverStateArray[i].alias,"");
          _serverStateArray[i].inUse=FALSE;
       } else {
          _foundImplDef=_find_impldef_by_alias(SOMD_ImplRepObject,
                                               ev,
                                               _serverStateArray[i].alias);
          if (ev->_major!=NO_EXCEPTION) {
             outOfSync=TRUE;
             strcpy(_serverStateArray[i].alias,"");
             _serverStateArray[i].inUse=FALSE;
             somExceptionFree(ev);
          } else {
             if (_serverStateArray[i].inUse) {
                 somPrintf("\t%s\n",_serverStateArray[i].alias);
             }
             if (_foundImplDef) {
                 _somFree(_foundImplDef);
             } /* endif */
          } /*endif*/
       } /*endif*/
    } /*end for*/
    _save(somSelf,ev);
}

