/* Definitions used in SOMBank ATM application */


/* Used during create window loop to  */
/* identify main window as parent window */

#define MAIN     -1

/* Child Winodw Identifiers */


#define LOGO     0
#define PAD      LOGO+1
#define DISP     PAD+1
#define DISPMSG1 DISP+1
#define DISPMSG2 DISPMSG1+1
#define DISPMSG3 DISPMSG2+1
#define SSTATMSG DISPMSG3+1
#define KEY_0    SSTATMSG+1
#define KEY_1    KEY_0+1
#define KEY_2    KEY_0+2
#define KEY_3    KEY_0+3
#define KEY_4    KEY_0+4
#define KEY_5    KEY_0+5
#define KEY_6    KEY_0+6
#define KEY_7    KEY_0+7
#define KEY_8    KEY_0+8
#define KEY_9    KEY_0+9
#define OKB      KEY_9+1
#define CLR      OKB+1
#define CANCEL   CLR+1
#define EDITBOX  CANCEL+1
#define BUTGRP   EDITBOX+1
#define RBUT1    BUTGRP+1
#define RBUT2    BUTGRP+2
#define RBUT3    BUTGRP+3

#define TOTWINS  RBUT3+1

#define SAVBUT   RBUT2
#define CHKBUT   RBUT1
#define INQBUT   RBUT3
#define DEPBUT   RBUT2
#define WTHBUT   RBUT1

#define STEPS    6

#define ENTERID   0
#define ENTERPIN  1
#define ENTERACCT 2
#define ENTERWID  3
#define ENTERAMT  4
#define DISP_RESULTS 5

#define ENTER_CUSTOMERID 10
#define CLEAR_DISABLE_FIELDS 11
#define INITBUTTONS 12
#define DISABLEFIELDS 13
#define ENTER_CUSTOMER 14
#define DISABLEBUTTONS 15

#define CHECKING  3
#define SAVINGS   2

#define WITHDRAW  0
#define DEPOSIT   1
#define INQUIRE   2

#define APP_NAME  "SOM_ATM"
#define WIN_TITLE "SOMBank ATM"

#define ATM_I_ID     0
#define ATM_I_PIN    1
#define ATM_I_ACCT   2
#define ATM_I_WID    3
#define ATM_I_AMT    4
#define ATM_I_RES    5

#define ATM_P_ID     6
#define ATM_P_PIN    7
#define ATM_P_ACCT   8
#define ATM_P_WID    9
#define ATM_P_AMT    10
#define ATM_P_RES    11

#define ATM_E_NODATA   16
#define ATM_E_SHORTID  17
#define ATM_E_SHORTPIN 18
#define ATM_E_IDNF     19
#define ATM_E_BADPIN   20
#define ATM_E_XCAN     21

#define ATM_S_INIT     22
#define ATM_S_INITC    23
#define ATM_S_VALPIN   24

#define ATM_C_INITC    25
#define ATM_C_SHORTID  26
#define ATM_C_SHORTPIN 27
#define ATM_C_DISPRES  28
#define ATM_C_CLR      29

#define ATM_R_INQUIRE  32
#define ATM_R_DEPOSIT  33
#define ATM_R_WITHDRAW 34

#define ATM_PRESS_OK   35
#define ATM_CLR_MSG    36
#define ATM_CAN_MSG    37

#define ATM_ACCT_TYPE  38
#define ATM_CHECKING   38
#define ATM_SAVINGS    39

#define ATM_WID_TYPE   40
#define ATM_WITHDRAW   40
#define ATM_DEPOSIT    41
#define ATM_INQUIRE    42
