 #include <dos.h>
 #include <stdio.h>

 union REGS  regs;     /* Register values           */

main(argc, argv, envp)
   int argc;
   char *argv[];
   char *envp[];
{
   printf("Prepare for an int86 INT 19H....\n");
   getchar ();
   int86 (0x19, &regs, &regs);
   printf("dosint INT 19H has been executed, result is %u\n",regs.x.cflag);
   getch ();
}
