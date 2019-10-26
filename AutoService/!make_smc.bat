@echo off
rem -----------------------------------------------------------------------
rem void InitEmulation(void);
rem !make.bat
rem MD: 30.07.2003
rem LE: 08.08.2003
rem -----------------------------------------------------------------------
set ORIG_EXE_NAME=BACKOFF
rem -----------------------------------------------------------------------
echo.
echo !MAKE ver. 0.2
echo This BAT file compiles MAC802_11 SDL emulator application.
echo.
if %1.==. goto nocom
if %1.==1. goto C_ZEUS
if %1.==2. goto C_ARI
echo. ERROR: Invalid key %1
gotro end
rem -----------------------------------------------------------------------
:C_ZEUS
set EXE_NAME=ZEUS
set COMP_KEY=MAC_ZEUS
goto C_MAKE
rem -----------------------------------------------------------------------
:C_ARI
set EXE_NAME=ARIADNA
set COMP_KEY=MAC_ARIADNA
goto C_MAKE
rem -----------------------------------------------------------------------
:C_MAKE
echo config          = %EXE_NAME%
echo compilation key = %COMP_KEY%
IF EXIST procsys.obj DEL procsys.obj
"C:\Programme\Microsoft Visual Studio\VC98\Bin\nmake" /f C:\Kolevatov\Projects\SDL\MAC80211\Make\SysMAC_my.m SCTDIR=C:\Programme\Telelogic\SDL_TTCN_Suite4.3\sdt\sdtdir\wini386\SCTAMDEBCOM MAC_COMP_CFG=%COMP_KEY%
IF EXIST %EXE_NAME%_smc.exe DEL %EXE_NAME%_smc.exe
IF EXIST %ORIG_EXE_NAME%_smc.exe REN %ORIG_EXE_NAME%_smc.exe %EXE_NAME%_smc.exe
goto end
rem -----------------------------------------------------------------------
:nocom
echo Error: configuration is not selected
echo        use !MAKE.bat [key]
echo.
echo        key = 1 for ZEUS
echo        key = 2 for ARIADNA
echo.
:end