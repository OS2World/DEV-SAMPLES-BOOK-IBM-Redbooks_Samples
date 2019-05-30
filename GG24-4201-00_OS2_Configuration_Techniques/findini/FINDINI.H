#define  ID_0      1000
#define  ID_1      1001
#define  ID_2      1002

#define  LI_ONE    1500
#define  LI_TWO    1501

#define  PB_SWITCH 1003
#define  PB_ABOUT  1004
#define  PB_EXIT   1005
#define  PB_OK     1006

#define  STX_1     1007
#define  STX_2     1017
#define  STX_ONE   1008
#define  STX_8     1009

#define  IC_1      1201
#define  IC_2      1202
#define  IC_3      1203
#define  IC_4      1204
#define  IC_5      1205
#define  IC_6      1206

#define  IDS_0     0
#define  IDS_1     1
#define  IDS_2     2
#define  IDS_3     3
#define  IDS_4     4
#define  IDS_5     5
#define  IDS_6     6
#define  IDS_7     7
#define  IDS_8     8

#define MSG_ONE    2201
#define MSG_TWO    2202

#define WM_ADDIT   WM_USER+10
#define WM_DELIT   WM_USER+11
#define WM_ADDTO   WM_USER+12
#define WM_STOPD   WM_USER+14
#define WM_ADDIT2  WM_USER+15
#define WM_DELIT2  WM_USER+16

#define ACTION_QUERY_FROM_OBJECTHANDLE  0
#define MAX_EANAMESIZE   500

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