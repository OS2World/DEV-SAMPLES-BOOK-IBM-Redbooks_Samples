10 ' tim sennitt Boca Raton 1990
11 ' Basic program to call interrupt 19H
20 CLS:KEY OFF: PRINT"Press the ENTER key to call interrupt 19H":INPUT A$
50 CLEAR 6400:POKE 6400,&HCD:POKE 6401,&H19
60 INT19 = 6400:CALL INT19
