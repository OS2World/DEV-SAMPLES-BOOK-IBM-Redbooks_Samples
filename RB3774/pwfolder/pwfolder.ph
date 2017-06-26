
/*
 * This file was generated by the SOM Compiler.
 * FileName: pwfolder.ph.
 * Generated using:
 *     SOM Precompiler spc: 1.22
 *     SOM Emitter emitph: 1.10
 */

#ifndef pwfolder_ph
#define pwfolder_ph


/*
 * Passthru lines: File: "C.ph", "before"
 */

typedef struct _PWF_INFO {                       /* Define password structure */
  CHAR   szPassword[20];                         /* Folder current password   */
  CHAR   szCurrentPassword[20];                  /* User-entered password     */
  CHAR   szUserid[20];                           /* Userid                    */
} PWF_INFO;
typedef PWF_INFO *PPWF_INFO;                     /* Define pointer type       */


#include "pwfolder.h"

/*
 * Include any needed private header files
 */

/*
 * New Method: QueryInfo
 */
typedef BOOL    SOMLINK somTP_PWFolder_QueryInfo(PWFolder *somSelf,
		PPWF_INFO pPWFolderInfo);
#pragma linkage(somTP_PWFolder_QueryInfo, system)
typedef somTP_PWFolder_QueryInfo *somTD_PWFolder_QueryInfo;
#define somMD_PWFolder_QueryInfo "----"
#define PWFolder_QueryInfo(somSelf,pPWFolderInfo) \
    (SOM_Resolve(somSelf, PWFolder, QueryInfo) \
	(somSelf,pPWFolderInfo))
#define _QueryInfo PWFolder_QueryInfo

/*
 * New Method: SetInfo
 */
typedef BOOL    SOMLINK somTP_PWFolder_SetInfo(PWFolder *somSelf,
		PPWF_INFO pPWFolderInfo);
#pragma linkage(somTP_PWFolder_SetInfo, system)
typedef somTP_PWFolder_SetInfo *somTD_PWFolder_SetInfo;
#define somMD_PWFolder_SetInfo "----"
#define PWFolder_SetInfo(somSelf,pPWFolderInfo) \
    (SOM_Resolve(somSelf, PWFolder, SetInfo) \
	(somSelf,pPWFolderInfo))
#define _SetInfo PWFolder_SetInfo

#endif       /* pwfolder_ph */

/*
 * This file was generated by the SOM Compiler.
 * FileName: pwfolder.ph.
 * Generated using:
 *     SOM Precompiler spc: 1.22
 *     SOM Emitter emitph: 1.10
 */

#ifndef pwfolder_mph
#define pwfolder_mph


/*
 * Include any needed private header files
 */

#endif       /* pwfolder_ph */
