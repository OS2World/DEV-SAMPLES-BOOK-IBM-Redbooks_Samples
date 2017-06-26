'*****************************************************************
'*  Program name: SOUND.BAS                                      *
'*  Created     : 05/14/90                                       *
'*  Revised     :                                                *
'*  Author      : Bernd Westphal                                 *
'*  Purpose     : Access the speaker system in a VDM environment *
'*                Only 1 VDM has access to the speaker           *
'*  Compiler    : IBM BASIC Compiler/2                           *
'*  Compile     : BASCOM SOUND /O/V;                             *
'*  Link        : LINK SOUND;                                    *
'*  Input param : none                                           *
'*****************************************************************

         CLS                           ' clear the screen
         PLAY ON                       ' trap background music events
         ON PLAY(3) GOSUB PlayMusic    ' If there are less than 3 notes
                                       ' in the buffer gosub line 1000
         PRINT "Press ENTER to end."   ' display info, how to end program

         PLAY "MB"                     ' background option for PLAY
         GOSUB PlayMusic               ' start the music

         kb$ = ""                      ' keyboard input buffer
         WHILE kb$ = ""                ' start of loop
            LOCATE 3, 1                ' position the cursor
            COLOR c                    ' change color and print some text,
                                       ' to show, that music executes
                                       ' independent
            PRINT "Playing your favourite music ..."
            c = c + 1                  ' next color
            IF c > 15 then c = 1       ' no blinking mode
            kb$ = INKEY$               ' get a character if present
         WEND                          ' end of loop
         COLOR 7                       ' white on black
         SYSTEM                        ' return to DOS

PlayMusic:
         PLAY "t180 o2 p2 p8 L8 GGG L2 E- p24 p8 L8 FFF L2 D"
         RETURN
