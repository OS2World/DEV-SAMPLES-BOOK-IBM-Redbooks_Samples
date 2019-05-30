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
 * File : fileutil.c       Module : fileutil.dll                        *
 *                                                                      *
 * Class : FileUtility                                                  *
 *                                                                      *
 *    Method(s):                 Public   Private                       *
 *                                                                      *
 *        identifyField                      X                          *
 *        fileUtilityLocate                  X                          *
 *        fileUtilityInsert        X                                    *
 *        fileUtilityUpdate        X                                    *
 *        fileUtilityDelete        X                                    *
 *        fileUtilityRetrieve      x                                    *
 *        fileUtilitySelect        X                                    *
 *        fileUtilityOpen          X                                    *
 *        fileUtilityGiveLastKey   X                                    *
 *        somInit                  X                                    *
 *        somUninit                x                                    *
 *                                                                      *
 * ---------------------------------------------------                  *
 * Description :                                                        *
 *                                                                      *
 * Define methods to access a binary file                               *
 * with fixed length records and a long key                             *
 *                                                                      *
 * This utility class was written to provide simple                     *
 * methods to access binary files.                                      *
 * ******************************************************************** *
 */
#include <stdio.h>
#ifndef _AIX_
   #include <io.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
 * included for TypeCode usage.
 */

#include <somtc.h>
#include "bankdef.h"
#include "sberr.h"

#define FileUtility_Class_Source
#include <fileutil.ih>

/*
 * identifyField is used in the select method to locate the
 * position and length in the structure of the field on witch a
 * comparison is made.
 * Example : locate the position and length of the customerId
 * field in the AccountRecord structure.
 * The position and length are then used to retrieve the value of
 * the field in an any._value buffer.
 *
 * Identifies the position and length of the field with name fieldName
 * in the record structure set with the open function.
 */

SOM_Scope long  SOMLINK identifyField(FileUtility somSelf,
                                      Environment *ev,
                                      string      fieldName,
                                      long        *position,
                                      long        *length)
{
   BankErr_EXCP *Bank_Excp;
   any name, type;
   int i;

   FileUtilityData *somThis = FileUtilityGetData(somSelf);
   FileUtilityMethodDebug("FileUtility","indentifyField");

   (*position) = 0;
   (*length) = 0;

   /*
    * The for loop will access the definition of all the fields in the
    * _recordTypeCode structure to find if there is a matching field name
    * for the fieldName passed as an argument to the method.
    */

    for (i=1; i < TypeCode_param_count(_recordTypeCode, ev) ; i+=2) {
         /*
          * the TypeCode_parameter function returns for field 1 the first
          * string describing the first entry in the structure.
          * for 2 you will get the type of this entry, in 3 the second string
          * in 4 the type of the second entry, and so on...
          */

          name = TypeCode_parameter(_recordTypeCode, ev, i);
          type = TypeCode_parameter(_recordTypeCode, ev, i+1);

          /*
           * we compare the name of the field in the structure with the name
           * passed as an argument.
           * Important ! the name._value does not contain the chars of the
           * string but a pointer to the actual array of chars.
           */

           if (strcmp( *(string *)name._value, fieldName )==0 ) {
               /*
                * the correct field has been found, set length
                */
               *length = TypeCode_size( *(TypeCode *)type._value, ev);
               return OK;
      }
      else
      { /*
         * increment the position counter by the size of i/2nth field.
         */
         (*position) += TypeCode_size ( *(TypeCode *)type._value, ev);
      }

   } /* endfor */
   /*
    * we did not return before, so the field name given is wrong. Return
    * a TYPE_MISMATCH.
    */
    Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
    Bank_Excp->errCode = FU_BAD_FIELD_NAME;
    strcpy(Bank_Excp->reason,FU_BAD_FIELD_NAME_TXT);
    somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
   return TYPE_MISMATCH;
}

/*
 * Locates a record in the file with the given Id. Position the cursor
 * on it.
 *
 * This function returns OK if a record is found with the given key, and
 * returns NOT_FOUND if any problem occurs.
 */

SOM_Scope long  SOMLINK fileUtilityLocate(FileUtility somSelf,
                                          Environment *ev,
                                          long        Id)
{
   long currentId;

   FileUtilityData *somThis = FileUtilityGetData(somSelf);
   FileUtilityMethodDebug("FileUtility","fileUtilityLocate");

   fseek(_fp, 0, SEEK_SET);

   while (1) {
      if (!fread(&currentId, sizeof (long ), 1, _fp))  /*EOF reached (maybe)*/
         return NOT_FOUND;

      if (currentId==Id) {
         fseek(_fp, -1L * sizeof (long),SEEK_CUR);
         return OK;
      } /* endif */

      fseek(_fp, _sizeOfRecord- sizeof(long) , SEEK_CUR);
   } /* endwhile */
}  /*FileUtilityLocate*/

/*
 * insert a record in sorted sequence in the file. Uses the first long
 * attribute as the key.
 */

SOM_Scope long  SOMLINK fileUtilityInsert(FileUtility somSelf,
                                          Environment *ev,
                                          any         *aRecord)
{
    void *readRecord;
    long readRecordId;
    long insertId;
   BankErr_EXCP *Bank_Excp;

    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","fileUtilityInsert");
    /*
     * retrieves the value of the first long field in the record to be
     * inserted. Another way of accessing this information would be to use
     * a long pointer to the _value buffer.
     */
    memcpy(&insertId, aRecord->_value, sizeof(long));

    /*
     * Call the locate function to check for records with the same key.
     */

    if (fileUtilityLocate( somSelf, ev, insertId)==0){
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = FU_DUP_KEY;
        strcpy(Bank_Excp->reason,FU_DUP_KEY_TXT);
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
        return DUPLICATE_KEY ;
    }

    fseek(_fp, 0L, SEEK_END);
    if (ftell(_fp)==0) {/* file is empty, write the record */
        if (fwrite(aRecord->_value,(size_t) _sizeOfRecord, 1, _fp)!=1){
            Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
            Bank_Excp->errCode = FU_WRT_ERR;
            strcpy(Bank_Excp->reason,FU_WRT_ERR_TXT);
            somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
            return WRITE_ERROR;
        } else {
            fflush(_fp);
            return OK;
        }
    } else {
        /*
         * Allocate memory to read one record at a time from the file.
         */

        readRecord = SOMMalloc ((size_t) _sizeOfRecord );
        fseek(_fp, -1L * _sizeOfRecord, SEEK_END);
        /*
         * Read the last record in the file.
         */
        if (fread(readRecord,(size_t) _sizeOfRecord, 1, _fp)!=1) {
            SOMFree( readRecord );
            Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
            Bank_Excp->errCode = FU_READ_ERR;
            strcpy(Bank_Excp->reason,FU_READ_ERR_TXT);
            somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
            return READ_ERROR;
        }
        /*
         * Copy the beginning of the readRecord wich is a long into a long
         * to perform comprisons on it.
         */

        memcpy(&readRecordId, readRecord, sizeof(long));
        /*
         * This loop will read all the records in the file in sequential
         * order beginning by the end of the file. All the records read will
         * be written one position further in the file to make room to
         * insert the new record, until a record with a lower key as the one
         * to insert is found in the read process.
         */

        while (readRecordId > insertId ) {
            /*
             * The record read has an Id greater than the one we want to write
             */
            fflush(_fp);
            /*
             * write record read after its previous position
             */
            fseek(_fp,0,SEEK_CUR); /* Needed for MSVC Compiler */
            fwrite(readRecord,(size_t) _sizeOfRecord,1 ,_fp );
            /*
             * test if we reached the beginning of the file before the read and
             * write operations. If it is true then nothing is left to read and
             * the new record can be written.
             */

            if (ftell(_fp) == 2 * _sizeOfRecord ) {
                fseek( _fp, -2 * _sizeOfRecord, SEEK_CUR );
                if (!fwrite(aRecord->_value,(size_t) _sizeOfRecord, 1, _fp)){
                    Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
                    Bank_Excp->errCode = FU_WRT_ERR;
                    strcpy(Bank_Excp->reason,FU_WRT_ERR_TXT);
                    somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
                    return WRITE_ERROR;
                }
                break;
            } else {
                /*
                 * read another record from the file. As in the previous
                 * iteration we performed a read and a write, to reach the next
                 * record to be read, a seek skipping 3 records is performed
                 * before the read operation.
                 */

                fseek(_fp, -3 * _sizeOfRecord, SEEK_CUR);
                fread(readRecord,(size_t) _sizeOfRecord, 1, _fp);
                memcpy(&readRecordId, readRecord, sizeof(long));
            } /* endif */
        } /* end of while loop */

        if (readRecordId < insertId) { /* while was terminated because a    */
            /*
             * record with a lower key was found.
             * Insert aRecord in the previous position of the last moved
             * record.
             */
            fflush(_fp);
            fseek(_fp,0,SEEK_CUR);  /* Needed for DOS!*/
            if (!fwrite(aRecord->_value,(size_t) _sizeOfRecord,1, _fp)) {
                Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
                Bank_Excp->errCode = FU_WRT_ERR;
                strcpy(Bank_Excp->reason,FU_WRT_ERR_TXT);
                somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
                return WRITE_ERROR;
            }
        } /* endif */
        fflush(_fp);
        /*
         * release the allocated buffer.
         */
        SOMFree( readRecord );
        return OK;
    } /* endif */
} /*FileUtilityInsert*/

/*
 * Update a record with the same first long attribute.
 */

SOM_Scope long  SOMLINK fileUtilityUpdate(FileUtility somSelf,
                                          Environment *ev,
                                          any         *aRecord)
{
    long aRecordId;
   BankErr_EXCP *Bank_Excp;

    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","fileUtilityUpdate");

    /*
     * store the contents of the long bytes at the beginning of the
     * the record in a long variable.
     */

    memcpy( &aRecordId, aRecord->_value, sizeof(long));

    /*
     * search the file for the record to update and position the cursor.
     */
    if (fileUtilityLocate( somSelf, ev, aRecordId)==OK) { /*record found*/
        fseek(_fp,0,SEEK_CUR);  /* Needed for MS VC++!*/
        if (fwrite(aRecord->_value,(size_t) _sizeOfRecord, 1, _fp)!=1){
            Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
            Bank_Excp->errCode = FU_WRT_ERR;
            strcpy(Bank_Excp->reason,FU_WRT_ERR_TXT);
            somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
            return WRITE_ERROR;
        } else {
            fflush(_fp);
            return OK;
        } /* end if*/
    } else {
        return NOT_FOUND;
    } /*end if*/
}/*FileUtilityUpdate*/

/*
 *
 * Delete a record based on key
 *
 *   Note: uses non-ANSI C functions chsize and fileno
 *
 */

SOM_Scope long  SOMLINK fileUtilityDelete(FileUtility somSelf,
                                           Environment *ev, long key)
{
    int rc;
    void *readRecord;
    BankErr_EXCP *Bank_Excp;

    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","fileUtilityDelete");

    if (fileUtilityLocate(somSelf,ev,key)!=0) {
       Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
       Bank_Excp->errCode = FU_REC_NOT_FOUND;
       strcpy(Bank_Excp->reason,FU_REC_NOT_FOUND_TXT);
       somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return NOT_FOUND;
    } else {
        readRecord = SOMMalloc ((size_t) _sizeOfRecord );
        while (1) {
            fseek(_fp,0L,SEEK_CUR);
            fseek(_fp,_sizeOfRecord,SEEK_CUR);
            rc =  fread(readRecord,(size_t) _sizeOfRecord, 1, _fp);
            if (rc!=1) {
                if (feof(_fp)==0) {
                    Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
                    Bank_Excp->errCode = FU_READ_ERR;
                    strcpy(Bank_Excp->reason,FU_READ_ERR_TXT);
                    somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
                    return READ_ERROR;
                } else {
                    fseek(_fp,-1L * _sizeOfRecord,SEEK_END);
#ifdef _AIX
                    ftruncate(fileno(_fp), ftell(_fp));
#else
                    _chsize(_fileno(_fp), ftell(_fp));
#endif
                    return OK;
                } /*end if*/
            } /* end if*/
            fseek(_fp,-2*_sizeOfRecord,SEEK_CUR);
            fwrite(readRecord,(size_t) _sizeOfRecord,1,_fp);
        } /*end while*/
    } /* end if*/
}

/*
 * Locate a record with the given key and put it into aRecord.
 */

SOM_Scope long  SOMLINK fileUtilityRetrieve(FileUtility somSelf,
                                            Environment *ev,
                                            long        key,
                                            any         *aRecord)
{
   BankErr_EXCP *Bank_Excp;
    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","fileUtilityRetrieve");

   if ( fileUtilityLocate( somSelf, ev, key )==OK) {
      if (fread(aRecord->_value,(size_t) _sizeOfRecord,1,_fp)!=1){
          Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
          Bank_Excp->errCode = FU_READ_ERR;
          strcpy(Bank_Excp->reason,FU_READ_ERR_TXT);
          somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
          return READ_ERROR ;
      } else {
         fflush(_fp);
         return OK;
      } /*end if*/
   } else {
        Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
        Bank_Excp->errCode = FU_REC_NOT_FOUND;
        strcpy(Bank_Excp->reason,FU_REC_NOT_FOUND_TXT);
        somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
        return NOT_FOUND;
   } /* endif */
} /*fileUtilityRetrieve*/

/*
 * Searches the file for all the records that have the same field value
 * as the given one. Locates the position of the field in the record
 * structure by using the selectFieldName.
 *
 * The value argument specified as a void cannot be used with DSOM. An
 * any structure should be passed if you want to use DSOM.
 * If more than 10 records are found, the function returns
 * ARRAY_OVERFLOW, but count and arrayOfIds are set correctly.
 */

SOM_Scope long  SOMLINK fileUtilitySelect(FileUtility somSelf,
                                          Environment *ev,
                                          string      selectFieldName,
                                          void        *value,
                                          long        *count,
                                          long        arrayOfIds[10])
{
   BankErr_EXCP *Bank_Excp;
   void *readRecord;
   long i=0;
   long startingPosition, length;

   FileUtilityData *somThis = FileUtilityGetData(somSelf);
   FileUtilityMethodDebug("FileUtility","fileUtilitySelect");

   *count=0;

   fseek(_fp, 0, SEEK_SET );
   readRecord = SOMMalloc((size_t) _sizeOfRecord );
   /*
    * Identify field will give information on the position and length of
    * field on wich a comparison is to be made. The value of the bytes in
    * memory will then be compared to the given value. This does not work
    * for strings as the unused bytes in a string are undefined.
    * An improvement of this function would be to use the type of the
    * field on wich the comparison is to be made to perform the correct
    * operation.
    */

   if ( FileUtility_identifyField( somSelf, ev, selectFieldName, &startingPosition, &length))
       return TYPE_MISMATCH;

   /*
    * The while loop will scan the file in sequential order and store the
    * Id of all the entries that match the selection criteria in the array
    * of Id. The while loop will end when 10 matches are found or the end
    * of file is reached.
    */

   while (feof(_fp)==0 && i < 11) {
      if (fread( readRecord,(size_t) _sizeOfRecord, 1, _fp)==1) {
          /*
           * a memory comparison is made between the value buffer passed and
           * the corresponding bytes in the readRecord buffer.
           * If a matching record is found, copy the key in array.
           */
          if (memcmp((char *)readRecord + startingPosition, value,(size_t) length) == 0) {
              if (i==10) { /* 11 records matching the criteria were found    */
                  *count = i;
                  Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
                  Bank_Excp->errCode = FU_ARRAY_OVFLW;
                  strcpy(Bank_Excp->reason,FU_ARRAY_OVFLW_TXT);
                  somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
                  return ARRAY_OVERFLOW;
              } else {
                  memcpy(&(arrayOfIds[i]), readRecord, sizeof( long ));
                  i++;
              } /* endif */
          } /* endif */
      } /* endif */
   } /* endwhile */
   SOMFree (readRecord);

   if (i==0) {
       Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
       Bank_Excp->errCode = FU_REC_NOT_FOUND;
       strcpy(Bank_Excp->reason,FU_REC_NOT_FOUND_TXT);
       somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
       return NOT_FOUND;
   } else {
      *count=i;
      return OK;
   } /* endif */
}

/*
 * This function performs initialisation of the file pointer.
 * could be done in the constructor of the object.
 */

SOM_Scope long  SOMLINK fileUtilityOpen(FileUtility somSelf,
                                        Environment *ev,
                                        char        fileName[13],
                                        TypeCode    theRecordTypeCode)
{
   BankErr_EXCP *Bank_Excp;
#ifdef _WIN16_
    struct _stat fileInfo;  /* used for _fstat  */
#else
    struct stat fileInfo;  /* used for _fstat  */
#endif

    /*
     * exception structure to be returned in case the file cannot be
     * accessed.
     */

    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","fileUtilityOpen");

    /*
     * store the size of a record in a variable. This size is fixed and
     * cannot be changed for the lifetime of the object.
     * Currently the TypeCode of the record is stored but no dynamic
     * type checking is done by the methods.
     */
    _sizeOfRecord = TypeCode_size( theRecordTypeCode, ev );
    _recordTypeCode = theRecordTypeCode;

    if ((_fp = fopen(fileName,"rb+"))==NULL) { /* file is not open */
#ifdef _WIN16_
        if (_stat( fileName , &fileInfo )==-1) { /* file name unknown */
#else
        if (stat( fileName , &fileInfo )==-1) { /* file name unknown */
#endif
            if ((_fp = fopen( fileName , "wb+" ) )==NULL) {
                /*
                 * raise an exception in the server.
                 */
                Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
                Bank_Excp->errCode = FU_FILE_CREATE_ERR;
                strcpy(Bank_Excp->reason,FU_FILE_CREATE_ERR_TXT);
                somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
                return CREATE_ERROR;
            }; /* endif */
        } else {
            /*
             * Information on the file has been found by stat.

             * Raise an exception in the server.
             */
            Bank_Excp= (BankErr_EXCP *) SOMMalloc(sizeof(BankErr_EXCP));
            Bank_Excp->errCode = FU_FILE_ACC_ERR;
            strcpy(Bank_Excp->reason,FU_FILE_ACC_ERR_TXT);
            somSetException(ev,USER_EXCEPTION,ex_BankErr_EXCP, (void *)Bank_Excp);
            return ACCESS_ERROR;
        } /* endif */
    } /* endif */
    return OK;
}


/*
 * Returns the key of the last record in the file.
 * private methods.
 */

SOM_Scope long  SOMLINK fileUtilityGiveLastKey(FileUtility somSelf,
                                               Environment *ev)
{
   long lastId;
   FileUtilityData *somThis = FileUtilityGetData(somSelf);
   FileUtilityMethodDebug("FileUtility","fileUtilityGiveLastKey");

    fseek(_fp, 0L, SEEK_END );

    /*
     * test if file is empty
     */
    if (ftell(_fp)==0) {
       return 0L;
    } else {
        fseek(_fp, -1L * _sizeOfRecord , SEEK_CUR);
        if ( fread( &lastId, sizeof(long), 1, _fp) !=1 ) {
            /*
             * we had a read error,
             * if the file is not available, no insert will be possible
             * so the Id is allowed to be wrong
             */
            return 0L;
        } else {
            /*
             * lastId has been read from the file
             */
            return lastId;
        }/*end if*/
    } /*end if*/
}/*fileUtilityGiveLastKey*/

SOM_Scope void  SOMLINK somInit(FileUtility somSelf)
{
    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","somInit");

    _sizeOfRecord = 0;
    _fp = NULL;

    FileUtility_parent_SOMObject_somInit(somSelf);
}

SOM_Scope void  SOMLINK somUninit(FileUtility somSelf)
{
    FileUtilityData *somThis = FileUtilityGetData(somSelf);
    FileUtilityMethodDebug("FileUtility","somUninit");

    fclose(_fp);

    FileUtility_parent_SOMObject_somUninit(somSelf);
}

