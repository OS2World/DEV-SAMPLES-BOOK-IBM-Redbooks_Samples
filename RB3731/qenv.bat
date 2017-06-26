@rem ******************************************************
@rem * Program name :  QENV.BAT                           *
@rem * Purpose      :  Query DOS environment size         *
@rem *                 W10390 VDM Lab - VDM Configuration *
@rem * Author       :  Bernd Westphal                     *
@rem ******************************************************
@echo off
cls
echo Filling free environment space ....
echo.
echo Ignore any messages like "Out of environment space".
echo.
set Dummy1=Dummy.Text.to.fill.the.environment.space
set Dummy2=%Dummy1%
set Dummy3=%Dummy1%
set Dummy4=%Dummy1%
set Dummy5=%Dummy1%
cls
environ.exe
set Dummy1=
set Dummy2=
set Dummy3=
set Dummy4=
set Dummy5=
cls
echo The dummy environment settings have been removed.
