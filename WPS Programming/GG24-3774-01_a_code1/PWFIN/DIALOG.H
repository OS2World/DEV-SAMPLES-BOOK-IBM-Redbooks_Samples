#define ID_DLG_PASSWORD             100
#define ID_EF_PASSWORD              103

#define ID_DLG_FIND                 200
#define ID_EF_TELNUMBER             201
#define ID_EF_SURNAME               203

#define ID_LOCK                     300
#define ID_UNLOCK                   301

#define ID_CXTMENU_LOCK             0x6501
#define IDM_LOCK                    0x6502
#define IDM_QUERY                   0x6503

/*
 *   The following user-defined Popup menu items (ID_xxx) should be higher
 *   than WPMENUID_USER.
 *
 *   The ID_OPENFinanceFile will become a submenu of the system's popup open menu
 *   id, WPMENUID_OPEN.
 *
 *   These menu ids don't really matter.  The MENU could be any number.
 *   It is the MENUITEMS that have to be greater WPMENUID+*
 */
#define ID_OPENFinanceFile                   0x6504
#define IDM_OPENFinanceFile                  0x6505

/*   Set unique view ids. */
/*   define OPEN_ constant to represent the new view.  */
/*   *Must* be equal to the menu id used for this view.*/
#define OPEN_FinanceFile           IDM_OPENFinanceFile


#define ID_FRAME           3000                       /* client window id */

