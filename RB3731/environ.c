/*******************************************************\
 *  Program name: ENVIRON.C                            *
 *  Created     : 05/05/90                             *
 *  Revised     : 12/13/91                             *
 *  Author      : Bernd Westphal                       *
 *  Purpose     : Get DOS environment size for VDM lab *
 *  Compile     : cl environ.c                         *
 *  or          : icc /O+ environ.c                    *
 *  Input param : none                                 *
\*******************************************************/

#include <stdio.h>
#include <string.h>

void main( int argc, char *argv[], char *envp[] )
{
   int   charcount = 0;                    /* # of char  */

   printf("Current environment settings:\n\n");
   printf("-----------------------------\n");
   while (*envp)
   {
      printf("%s\n", *envp);
      charcount += strlen (*envp) + 1;     /* add 1 for the string terminator */
      *envp++;
   }
   printf("-----------------------------\n");
   printf("\nTotal environment size is %d bytes.\n\n", charcount);
   /* printf("Press Enter to continue ...\n"); */
   getchar();
}

