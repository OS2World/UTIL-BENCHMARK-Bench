@echo off
 echo Sample CMD-File for building BENCH.EXE
 SET LIB=C:\SDK12\TOOLKT12\LIB;D:\C600\LIB;
 SET INCLUDE=C:\SDK12\TOOLKT12\C\INCLUDE;D:\C600\INCLUDE;
 SET OLDPATH=%PATH%
 SET PATH=D:\C600\BINB;D:\C600\BINP;%PATH%
 IF "%1"=="" GOTO usage
 cl /AL /FPa /G2sw /Od /c /Zpi  %1.c
 IF errorlevel==1 GOTO fehler
 link /CO  %1,,nul,os2,%1
 IF errorlevel==1 GOTO fehler
 rc %1
 IF errorlevel==1 GOTO fehler
 %1
 goto fehler
 :usage
 echo  usage :   c <filename>  !
 :fehler
 SET PATH=%OLDPATH%
