/* ****************************************************** */
/*                                                        */
/* File : bankdef.h                                       */
/*                                                        */
/* Include File for                                       */
/*                                                        */
/* SOMBank Sample Application - ITSO Redbook GG24-4357    */
/*                                                        */
/* "SOMobjects: A Practical Introduction to SOM and DSOM" */
/*                                                        */
/* ****************************************************** */
#ifndef BANKDEF
#define BANKDEF

#ifndef ___check
#define ___check
long check(Environment e);

#endif

enum RCODE {OK,
            DUPLICATE_KEY,
            WRITE_ERROR,
            READ_ERROR,
            NOT_FOUND,
            TYPE_MISMATCH,
            ARRAY_OVERFLOW,
            SOM_ENVIRONMENT,
            CREATE_ERROR,
            RESTORE_ERROR,
            DELETE_ERROR,
            ACCESS_ERROR,
            BAD_PIN,
            BAD_CUSTOMER,
            INT_RATE,
            GENERAL_ERROR,
            OVERDRAWN,
            WITHDRAW_ERROR,
            DEPOSIT_ERROR
            };

/* File to use for ServerBroker persistence */
#define SVRBROKRDAT "srvbrokr.dat"

#define MAX_ALIAS_SZ 20
#define MAX_ACCTS    2



/* Values used for SOMBank Exceptions               */
/* SOMBank Exceptions are declared as :             */
/* exception EXCP {long errCode; char reason[40];}; */

#define AM_BDB_ACC_ERR      1001
#define AM_BDB_ACC_ERR_TXT  "AccMgr: Can't access BranchDB"
#define AM_BDB_SAF_ERR      1002
#define AM_BDB_SAF_ERR_TXT  "AccMgr: SelectAccts Failed"
#define AM_BDB_RAF_ERR      1003
#define AM_BDB_RAF_ERR_TXT  "AccMgr: retrieveAccount Failed"
#define AM_BDB_DAF_ERR      1004
#define AM_BDB_DAF_ERR_TXT  "AccMgr: deleteAccount Failed"


#define FU_BAD_FIELD_NAME   2001
#define FU_BAD_FIELD_NAME_TXT  "FileUtility: Bad Field Name"
#define FU_DUP_KEY     2002
#define FU_DUP_KEY_TXT  "FileUtility: Duplicate Key"
#define FU_WRT_ERR      2003
#define FU_WRT_ERR_TXT  "FileUtility: Write Error"
#define FU_READ_ERR      2004
#define FU_READ_ERR_TXT  "FileUtility: Read Error"
#define FU_REC_NOT_FOUND      2005
#define FU_REC_NOT_FOUND_TXT  "FileUtility: Record Not Found"
#define FU_ARRAY_OVFLW      2006
#define FU_ARRAY_OVFLW_TXT  "FileUtility: Result Array Overflow"
#define FU_FILE_CREATE_ERR      2007
#define FU_FILE_CREATE_ERR_TXT  "FileUtility: File Creation Error"
#define FU_FILE_ACC_ERR     2008
#define FU_FILE_ACC_ERR_TXT  "FileUtility: File Access Error"

#define SA_INT_RATE_ERR     3001
#define SA_INT_RATE_ERR_TXT  "SavingsAccount: Bad Interest Rate"

#endif /*BANKDEF*/
