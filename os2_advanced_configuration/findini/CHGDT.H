#define ININAME_LENGTH   200
#define ALLOC_SIZE       4096

         PSZ        QueryDesktopDirectory(HINI hini, PUSHORT Retc);
         USHORT     CheckIniFile(PSZ FileName, HAB hab, PSZ *DirName);
         USHORT     SetDesktopDirectory(PSZ FileName, HAB hab, PSZ DirName);
