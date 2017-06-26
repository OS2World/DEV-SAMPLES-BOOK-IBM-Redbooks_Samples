'*********************************************************
'*  Program name: GRAPHIC.BAS                            *
'*  Created     : 05/14/90                               *
'*  Revised     :                                        *
'*  Author      : Bernd Westphal                         *
'*  Purpose     : Draw some graphics                     *
'*                for VDM clipboard lab session          *
'*  Compiler    : IBM BASIC Compiler/2 V1.00             *
'*  Compile     : BASCOM GRAPHIC /O;                     *
'*  Link        : LINK GRAPHIC;                          *
'*  Input param : none                                   *
'*********************************************************

         SCREEN 2                           ' select 640 x 200 graphics mode
         CLS                                ' clear the screen
         FOR X=1 TO 640 STEP 10
            LINE (320,199)-(X,0)            ' draw some lines
         NEXT

         FOR X=1 TO 640 STEP 10
            LINE (320,0)-(X,199)            ' draw some lines
         NEXT

         LOCATE 12,31                       ' position the cursor
         PRINT SPACE$(21)                   ' print 21 blanks
         LOCATE 13,31                       ' position the cursor
         PRINT " IBM ITSC Boca Raton "      ' print some text
         LOCATE 14,31                       ' position the cursor
         PRINT SPACE$(21)                   ' print 21 blanks
         kb$ = INPUT$(1)                    ' check for keystroke
         SYSTEM                             ' return to DOS
