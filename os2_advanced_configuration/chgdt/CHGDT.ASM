	TITLE	CHGDT.C
	.386
	.387
	INCLUDELIB os2386.lib
	INCLUDELIB dde4sbs.lib
CODE32	SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32	ENDS
DATA32	SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32	ENDS
CONST32	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST32	ENDS
BSS32	SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS32	ENDS
DGROUP	GROUP CONST32, BSS32, DATA32
	ASSUME	CS:FLAT, DS:FLAT, SS:FLAT, ES:FLAT
	EXTRN	strcmp:PROC
	EXTRN	stricmp:PROC
	EXTRN	strlen:PROC
	EXTRN	memicmp:PROC
	EXTRN	_printfieee:PROC
	EXTRN	WinInitialize:PROC
	EXTRN	DosAllocMem:PROC
	EXTRN	WinTerminate:PROC
	EXTRN	PrfQueryProfile:PROC
	EXTRN	strcpy:PROC
	EXTRN	DosFreeMem:PROC
	EXTRN	QueryDesktopDirectory:PROC
	EXTRN	GetEALongname:PROC
	EXTRN	free:PROC
	EXTRN	malloc:PROC
	EXTRN	DosQueryPathInfo:PROC
	EXTRN	SetDesktopDirectory:PROC
	EXTRN	CheckIniFile:PROC
	EXTRN	strupr:PROC
	EXTRN	PrfQueryProfileData:PROC
	EXTRN	PrfReset:PROC
	EXTRN	PrfOpenProfile:PROC
	EXTRN	DosSleep:PROC
	EXTRN	PrfWriteProfileData:PROC
	EXTRN	PrfCloseProfile:PROC
	EXTRN	getenv:PROC
	EXTRN	_putenv:PROC
	EXTRN	_sprintfieee:PROC
	EXTRN	_tempnam:PROC
	EXTRN	fopen:PROC
	EXTRN	fclose:PROC
	EXTRN	fgets:PROC
	EXTRN	strspn:PROC
	EXTRN	fputs:PROC
	EXTRN	DosDelete:PROC
	EXTRN	DosMove:PROC
	EXTRN	_exeentry:PROC
DATA32	SEGMENT
@30Application	DB "PM_Workplace:Location",0H
	ALIGN 04H
@32Keyname	DB "<WP_DESKTOP>",0H
	ALIGN 04H
@6cEnvVarName	DB "TMP",0H
@STAT1	DB "/?",0H
@STAT2	DB "/C",0H
	ALIGN 04H
@STAT3	DB "/D:",0H
@STAT4	DB "Invalid parameter %s ign"
DB "ored.",0aH,0H
@STAT5	DB "?",0H
	ALIGN 04H
@STAT6	DB "Unable to initialize app"
DB "lication to PM.",0aH,0H
	ALIGN 04H
@STAT7	DB "Allocate Error, rc = %ld"
DB 0aH,0H
	ALIGN 04H
@STAT8	DB "Actual User-Profile: %s",0aH
DB 0H
	ALIGN 04H
@STAT9	DB "Actual System-Profile: %"
DB "s",0aH,0H
	ALIGN 04H
@STATa	DB "Error in PrfQueryProfile"
DB 0aH,0H
	ALIGN 04H
@STATb	DB "The actual desktop direc"
DB "tory is %s",0aH,0H
@STATc	DB "The desktop name is %s",0aH,0H
@STATd	DB "No longname found for th"
DB "e desktop.",0aH,0H
@STATe	DB "Desktop directory cannot"
DB " be queried, ",0H
	ALIGN 04H
@STATf	DB "location for WP_DESKTOP "
DB "not",0aH,"found in INI file.",0aH,0H
@STAT10	DB "unable to allocate share"
DB "d memory.",0aH,0H
	ALIGN 04H
@STAT11	DB "object MyGEQueryObject i"
DB "n class",0aH,"GEQueryObject co"
DB "uldn't be created. Maybe"
DB " QDESK.DLL is not instal"
DB "led",0aH,"or registered in the"
DB " system.",0aH,0H
	ALIGN 04H
@STAT12	DB "error in WinSetObjectDat"
DB "a.",0aH,0H
@STAT13	DB "No name for new user ini"
DB " defined",0aH,0H
	ALIGN 04H
@STAT14	DB "Allocation error for INI"
DB " file name.",0aH,0H
	ALIGN 04H
@STAT15	DB "Error %ld in DosQueryPat"
DB "hInfo for %s.",0aH,0H
	ALIGN 04H
@STAT16	DB "Set desktop directory, r"
DB "c = %u",0aH,0H
@STAT17	DB "Ini file %s not found.",0aH,0H
@STAT18	DB "The file %s is no valid "
DB "user ini file or is",0aH,"curr"
DB "ently in use by the syst"
DB "em.",0aH,0H
	ALIGN 04H
@STAT19	DB "Memory allocation error",0aH
DB 0H
	ALIGN 04H
@STAT1a	DB "Handle of desktop object"
DB " not found in INI file %"
DB "s.",0aH,0H
@STAT1b	DB "Unable to allocate share"
DB "d memory.",0aH,0H
	ALIGN 04H
@STAT1c	DB "Object MyGEQueryObject i"
DB "n class GEQueryObject co"
DB "uldn't be created.",0aH,"Maybe"
DB " QDESK.DLL is not instal"
DB "led or registered in the"
DB " system.",0aH,0H
	ALIGN 04H
@STAT1d	DB "The directory structure "
DB "associated to inifile %s"
DB 0aH,"cannot be located.",0aH,0H
	ALIGN 04H
@STAT1e	DB "The desktop directory %s"
DB " couldn't be located.",0aH,0H
	ALIGN 04H
@STAT1f	DB "Cannot update INI file %"
DB "s.",0aH,0H
@STAT20	DB "INI file %s successfully"
DB " updated with new direct"
DB "ory structure",0aH,0H
	ALIGN 04H
@STAT21	DB "Allocate Error, rc = %ld"
DB 0aH,0H
	ALIGN 04H
@STAT22	DB "New User-Profile: %s",0aH,"New"
DB " System-Profile: %s",0aH,0H
	ALIGN 04H
@STAT23	DB "Desktop directory used b"
DB "y %s is %s",0aH,0H
@STAT24	DB "The desktop name is %s",0aH,0H
@STAT25	DB "No longname found for th"
DB "e desktop.",0aH,0H
@STAT26	DB "Desktop directory used b"
DB "y %s is %s",0aH,0H
@STAT27	DB "The desktop name is %s",0aH,0H
@STAT28	DB "No longname found for th"
DB "e desktop.",0aH,0H
@STAT29	DB "Desktop directory cannot"
DB " be queried, maybe QDESK"
DB ".DLL is not installed.",0aH,0H
@STAT2a	DB "Desktop key in %s update"
DB "d.",0aH,0H
@STAT2b	DB "Error while switching to"
DB " %s.",0aH,0H
	ALIGN 04H
@STAT2c	DB 0aH,"CHGDT - Change actual D"
DB "esktop",0aH,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0aH,0aH,"Syntax:",0aH,0aH," "
DB "    CHGDT ",0c4H,0c4H,0c4H,0c2H,0c4H,0c2H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c2H,0c4H,0c4H,0c2H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c2H,0c4H,0c4H,0c2H,0c4H,0c4H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c2H,0c4H,0c2H,0c4H,0c4H,0c4H,0b4H,0aH,"   "
DB "           ",0b3H," ",0c0H,0c4H," new ini "
DB "name ",0c4H,0d9H,"  ",0c0H,0c4H," /C ",0c4H,0d9H,"  ",0c0H,0c4H," /D"
DB ":dir name ",0c4H,0d9H," ",0b3H,0aH,"         "
DB "     ",0b3H,"                  "
DB "                        "
DB "       ",0b3H,0aH,"              ",0c3H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H," /? "
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H
DB 0c4H,0b4H,0aH,"              ",0c0H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H," ? ",0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H
DB 0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0c4H,0d9H,0aH,0aH,"  "
DB "   new ini name     Swit"
DB "ch to that ini file",0aH,"    "
DB " /C               Update"
DB " CONFIG.SYS",0aH,"     /? or ?"
DB "          Display the sy"
DB "ntax",0aH,"     dir name      "
DB "   Name of associated di"
DB "rectory structure to use"
DB 0aH,"                      w"
DB "ith the new ini file",0aH,0aH,"  "
DB "   CHGDT without paramet"
DB "er will display the actu"
DB "al INI files and",0aH,"     de"
DB "sktop directory.",0aH,0H
	ALIGN 04H
@STAT2d	DB "%c:\",0H
	ALIGN 04H
@STAT2e	DB "%c:\CONFIG.SYS",0H
@STAT2f	DB "r",0H
	ALIGN 04H
@STAT30	DB "Cannot open %s",0aH,0H
@STAT31	DB "w",0H
	ALIGN 04H
@STAT32	DB "Cannot create tempfile %"
DB "s",0aH,0H
@STAT33	DB " ",0H
	ALIGN 04H
@STAT34	DB "SET ",0H
@STAT35	DB " ",0H
	ALIGN 04H
@STAT36	DB "USER_INI=",0H
	ALIGN 04H
@STAT37	DB "%s",0aH,0H
@STAT38	DB "%s successfully updated."
DB 0aH,0H
	ALIGN 04H
@STAT39	DB "Cannot rename tempfile %"
DB "s to %s.",0aH,0H
	ALIGN 04H
@STAT3a	DB "Unable to replace %s.",0aH,0H
	ALIGN 04H
@STAT3b	DB "Nothing found to update "
DB "%s.",0aH,0H
	DD	_exeentry
DATA32	ENDS
BSS32	SEGMENT
BSS32	ENDS
CONST32	SEGMENT
CONST32	ENDS
CODE32	SEGMENT

;***** 23          int        main(int argc, char *argv[])
	ALIGN 04H

	PUBLIC main
main	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,0178H
	PUSH	EBX
	SUB	ESP,0cH
	MOV	[EBP+08H],EAX;	argc
	MOV	[EBP+0cH],EDX;	argv

;***** 35          PSZ        NewIniName = (PSZ)NULL;
	MOV	DWORD PTR [EBP-04H],0H;	NewIniName

;***** 36          int        Flag = 0;
	MOV	DWORD PTR [EBP-08H],0H;	Flag

;***** 37          APIRET     rc;
;***** 38          int        i1;
;***** 39          PSZ        DTDir;
;***** 40          PSZ        IniName;
;***** 41          PSZ        DirName = (PSZ)NULL;
	MOV	DWORD PTR [EBP-0cH],0H;	DirName

;***** 42          USHORT     CheckRC;
;***** 43          PSZ        Longname;
;***** 44          ULONG      DataLen;
;***** 45          HINI       hini;
;***** 46          CHAR       OldIniFile[300];
;***** 47 
;***** 48 /* Check parameters */
;***** 49 for (i1 = 1; i1 < argc; i1++)
	MOV	DWORD PTR [EBP-010H],01H;	i1
	ALIGN 04H
BLBL1:
	MOV	EAX,[EBP+08H];	argc
	CMP	[EBP-010H],EAX;	i1
	JGE	BLBL2

;***** 50    {
;***** 51    if (argv[i1][0] == '/')
	MOV	EAX,[EBP+0cH];	argv
	MOV	ECX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+ECX*04H]
	CMP	BYTE PTR [EAX],02fH
	JNE	BLBL3

;***** 52       {
;***** 53       if (!strcmp(argv[i1], "/?"))
	MOV	EDX,OFFSET FLAT:@STAT1
	MOV	EAX,[EBP+0cH];	argv
	MOV	ECX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+ECX*04H]
	CALL	strcmp
	OR	EAX,EAX
	JNE	BLBL4

;***** 54          {
;***** 55          Flag = DISPLAY_SYNTAX;
	MOV	DWORD PTR [EBP-08H],01H;	Flag

;***** 56          break;
	JMP	BLBL2
	ALIGN 04H
BLBL4:

;***** 57          }
;***** 58       if (!stricmp(argv[i1], "/C"))
	MOV	EDX,OFFSET FLAT:@STAT2
	MOV	EAX,[EBP+0cH];	argv
	MOV	ECX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+ECX*04H]
	CALL	stricmp
	OR	EAX,EAX
	JNE	BLBL5

;***** 59          {
;***** 60          Flag |= CHANGE_CONFIG_SYS;
	MOV	EAX,[EBP-08H];	Flag
	OR	AL,02H
	MOV	[EBP-08H],EAX;	Flag

;***** 61          continue;
	JMP	BLBL6
	ALIGN 04H
BLBL5:

;***** 62          }
;***** 63       if (strlen(argv[i1]) > 3)
	MOV	EAX,[EBP+0cH];	argv
	MOV	ECX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+ECX*04H]
	CALL	strlen
	CMP	EAX,03H
	JBE	BLBL7

;***** 64          {
;***** 65          if (!memicmp(argv[i1], "/D:", 3))
	MOV	ECX,03H
	MOV	EDX,OFFSET FLAT:@STAT3
	MOV	EAX,[EBP+0cH];	argv
	MOV	EBX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+EBX*04H]
	CALL	memicmp
	OR	EAX,EAX
	JNE	BLBL8

;***** 66             {
;***** 67             Flag |= CHANGE_ASSOC_DIR;
	MOV	EAX,[EBP-08H];	Flag
	OR	AL,04H
	MOV	[EBP-08H],EAX;	Flag

;***** 68             DirName = &argv[i1][3];
	MOV	EAX,[EBP+0cH];	argv
	MOV	ECX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+ECX*04H]
	ADD	EAX,03H
	MOV	[EBP-0cH],EAX;	DirName

;***** 69             continue;
	JMP	BLBL6
	ALIGN 04H
BLBL8:
BLBL7:

;***** 70             }
;***** 71          }
;***** 72       printf("Invalid parameter %s ignored.\n", argv[i1]);
	MOV	EAX,[EBP+0cH];	argv
	MOV	EBX,[EBP-010H];	i1
	PUSH	DWORD PTR [EAX+EBX*04H]
	MOV	EAX,OFFSET FLAT:@STAT4
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
	JMP	BLBL9
	ALIGN 04H
BLBL3:

;***** 73       }
;***** 74    else
;***** 75       {
;***** 76       if (!strcmp(argv[i1], "?"))
	MOV	EDX,OFFSET FLAT:@STAT5
	MOV	EAX,[EBP+0cH];	argv
	MOV	EBX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+EBX*04H]
	CALL	strcmp
	OR	EAX,EAX
	JNE	BLBL10

;***** 77          {
;***** 78          Flag = DISPLAY_SYNTAX;
	MOV	DWORD PTR [EBP-08H],01H;	Flag

;***** 79          break;
	JMP	BLBL2
	ALIGN 04H
BLBL10:

;***** 80          }
;***** 81       NewIniName = argv[i1];
	MOV	EAX,[EBP+0cH];	argv
	MOV	EBX,[EBP-010H];	i1
	MOV	EAX,DWORD PTR [EAX+EBX*04H]
	MOV	[EBP-04H],EAX;	NewIniName
BLBL9:
BLBL6:
	MOV	EAX,[EBP-010H];	i1
	INC	EAX
	MOV	[EBP-010H],EAX;	i1
	JMP	BLBL1
	ALIGN 04H
BLBL2:

;***** 82       }
;***** 83    }
;***** 84 
;***** 85 if (Flag & DISPLAY_SYNTAX)
	TEST	BYTE PTR [EBP-08H],01H;	Flag
	JE	BLBL11

;***** 86    {
;***** 87    Syntax();
	CALL	Syntax

;***** 88    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL11:

;***** 89    }
;***** 90 
;***** 91 hab = WinInitialize(0);
	PUSH	0H
	MOV	AL,01H
	CALL	WinInitialize
	ADD	ESP,04H
	MOV	[EBP-014H],EAX;	hab

;***** 92 
;***** 93 if (!hab)
	CMP	DWORD PTR [EBP-014H],0H;	hab
	JNE	BLBL13

;***** 94    {
;***** 95    printf("Unable to initialize application to PM.\n");
	MOV	EAX,OFFSET FLAT:@STAT6
	CALL	_printfieee

;***** 96    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL13:

;***** 97    }
;***** 98 
;***** 99 rc = DosAllocMem(&Ptr, ALLOC_SIZE, PAG_COMMIT | PAG_READ | PAG_WRITE);
	PUSH	013H
	PUSH	01000H
	LEA	EAX,[EBP-01cH];	Ptr
	PUSH	EAX
	MOV	AL,03H
	CALL	DosAllocMem
	ADD	ESP,0cH
	MOV	[EBP-018H],EAX;	rc

;***** 100 
;***** 101 if (rc)
	CMP	DWORD PTR [EBP-018H],0H;	rc
	JE	BLBL14

;***** 102    {
;***** 103    printf("Allocate Error, rc = %ld\n", (LONG)rc);
	MOV	EAX,[EBP-018H];	rc
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT7
	SUB	ESP,04H
	CALL	_printfieee

;***** 104    WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,0cH

;***** 105    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL14:

;***** 106    }
;***** 107 
;***** 108 pprfProfile = (PPRFPROFILE)Ptr;
	MOV	EAX,[EBP-01cH];	Ptr
	MOV	[EBP-020H],EAX;	pprfProfile

;***** 109 pprfProfile->pszUserName = (PSZ)&pprfProfile[1];
	MOV	EBX,[EBP-020H];	pprfProfile
	ADD	EBX,010H
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	[EAX+04H],EBX

;***** 110 pprfProfile->pszSysName = &pprfProfile->pszUserName[ININAME_LENGTH];
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	EBX,[EAX+04H]
	ADD	EBX,0c8H
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	[EAX+0cH],EBX

;***** 111 pprfProfile->cchUserName = ININAME_LENGTH;
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	DWORD PTR [EAX],0c8H

;***** 112 pprfProfile->cchSysName = ININAME_LENGTH;
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	DWORD PTR [EAX+08H],0c8H

;***** 113 
;***** 114 fSuccess = PrfQueryProfile(hab, pprfProfile);
	PUSH	DWORD PTR [EBP-020H];	pprfProfile
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,02H
	CALL	PrfQueryProfile
	ADD	ESP,08H
	MOV	[EBP-024H],EAX;	fSuccess

;***** 115 
;***** 116 if (fSuccess)
	CMP	DWORD PTR [EBP-024H],0H;	fSuccess
	JE	BLBL15

;***** 117    {
;***** 118    printf("Actual User-Profile: %s\n", pprfProfile->pszUserName);
	MOV	EAX,[EBP-020H];	pprfProfile
	PUSH	DWORD PTR [EAX+04H]
	MOV	EAX,OFFSET FLAT:@STAT8
	SUB	ESP,04H
	CALL	_printfieee

;***** 119    printf("Actual System-Profile: %s\n", pprfProfile->pszSysName);
	MOV	EAX,[EBP-020H];	pprfProfile
	PUSH	DWORD PTR [EAX+0cH]
	MOV	EAX,OFFSET FLAT:@STAT9
	SUB	ESP,04H
	CALL	_printfieee

;***** 120    strcpy(OldIniFile, pprfProfile->pszUserName);
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	EDX,[EAX+04H]
	LEA	EAX,[EBP-0150H];	OldIniFile
	CALL	strcpy
	ADD	ESP,010H
	JMP	BLBL16
	ALIGN 04H
BLBL15:

;***** 121    }
;***** 122 else
;***** 123    {
;***** 124    printf("Error in PrfQueryProfile\n");
	MOV	EAX,OFFSET FLAT:@STATa
	CALL	_printfieee

;***** 125    DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 126    WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,08H

;***** 127    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL16:

;***** 128    }
;***** 129 
;***** 130 if (!NewIniName)
	CMP	DWORD PTR [EBP-04H],0H;	NewIniName
	JNE	BLBL17

;***** 131    {
;***** 132    DTDir = QueryDesktopDirectory(HINI_USERPROFILE, &CheckRC);
	LEA	EDX,[EBP-0156H];	CheckRC
	OR	EAX,0ffffffffH
	CALL	QueryDesktopDirectory
	MOV	[EBP-0154H],EAX;	DTDir

;***** 133    if (DTDir)
	CMP	DWORD PTR [EBP-0154H],0H;	DTDir
	JE	BLBL18

;***** 134       {
;***** 135       printf("The actual desktop directory is %s\n", DTDir);
	PUSH	DWORD PTR [EBP-0154H];	DTDir
	MOV	EAX,OFFSET FLAT:@STATb
	SUB	ESP,04H
	CALL	_printfieee

;***** 136       GetEALongname(DTDir, &Longname);
	LEA	EDX,[EBP-015cH];	Longname
	MOV	EAX,[EBP-0154H];	DTDir
	CALL	GetEALongname
	ADD	ESP,08H

;***** 137       if (Longname)
	CMP	DWORD PTR [EBP-015cH],0H;	Longname
	JE	BLBL19

;***** 138          {
;***** 139          printf("The desktop name is %s\n", Longname);
	PUSH	DWORD PTR [EBP-015cH];	Longname
	MOV	EAX,OFFSET FLAT:@STATc
	SUB	ESP,04H
	CALL	_printfieee

;***** 140          free(Longname);
	MOV	EAX,[EBP-015cH];	Longname
	CALL	free
	ADD	ESP,08H
	JMP	BLBL20
	ALIGN 04H
BLBL19:

;***** 141          }
;***** 142       else
;***** 143          printf("No longname found for the desktop.\n");
	MOV	EAX,OFFSET FLAT:@STATd
	CALL	_printfieee
BLBL20:

;***** 144       free(DTDir);
	MOV	EAX,[EBP-0154H];	DTDir
	CALL	free
	JMP	BLBL21
	ALIGN 04H
BLBL18:

;***** 145       }
;***** 146    else
;***** 147       {
;***** 148       printf("Desktop directory cannot be queried, ");
	MOV	EAX,OFFSET FLAT:@STATe
	CALL	_printfieee

;***** 149       switch (CheckRC)
	XOR	EAX,EAX
	MOV	AX,[EBP-0156H];	CheckRC
	JMP	BLBL52
	ALIGN 04H
BLBL53:

;***** 150          {
;***** 151          case 1:
;***** 152               printf("location for WP_DESKTOP not\nfound in INI file.\n");
	MOV	EAX,OFFSET FLAT:@STATf
	CALL	_printfieee

;***** 153               break;
	JMP	BLBL51
	ALIGN 04H
BLBL54:

;***** 154          case 2:
;***** 155               printf("unable to allocate shared memory.\n");
	MOV	EAX,OFFSET FLAT:@STAT10
	CALL	_printfieee

;***** 156               break;
	JMP	BLBL51
	ALIGN 04H
BLBL55:

;***** 157          case 3:
;***** 158               printf("object MyGEQueryObject in class\nGEQueryObject couldn't"
	MOV	EAX,OFFSET FLAT:@STAT11
	CALL	_printfieee

;***** 159                      " be created. Maybe QDESK.DLL is not installed\nor "
;***** 160                      "registered in the system.\n");
;***** 161               break;
	JMP	BLBL51
	ALIGN 04H
BLBL56:

;***** 162          case 4:
;***** 163               printf("error in WinSetObjectData.\n");
	MOV	EAX,OFFSET FLAT:@STAT12
	CALL	_printfieee

;***** 164               break;
	JMP	BLBL51
	ALIGN 04H
	JMP	BLBL51
	ALIGN 04H
BLBL52:
	CMP	EAX,01H
	JE	BLBL53
	CMP	EAX,02H
	JE	BLBL54
	CMP	EAX,03H
	JE	BLBL55
	CMP	EAX,04H
	JE	BLBL56
BLBL51:
BLBL21:

;***** 165          } /* endswitch */
;***** 166       }
;***** 167    printf("No name for new user ini defined\n");
	MOV	EAX,OFFSET FLAT:@STAT13
	CALL	_printfieee

;***** 168    DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 169    WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,08H

;***** 170    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL17:

;***** 171    }
;***** 172 
;***** 173 IniName = malloc(2048);
	MOV	EAX,0800H
	CALL	malloc
	MOV	[EBP-0160H],EAX;	IniName

;***** 174 
;***** 175 if (!IniName)
	CMP	DWORD PTR [EBP-0160H],0H;	IniName
	JNE	BLBL22

;***** 176    {
;***** 177    printf("Allocation error for INI file name.\n");
	MOV	EAX,OFFSET FLAT:@STAT14
	CALL	_printfieee

;***** 178    DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 179    WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,08H

;***** 180    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL22:

;***** 181    }
;***** 182 
;***** 183 rc = DosQueryPathInfo(NewIniName, FIL_QUERYFULLNAME, IniName, 2048);
	PUSH	0800H
	PUSH	DWORD PTR [EBP-0160H];	IniName
	PUSH	05H
	PUSH	DWORD PTR [EBP-04H];	NewIniName
	MOV	AL,04H
	CALL	DosQueryPathInfo
	ADD	ESP,010H
	MOV	[EBP-018H],EAX;	rc

;***** 184 
;***** 185 if (rc)
	CMP	DWORD PTR [EBP-018H],0H;	rc
	JE	BLBL23

;***** 186    {
;***** 187    printf("Error %ld in DosQueryPathInfo for %s.\n", (ULONG)rc, NewIniName);
	PUSH	DWORD PTR [EBP-04H];	NewIniName
	PUSH	DWORD PTR [EBP-018H];	rc
	MOV	EAX,OFFSET FLAT:@STAT15
	SUB	ESP,04H
	CALL	_printfieee

;***** 188    free(IniName);
	MOV	EAX,[EBP-0160H];	IniName
	CALL	free

;***** 189    DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 190    WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,014H

;***** 191    return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL23:

;***** 192    }
;***** 193 
;***** 194 if (Flag & CHANGE_ASSOC_DIR)
	TEST	BYTE PTR [EBP-08H],04H;	Flag
	JE	BLBL24

;***** 195    {
;***** 196    CheckRC = SetDesktopDirectory(IniName, hab, DirName);
	MOV	ECX,[EBP-0cH];	DirName
	MOV	EDX,[EBP-014H];	hab
	MOV	EAX,[EBP-0160H];	IniName
	CALL	SetDesktopDirectory
	MOV	[EBP-0156H],AX;	CheckRC

;***** 197    printf("Set desktop directory, rc = %u\n", CheckRC);
	XOR	EAX,EAX
	MOV	AX,[EBP-0156H];	CheckRC
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT16
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
	JMP	BLBL25
	ALIGN 04H
BLBL24:

;***** 198    }
;***** 199 else
;***** 200    CheckRC = CheckIniFile(IniName, hab, &DirName);
	LEA	ECX,[EBP-0cH];	DirName
	MOV	EDX,[EBP-014H];	hab
	MOV	EAX,[EBP-0160H];	IniName
	CALL	CheckIniFile
	MOV	[EBP-0156H],AX;	CheckRC
BLBL25:

;***** 201 
;***** 202 switch (CheckRC)
	XOR	EAX,EAX
	MOV	AX,[EBP-0156H];	CheckRC
	JMP	BLBL58
	ALIGN 04H
BLBL59:

;***** 203    {
;***** 204    case 2:
;***** 205         printf("Ini file %s not found.\n", IniName);
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT17
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 206         break;
	JMP	BLBL57
	ALIGN 04H
BLBL60:

;***** 207    case 3:
;***** 208         printf("The file %s is no valid user ini file or is\ncurrently in use"
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT18
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 209                " by the system.\n", IniName);
;***** 210         break;
	JMP	BLBL57
	ALIGN 04H
BLBL61:

;***** 211    case 4:
;***** 212         printf("Memory allocation error\n");
	MOV	EAX,OFFSET FLAT:@STAT19
	CALL	_printfieee

;***** 213         break;
	JMP	BLBL57
	ALIGN 04H
BLBL62:

;***** 214    case 5:
;***** 215         printf("Handle of desktop object not found in INI file %s.\n",
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT1a
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 216                IniName);
;***** 217         break;
	JMP	BLBL57
	ALIGN 04H
BLBL63:

;***** 218    case 6:
;***** 219         printf("Unable to allocate shared memory.\n");
	MOV	EAX,OFFSET FLAT:@STAT1b
	CALL	_printfieee

;***** 220         break;
	JMP	BLBL57
	ALIGN 04H
BLBL64:

;***** 221    case 7:
;***** 222         printf("Object MyGEQueryObject in class GEQueryObject couldn't"
	MOV	EAX,OFFSET FLAT:@STAT1c
	CALL	_printfieee

;***** 223                " be created.\nMaybe QDESK.DLL is not installed or "
;***** 224                "registered in the system.\n");
;***** 225         break;
	JMP	BLBL57
	ALIGN 04H
BLBL65:

;***** 226    case 8:
;***** 227         printf("The directory structure associated to inifile %s\n"
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT1d
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 228                "cannot be located.\n", IniName);
;***** 229         break;
	JMP	BLBL57
	ALIGN 04H
BLBL66:

;***** 230    case 9:
;***** 231         printf("The desktop directory %s couldn't be located.\n", DirName);
	PUSH	DWORD PTR [EBP-0cH];	DirName
	MOV	EAX,OFFSET FLAT:@STAT1e
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 232         break;
	JMP	BLBL57
	ALIGN 04H
BLBL67:

;***** 233    case 10:
;***** 234         printf("Cannot update INI file %s.\n", IniName);
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT1f
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 235         break;
	JMP	BLBL57
	ALIGN 04H
BLBL68:

;***** 236    default:
;***** 237         if (Flag & CHANGE_ASSOC_DIR)
	TEST	BYTE PTR [EBP-08H],04H;	Flag
	JE	BLBL26

;***** 238            printf("INI file %s successfully updated with new directory "
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT20
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
BLBL26:

;***** 239                         "structure\n", IniName);
;***** 240         rc = DosAllocMem(&PtrNew, ALLOC_SIZE,
	PUSH	013H
	PUSH	01000H
	LEA	EAX,[EBP-0164H];	PtrNew
	PUSH	EAX
	MOV	AL,03H
	CALL	DosAllocMem
	ADD	ESP,0cH
	MOV	[EBP-018H],EAX;	rc

;***** 241                          PAG_COMMIT | PAG_READ | PAG_WRITE);
;***** 242 
;***** 243         if (rc)
	CMP	DWORD PTR [EBP-018H],0H;	rc
	JE	BLBL27

;***** 244            {
;***** 245            printf("Allocate Error, rc = %ld\n", (LONG)rc);
	MOV	EAX,[EBP-018H];	rc
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT21
	SUB	ESP,04H
	CALL	_printfieee

;***** 246            free(IniName);
	MOV	EAX,[EBP-0160H];	IniName
	CALL	free

;***** 247            DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 248            WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,010H

;***** 249            return(1);
	MOV	EAX,01H
	JMP	BLBL12
	ALIGN 04H
BLBL27:

;***** 250            }
;***** 251 
;***** 252         pprfProfileNew = (PPRFPROFILE)PtrNew;
	MOV	EAX,[EBP-0164H];	PtrNew
	MOV	[EBP-0168H],EAX;	pprfProfileNew

;***** 253         pprfProfileNew->pszUserName = (PSZ)&pprfProfile[1];
	MOV	EBX,[EBP-020H];	pprfProfile
	ADD	EBX,010H
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	[EAX+04H],EBX

;***** 254         strcpy(pprfProfileNew->pszUserName, IniName);
	MOV	EDX,[EBP-0160H];	IniName
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EAX,[EAX+04H]
	CALL	strcpy

;***** 255         strupr(pprfProfileNew->pszUserName);
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EAX,[EAX+04H]
	CALL	strupr

;***** 256         pprfProfileNew->cchUserName = strlen(IniName) + 1;
	MOV	EAX,[EBP-0160H];	IniName
	CALL	strlen
	MOV	EBX,EAX
	INC	EBX
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	[EAX],EBX

;***** 257         pprfProfileNew->pszSysName =
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EBX,[EAX+04H]
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EAX,[EAX]
	ADD	EBX,EAX
	INC	EBX
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	[EAX+0cH],EBX

;***** 258                           &pprfProfileNew->pszUserName[pprfProfileNew->cchUserName + 1];
;***** 259         strcpy(pprfProfileNew->pszSysName, pprfProfile->pszSysName);
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	EDX,[EAX+0cH]
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EAX,[EAX+0cH]
	CALL	strcpy

;***** 260         pprfProfileNew->cchSysName = pprfProfile->cchSysName;
	MOV	EAX,[EBP-020H];	pprfProfile
	MOV	EBX,[EAX+08H]
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	[EAX+08H],EBX

;***** 261 
;***** 262         DataLen = sizeof(hObjectDesktop);
	MOV	DWORD PTR [EBP-016cH],04H;	DataLen

;***** 263         fSuccess = PrfQueryProfileData(HINI_USERPROFILE,
	LEA	EAX,[EBP-016cH];	DataLen
	PUSH	EAX
	LEA	EAX,[EBP-0170H];	hObjectDesktop
	PUSH	EAX
	PUSH	OFFSET FLAT:@32Keyname
	PUSH	OFFSET FLAT:@30Application
	PUSH	0ffffffffH
	MOV	AL,05H
	CALL	PrfQueryProfileData
	ADD	ESP,014H
	MOV	[EBP-024H],EAX;	fSuccess

;***** 264                                        (PSZ)Application,
;***** 265                                        (PSZ)Keyname,
;***** 266                                        (PSZ)&hObjectDesktop,
;***** 267                                        &DataLen);
;***** 268         if ((!fSuccess) || (DataLen < sizeof(hObjectDesktop)))
	CMP	DWORD PTR [EBP-024H],0H;	fSuccess
	JE	BLBL28
	CMP	DWORD PTR [EBP-016cH],04H;	DataLen
	JB	BLBL28
	JMP	BLBL29
	ALIGN 04H
BLBL28:

;***** 269            hObjectDesktop = (HOBJECT)0;
	MOV	DWORD PTR [EBP-0170H],0H;	hObjectDesktop
BLBL29:

;***** 270 
;***** 271         fSuccess = PrfReset(hab, pprfProfileNew);
	PUSH	DWORD PTR [EBP-0168H];	pprfProfileNew
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,02H
	CALL	PrfReset
	ADD	ESP,08H
	MOV	[EBP-024H],EAX;	fSuccess

;***** 272 
;***** 273         if (fSuccess)
	CMP	DWORD PTR [EBP-024H],0H;	fSuccess
	JE	BLBL30

;***** 274            {
;***** 275            printf("New User-Profile: %s\nNew System-Profile: %s\n",
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	PUSH	DWORD PTR [EAX+0cH]
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	PUSH	DWORD PTR [EAX+04H]
	MOV	EAX,OFFSET FLAT:@STAT22
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,0cH

;***** 276                   pprfProfileNew->pszUserName, pprfProfileNew->pszSysName);
;***** 277 
;***** 278            if (!CheckRC)
	CMP	WORD PTR [EBP-0156H],0H;	CheckRC
	JNE	BLBL31

;***** 279               {
;***** 280               if (DirName)
	CMP	DWORD PTR [EBP-0cH],0H;	DirName
	JE	BLBL32

;***** 281                  {
;***** 282                  printf("Desktop directory used by %s is %s\n",
	PUSH	DWORD PTR [EBP-0cH];	DirName
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	PUSH	DWORD PTR [EAX+04H]
	MOV	EAX,OFFSET FLAT:@STAT23
	SUB	ESP,04H
	CALL	_printfieee

;***** 283                         pprfProfileNew->pszUserName, DirName);
;***** 284                  GetEALongname(DirName, &Longname);
	LEA	EDX,[EBP-015cH];	Longname
	MOV	EAX,[EBP-0cH];	DirName
	CALL	GetEALongname
	ADD	ESP,0cH

;***** 285                  if (Longname)
	CMP	DWORD PTR [EBP-015cH],0H;	Longname
	JE	BLBL33

;***** 286                     {
;***** 287            
;***** 287          printf("The desktop name is %s\n", Longname);
	PUSH	DWORD PTR [EBP-015cH];	Longname
	MOV	EAX,OFFSET FLAT:@STAT24
	SUB	ESP,04H
	CALL	_printfieee

;***** 288                     free(Longname);
	MOV	EAX,[EBP-015cH];	Longname
	CALL	free
	ADD	ESP,08H
	JMP	BLBL34
	ALIGN 04H
BLBL33:

;***** 289                     }
;***** 290                  else
;***** 291                     printf("No longname found for the desktop.\n");
	MOV	EAX,OFFSET FLAT:@STAT25
	CALL	_printfieee
BLBL34:
	JMP	BLBL35
	ALIGN 04H
BLBL32:

;***** 292                  }
;***** 293               else
;***** 294                  {
;***** 295                  DTDir = QueryDesktopDirectory(HINI_USERPROFILE, &CheckRC);
	LEA	EDX,[EBP-0156H];	CheckRC
	OR	EAX,0ffffffffH
	CALL	QueryDesktopDirectory
	MOV	[EBP-0154H],EAX;	DTDir

;***** 296                  if (DTDir)
	CMP	DWORD PTR [EBP-0154H],0H;	DTDir
	JE	BLBL36

;***** 297                     {
;***** 298                     printf("Desktop directory used by %s is %s\n",
	PUSH	DWORD PTR [EBP-0154H];	DTDir
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	PUSH	DWORD PTR [EAX+04H]
	MOV	EAX,OFFSET FLAT:@STAT26
	SUB	ESP,04H
	CALL	_printfieee

;***** 299                            pprfProfileNew->pszUserName, DTDir);
;***** 300                     GetEALongname(DTDir, &Longname);
	LEA	EDX,[EBP-015cH];	Longname
	MOV	EAX,[EBP-0154H];	DTDir
	CALL	GetEALongname
	ADD	ESP,0cH

;***** 301                     if (Longname)
	CMP	DWORD PTR [EBP-015cH],0H;	Longname
	JE	BLBL37

;***** 302                        {
;***** 303                        printf("The desktop name is %s\n", Longname);
	PUSH	DWORD PTR [EBP-015cH];	Longname
	MOV	EAX,OFFSET FLAT:@STAT27
	SUB	ESP,04H
	CALL	_printfieee

;***** 304                        free(Longname);
	MOV	EAX,[EBP-015cH];	Longname
	CALL	free
	ADD	ESP,08H
	JMP	BLBL38
	ALIGN 04H
BLBL37:

;***** 305                        }
;***** 306                     else
;***** 307                        printf("No longname found for the desktop.\n");
	MOV	EAX,OFFSET FLAT:@STAT28
	CALL	_printfieee
BLBL38:

;***** 308                     free(DTDir);
	MOV	EAX,[EBP-0154H];	DTDir
	CALL	free
	JMP	BLBL39
	ALIGN 04H
BLBL36:

;***** 309                     }
;***** 310                  else
;***** 311                     {
;***** 312                     printf("Desktop directory cannot be queried, maybe "
	MOV	EAX,OFFSET FLAT:@STAT29
	CALL	_printfieee
BLBL39:
BLBL35:
BLBL31:

;***** 313                            "QDESK.DLL is not installed.\n");
;***** 314                     }
;***** 315                  }
;***** 316               }
;***** 317            if (Flag & CHANGE_CONFIG_SYS)
	TEST	BYTE PTR [EBP-08H],02H;	Flag
	JE	BLBL40

;***** 318               {
;***** 319               ChangeConfigSys(pprfProfileNew->pszSysName,
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EDX,[EAX+04H]
	MOV	EAX,[EBP-0168H];	pprfProfileNew
	MOV	EAX,[EAX+0cH]
	CALL	ChangeConfigSys
BLBL40:

;***** 320                               pprfProfileNew->pszUserName);
;***** 321               }
;***** 322            if (hObjectDesktop)
	CMP	DWORD PTR [EBP-0170H],0H;	hObjectDesktop
	JE	BLBL41

;***** 323               {
;***** 324               hini = (HINI)NULL;
	MOV	DWORD PTR [EBP-0174H],0H;	hini

;***** 325               i1 = 0;
	MOV	DWORD PTR [EBP-010H],0H;	i1

;***** 326               while (!hini)
	ALIGN 04H
BLBL42:
	CMP	DWORD PTR [EBP-0174H],0H;	hini
	JNE	BLBL43

;***** 327                  {
;***** 328                  hini = PrfOpenProfile(hab, OldIniFile);
	LEA	EAX,[EBP-0150H];	OldIniFile
	PUSH	EAX
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,02H
	CALL	PrfOpenProfile
	ADD	ESP,08H
	MOV	[EBP-0174H],EAX;	hini

;***** 329                  if (!hini)
	CMP	DWORD PTR [EBP-0174H],0H;	hini
	JNE	BLBL44

;***** 330                     {
;***** 331                     i1++;
	MOV	EAX,[EBP-010H];	i1
	INC	EAX
	MOV	[EBP-010H],EAX;	i1

;***** 332                     if (i1 > 60)
	CMP	DWORD PTR [EBP-010H],03cH;	i1
	JLE	BLBL45

;***** 333                        {
;***** 334                        break;
	JMP	BLBL43
	ALIGN 04H
BLBL45:

;***** 335                        }
;***** 336                     DosSleep(1500L);  /* Wait 1.5 seconds, then try again */
	PUSH	05dcH
	MOV	AL,01H
	CALL	DosSleep
	ADD	ESP,04H
BLBL44:
	JMP	BLBL42
	ALIGN 04H
BLBL43:

;***** 337                     }
;***** 338                  }
;***** 339               if (hini)
	CMP	DWORD PTR [EBP-0174H],0H;	hini
	JE	BLBL46

;***** 340                  {
;***** 341                  DataLen = sizeof(hObjectDesktop2);
	MOV	DWORD PTR [EBP-016cH],04H;	DataLen

;***** 342                  fSuccess = PrfQueryProfileData(hini,
	LEA	EAX,[EBP-016cH];	DataLen
	PUSH	EAX
	LEA	EAX,[EBP-0178H];	hObjectDesktop2
	PUSH	EAX
	PUSH	OFFSET FLAT:@32Keyname
	PUSH	OFFSET FLAT:@30Application
	PUSH	DWORD PTR [EBP-0174H];	hini
	MOV	AL,05H
	CALL	PrfQueryProfileData
	ADD	ESP,014H
	MOV	[EBP-024H],EAX;	fSuccess

;***** 343                                                 (PSZ)Application,
;***** 344                                                 (PSZ)Keyname,
;***** 345                                                 (PSZ)&hObjectDesktop2,
;***** 346                                                 &DataLen);
;***** 347                  if ((!fSuccess) || (DataLen < sizeof(hObjectDesktop2)))
	CMP	DWORD PTR [EBP-024H],0H;	fSuccess
	JE	BLBL47
	CMP	DWORD PTR [EBP-016cH],04H;	DataLen
	JB	BLBL47
	JMP	BLBL48
	ALIGN 04H
BLBL47:

;***** 348                     {
;***** 349                     PrfWriteProfileData(hini,
	PUSH	04H
	LEA	EAX,[EBP-0170H];	hObjectDesktop
	PUSH	EAX
	PUSH	OFFSET FLAT:@32Keyname
	PUSH	OFFSET FLAT:@30Application
	PUSH	DWORD PTR [EBP-0174H];	hini
	MOV	AL,05H
	CALL	PrfWriteProfileData

;***** 350                                         (PSZ)Application,
;***** 351                                         (PSZ)Keyname,
;***** 352                                         (PVOID)&hObjectDesktop,
;***** 353                                         sizeof(HOBJECT));
;***** 354                     printf("Desktop key in %s updated.\n", OldIniFile);
	LEA	EAX,[EBP-0150H];	OldIniFile
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT2a
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,01cH
BLBL48:

;***** 355                     }
;***** 356                  PrfCloseProfile(hini);
	PUSH	DWORD PTR [EBP-0174H];	hini
	MOV	AL,01H
	CALL	PrfCloseProfile
	ADD	ESP,04H
BLBL46:
BLBL41:
	JMP	BLBL49
	ALIGN 04H
BLBL30:

;***** 357                  }
;***** 358               }
;***** 359            }
;***** 360         else
;***** 361            printf("Error while switching to %s.\n", IniName);
	PUSH	DWORD PTR [EBP-0160H];	IniName
	MOV	EAX,OFFSET FLAT:@STAT2b
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
BLBL49:

;***** 362         DosFreeMem(PtrNew);
	PUSH	DWORD PTR [EBP-0164H];	PtrNew
	MOV	AL,01H
	CALL	DosFreeMem
	ADD	ESP,04H
	JMP	BLBL57
	ALIGN 04H
BLBL58:
	CMP	EAX,02H
	JE	BLBL59
	CMP	EAX,03H
	JE	BLBL60
	CMP	EAX,04H
	JE	BLBL61
	CMP	EAX,05H
	JE	BLBL62
	CMP	EAX,06H
	JE	BLBL63
	CMP	EAX,07H
	JE	BLBL64
	CMP	EAX,08H
	JE	BLBL65
	CMP	EAX,09H
	JE	BLBL66
	CMP	EAX,0aH
	JE	BLBL67
	JMP	BLBL68
	ALIGN 04H
BLBL57:

;***** 363    } /* endswitch */
;***** 364 
;***** 365 if (DirName && (!(Flag & CHANGE_ASSOC_DIR)))
	CMP	DWORD PTR [EBP-0cH],0H;	DirName
	JE	BLBL50
	MOV	EAX,[EBP-08H];	Flag
	AND	EAX,04H
	OR	EAX,EAX
	JNE	BLBL50

;***** 366    free(DirName);
	MOV	EAX,[EBP-0cH];	DirName
	CALL	free
BLBL50:

;***** 367 
;***** 368 free(IniName);
	MOV	EAX,[EBP-0160H];	IniName
	CALL	free

;***** 369 DosFreeMem(Ptr);
	PUSH	DWORD PTR [EBP-01cH];	Ptr
	MOV	AL,01H
	CALL	DosFreeMem

;***** 370 
;***** 371 WinTerminate(hab);
	PUSH	DWORD PTR [EBP-014H];	hab
	MOV	AL,01H
	CALL	WinTerminate
	ADD	ESP,08H

;***** 372 
;***** 373 return(0);
	XOR	EAX,EAX
	JMP	BLBL12
	ALIGN 04H

;***** 374 }
	XOR	EAX,EAX
	JMP	BLBL12
	ALIGN 04H
BLBL12:
	ADD	ESP,0cH
	POP	EBX
	LEAVE	
	RET	
main	ENDP

;***** 376          void       Syntax(void)
	ALIGN 04H

	PUBLIC Syntax
Syntax	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,04H

;***** 378 printf("\nCHGDT - Change actual Desktop\n"
	MOV	EAX,OFFSET FLAT:@STAT2c
	CALL	_printfieee

;***** 379        "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n"
;***** 380        "Syntax:\n\n"
;***** 381        "     CHGDT ÄÄÄÂÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÂÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÂÄÄÄ´\n"
;***** 382        "              ³ ÀÄ new ini name ÄÙ  ÀÄ /C ÄÙ  ÀÄ /D:dir name ÄÙ ³\n"
;***** 383        "              ³                                                 ³\n"
;***** 384        "              ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ /? ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n"
;***** 385        "              ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ? ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n"
;***** 386        "     new ini name     Switch to that ini file\n"
;***** 387        "     /C               Update CONFIG.SYS\n"
;***** 388        "     /? or ?          Display the syntax\n"
;***** 389        "     dir name         Name of associated directory structure to use\n"
;***** 390        "                      with the new ini file\n\n"
;***** 391        "     CHGDT without parameter will display the actual INI files and\n"
;***** 392        "     desktop directory.\n");
;***** 393 
;***** 394 }
BLBL69:
	ADD	ESP,04H
	LEAVE	
	RET	
Syntax	ENDP

;***** 396          USHORT     ChangeConfigSys(PSZ SysIniName, PSZ UserIniName)
	ALIGN 04H

	PUBLIC ChangeConfigSys
ChangeConfigSys	PROC
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,023cH
	MOV	[EBP+08H],EAX;	SysIniName
	MOV	[EBP+0cH],EDX;	UserIniName

;***** 407          BOOL       Modified = FALSE;
	MOV	DWORD PTR [EBP-04H],0H;	Modified

;***** 408 static   CHAR       EnvVarName[] = "TMP";
;***** 409 
;***** 410 /* Delete environment variable TMP if exists */
;***** 411 EnvTmp = getenv(EnvVarName);
	MOV	EAX,OFFSET FLAT:@6cEnvVarName
	CALL	getenv
	MOV	[EBP-08H],EAX;	EnvTmp

;***** 412 if (EnvTmp)
	CMP	DWORD PTR [EBP-08H],0H;	EnvTmp
	JE	BLBL70

;***** 413    putenv(EnvVarName);
	MOV	EAX,OFFSET FLAT:@6cEnvVarName
	CALL	_putenv
BLBL70:

;***** 414 
;***** 415 sprintf(NewPath, "%c:\\", SysIniName[0]);
	MOV	ECX,[EBP+08H];	SysIniName
	XOR	EAX,EAX
	MOV	AL,[ECX]
	PUSH	EAX
	MOV	EDX,OFFSET FLAT:@STAT2d
	LEA	EAX,[EBP-014H];	NewPath
	SUB	ESP,08H
	CALL	_sprintfieee

;***** 416 NewName = tempnam(NewPath, EnvVarName);
	MOV	EDX,OFFSET FLAT:@6cEnvVarName
	LEA	EAX,[EBP-014H];	NewPath
	CALL	_tempnam
	MOV	[EBP-018H],EAX;	NewName

;***** 417 
;***** 418 sprintf(ConfigSys, "%c:\\CONFIG.SYS", SysIniName[0]);
	MOV	ECX,[EBP+08H];	SysIniName
	XOR	EAX,EAX
	MOV	AL,[ECX]
	PUSH	EAX
	MOV	EDX,OFFSET FLAT:@STAT2e
	LEA	EAX,[EBP-02cH];	ConfigSys
	SUB	ESP,08H
	CALL	_sprintfieee

;***** 419 
;***** 420 Infile = fopen(ConfigSys, "r");
	MOV	EDX,OFFSET FLAT:@STAT2f
	LEA	EAX,[EBP-02cH];	ConfigSys
	CALL	fopen
	ADD	ESP,018H
	MOV	[EBP-030H],EAX;	Infile

;***** 421 if (!Infile)
	CMP	DWORD PTR [EBP-030H],0H;	Infile
	JNE	BLBL71

;***** 422    {
;***** 423    printf("Cannot open %s\n", ConfigSys);
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT30
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 424    return(1);
	MOV	AX,01H
	JMP	BLBL72
	ALIGN 04H
BLBL71:

;***** 425    }
;***** 426 
;***** 427 Outfile = fopen(NewName, "w");
	MOV	EDX,OFFSET FLAT:@STAT31
	MOV	EAX,[EBP-018H];	NewName
	CALL	fopen
	MOV	[EBP-034H],EAX;	Outfile

;***** 428 if (!Outfile)
	CMP	DWORD PTR [EBP-034H],0H;	Outfile
	JNE	BLBL73

;***** 429    {
;***** 430    fclose(Infile);
	MOV	EAX,[EBP-030H];	Infile
	CALL	fclose

;***** 431    printf("Cannot create tempfile %s\n", NewName);
	PUSH	DWORD PTR [EBP-018H];	NewName
	MOV	EAX,OFFSET FLAT:@STAT32
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H

;***** 432    return(2);
	MOV	AX,02H
	JMP	BLBL72
	ALIGN 04H
BLBL73:

;***** 433    }
;***** 434 
;***** 435 while (fgets(Buffer, sizeof(Buffer), Infile))
	ALIGN 04H
BLBL74:
	MOV	ECX,[EBP-030H];	Infile
	MOV	EDX,01f4H
	LEA	EAX,[EBP-0228H];	Buffer
	CALL	fgets
	OR	EAX,EAX
	JE	BLBL75

;***** 436    {
;***** 437    StartPos = strspn(Buffer, " ");
	MOV	EDX,OFFSET FLAT:@STAT33
	LEA	EAX,[EBP-0228H];	Buffer
	CALL	strspn
	MOV	[EBP-022cH],EAX;	StartPos

;***** 438    if (!memicmp(&Buffer[StartPos], "SET ", 4))
	MOV	ECX,04H
	MOV	EDX,OFFSET FLAT:@STAT34
	MOV	EAX,[EBP-022cH];	StartPos
	LEA	EAX,DWORD PTR [EBP+EAX-0228H]
	CALL	memicmp
	OR	EAX,EAX
	JNE	BLBL76

;***** 439       {
;***** 440       UserIniPos = strspn(&Buffer[StartPos + 4], " ") + StartPos + 4;
	MOV	EDX,OFFSET FLAT:@STAT35
	MOV	EAX,[EBP-022cH];	StartPos
	LEA	EAX,DWORD PTR [EBP+EAX-0228H]
	ADD	EAX,04H
	CALL	strspn
	MOV	ECX,EAX
	MOV	EAX,[EBP-022cH];	StartPos
	ADD	EAX,ECX
	ADD	EAX,04H
	MOV	[EBP-0230H],EAX;	UserIniPos

;***** 441       if (!memicmp(&Buffer[UserIniPos], "USER_INI=", 9))
	MOV	ECX,09H
	MOV	EDX,OFFSET FLAT:@STAT36
	MOV	EAX,[EBP-0230H];	UserIniPos
	LEA	EAX,DWORD PTR [EBP+EAX-0228H]
	CALL	memicmp
	OR	EAX,EAX
	JNE	BLBL77

;***** 442          {
;***** 443          sprintf(&Buffer[UserIniPos + 9], "%s\n", UserIniName);
	PUSH	DWORD PTR [EBP+0cH];	UserIniName
	MOV	EDX,OFFSET FLAT:@STAT37
	MOV	EAX,[EBP-0230H];	UserIniPos
	LEA	EAX,DWORD PTR [EBP+EAX-0228H]
	ADD	EAX,09H
	SUB	ESP,08H
	CALL	_sprintfieee
	ADD	ESP,0cH

;***** 444          Modified = TRUE;
	MOV	DWORD PTR [EBP-04H],01H;	Modified
BLBL77:
BLBL76:

;***** 445          }
;***** 446       }
;***** 447    fputs(Buffer, Outfile);
	MOV	EDX,[EBP-034H];	Outfile
	LEA	EAX,[EBP-0228H];	Buffer
	CALL	fputs
	JMP	BLBL74
	ALIGN 04H
BLBL75:

;***** 448    }
;***** 449 
;***** 450 fclose(Infile);
	MOV	EAX,[EBP-030H];	Infile
	CALL	fclose

;***** 451 fclose(Outfile);
	MOV	EAX,[EBP-034H];	Outfile
	CALL	fclose

;***** 452 
;***** 453 if (Modified)
	CMP	DWORD PTR [EBP-04H],0H;	Modified
	JE	BLBL78

;***** 454    {
;***** 455    if (!DosDelete(ConfigSys))
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	MOV	AL,01H
	CALL	DosDelete
	ADD	ESP,04H
	OR	EAX,EAX
	JNE	BLBL79

;***** 456       {
;***** 457       if (!DosMove(NewName, ConfigSys))
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	PUSH	DWORD PTR [EBP-018H];	NewName
	MOV	AL,02H
	CALL	DosMove
	ADD	ESP,08H
	OR	EAX,EAX
	JNE	BLBL80

;***** 458          printf("%s successfully updated.\n", ConfigSys);
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT38
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
	JMP	BLBL81
	ALIGN 04H
BLBL80:

;***** 459       else
;***** 460          printf("Cannot rename tempfile %s to %s.\n", NewName, ConfigSys);
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	PUSH	DWORD PTR [EBP-018H];	NewName
	MOV	EAX,OFFSET FLAT:@STAT39
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,0cH
BLBL81:
	JMP	BLBL82
	ALIGN 04H
BLBL79:

;***** 461       }
;***** 462    else
;***** 463       printf("Unable to replace %s.\n", ConfigSys);
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT3a
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,08H
BLBL82:
	JMP	BLBL83
	ALIGN 04H
BLBL78:

;***** 464    }
;***** 465 else
;***** 466    {
;***** 467    DosDelete(NewName);
	PUSH	DWORD PTR [EBP-018H];	NewName
	MOV	AL,01H
	CALL	DosDelete

;***** 468    printf("Nothing found to update %s.\n", ConfigSys);
	LEA	EAX,[EBP-02cH];	ConfigSys
	PUSH	EAX
	MOV	EAX,OFFSET FLAT:@STAT3b
	SUB	ESP,04H
	CALL	_printfieee
	ADD	ESP,0cH
BLBL83:

;***** 469    }
;***** 470 
;***** 471 free(NewName);
	MOV	EAX,[EBP-018H];	NewName
	CALL	free

;***** 472 
;***** 473 return(0);
	XOR	EAX,EAX
	JMP	BLBL72
	ALIGN 04H

;***** 474 }
BLBL72:
	ADD	ESP,0cH
	LEAVE	
	RET	
ChangeConfigSys	ENDP
CODE32	ENDS
END
