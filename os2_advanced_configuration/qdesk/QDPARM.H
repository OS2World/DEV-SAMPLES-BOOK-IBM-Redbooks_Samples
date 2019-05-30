/* Header file for structure used by Query Desktop */

#define ACTION_QUERY_FROM_OBJECTHANDLE  0
#define ACTION_QUERY_FROM_OBJECTID      1
#define ACTION_FIND_OBJECT_FIRST        2
#define ACTION_FIND_OBJECT_NEXT         3
#define ACTION_FIND_OBJECT_END          4

#define NO_ERROR                        0
#define ERROR_CANNOT_CREATE_SHAREDMEM   1
#define ERROR_CANNOT_CREATE_OBJECT      2

typedef  struct
{
         ULONG      Action;
         ULONG      QueryRNRC;
         ULONG      FoundCt;
         ULONG      ErrorCode;
         SOMAny    *somDesktop;
         SOMAny    *somObject;
         HOBJECT    ObjHandle;
         BOOL       FindObjectRC;
         HFIND      FindHandle;
         USHORT     CallNr;
         CHAR       String[500];
}                   QDESK_PARM, * PQDESK_PARM;

         HOBJECT    CreateQDObject(PQDESK_PARM * qd, PUSHORT Retc, PSZ * Msg);
         VOID       DeleteQDObject(VOID);
         BOOL       SetQDObjectData(VOID);
