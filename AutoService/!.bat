@echo off
echo.
echo MDM_ACCESS ver. 1.0 (22.07.2003)
if %1.==. goto nocom
echo !A_SRC ..\ MDM_ACCESS %1 ..\LastBackup.h !exclude.txt C:\Kolevatov\BACKUP
echo.
echo !A_SRC ..\ MDM_ACCESS %1 ..\LastBackup.h !exclude.txt C:\Kolevatov\BACKUP %2
if %2.==.     call !A_SRC ..\ MDM_ACCESS  %1 ..\LastBackup.h !exclude.txt C:\Kolevatov\BACKUP
if not %2.==. call !A_SRC ..\ MDM_ACCESS  %1 ..\LastBackup.h !exclude.txt C:\Kolevatov\BACKUP %2
goto end
:nocom
echo Type Build number !!! Ex: !makeres 10
:end
