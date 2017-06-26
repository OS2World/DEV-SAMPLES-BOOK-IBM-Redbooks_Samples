

#define WMP_INITIALISE_REQUESTER             (WM_USER+101)
#define WMP_SERVER_READY                     (WM_USER+102)
#define WMP_SEARCH_DATABASE                  (WM_USER+103)
#define WMP_SERVER_TERMINATED                (WM_USER+104)
#define WMP_RESULTS_FROM_SEARCH              (WM_USER+106)
#define WMP_START_SERVER_PROGRAM             (WM_USER+107)
#define WMP_SEND_MSG_TO_SERVER               (WM_USER+108)
#define WMP_INSERT_DATA_INTO_VIEW            (WM_USER+109)
#define WMP_DISPLAY_DATABASE_QUERY_DIALOG    (WM_USER+110)
#define WMP_INITIALISE_SERVER                (WM_USER+111)
#define WMP_IPC_END                          (WM_USER+112)

#define SERVER_PROGRAM "server.exe"


typedef struct {
    ULONG  folder;
    CHAR   szSearch[100];
    CHAR   szDateTime;
} QUERY_DATA, *PQUERY_DATA;


typedef struct {
    ULONG  folder;
    USHORT cRecords;
    CHAR   szRecordType[20];
    CHAR   szSearch[100];
} RESULTS_DATA, *PRESULTS_DATA;


typedef struct {
    CHAR szName[100];
    CHAR szAddress[300];
    CHAR szTelepnone[40];
} PERSON_DATA, *PPERSON_DATA;

