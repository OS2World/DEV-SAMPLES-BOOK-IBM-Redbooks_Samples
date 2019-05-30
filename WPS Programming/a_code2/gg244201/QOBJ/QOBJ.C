#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INCL_WINWORKPLACE
#include <os2.h>

         int        main(int argc, char *argv[])
{
         HOBJECT    hObject;
         CHAR       Name[400];

if (argc < 2)
   {
   printf("No objectid defined.\n");
   return(1);
   }

if (argv[1][0] == '!')
   strcpy(Name, &argv[1][1]);
else
   sprintf(Name, "<%s>", argv[1]);

hObject = WinQueryObject(Name);

if (hObject)
   {
   printf("Object handle: %lX\n", (ULONG)hObject);
   return(0);
   }
else
   printf("Object %s doesn't exist or could't be awakened.\n", Name);

return(2);
}
