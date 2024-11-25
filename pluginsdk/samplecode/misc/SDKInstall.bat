echo off
echo.
echo Copy the SDK examples to the common location where Photoshop will find them. 
echo Use the Windows Registry to find the location of installed versions of Photoshop. 
echo This script no longer prompts for options and defaults to 64 bit debug copy.
echo.    
echo Script  Version  2023.10.12
set WinVer=
set WinMajor=
set WinMinor=
set WinBuild=
:; Get Value from 'VER' command output 
FOR /F "tokens=*" %%i in ('VER') do SET WinVer=%%i 
FOR /F "tokens=1-3 delims=]-" %%A IN ("%WinVer%" ) DO ( 
 set Var1=%%A 
) 
:; Get version number only so drop off Microsoft Windows Version 
FOR /F "tokens=1-9 delims=n" %%A IN ("%Var1%" ) DO ( 
 set WinVer=%%C 
 REM echo %WinVer% 
) 
:; Separate version numbers 
FOR /F "tokens=1-8 delims=.-" %%A IN ("%WinVer%" ) DO ( 
set WinMajor=%%A 
set WinMinor=%%B 
set WinBuild=%%C 
) 
:; Fix the extra space left over in the Major 
FOR /F "tokens=1 delims= " %%A IN ("%WinMajor%" ) DO ( 
 set WinMajor=%%A 
) 
:; Display Results	
ECHO Windows Version %WinVer% 
REM ECHO WinMajor = %WinMajor% 
REM ECHO WinMinor = %WinMinor% 
REM ECHO WinBuild = %WinBuild% 
echo.
set ps_cc_2019="NOT INSTALLED"
set ps_2020="NOT INSTALLED"
set ps_2021="NOT INSTALLED"
set ps_2021_arm="NOT INSTALLED"
set ps_2022="NOT INSTALLED"
set ps_2022_arm="NOT INSTALLED"
set ps_2023="NOT INSTALLED"
set ps_2023_arm="NOT INSTALLED"
set ps_2024="NOT INSTALLED"
set ps_2024_arm="NOT INSTALLED"
set ps_install_path="NOT INSTALLED"
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\130.0\PluginPath" ') DO SET ps_cc_2019="%%B"
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\140.0\PluginPath" ') DO SET ps_2020="%%B"
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\150.0\PluginPath" ') DO SET ps_2021="%%B"
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\160.0\PluginPath" ') DO SET ps_2022="%%B"
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\170.0\PluginPath" ') DO SET ps_2023="%%B"
 FOR /F "tokens=2* delims=	 " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\180.0\PluginPath" ') DO SET ps_2024="%%B"
) ELSE (
 echo Windows ARM64 %PROCESSOR_ARCHITECTURE%
 FOR /F "tokens=2* delims=    " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\150.0\PluginPath" ') DO SET ps_2021_arm="%%B"
 FOR /F "tokens=2* delims=    " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\160.0\PluginPath" ') DO SET ps_2022_arm="%%B"
 FOR /F "tokens=2* delims=    " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\170.0\PluginPath" ') DO SET ps_2023_arm="%%B"
 FOR /F "tokens=2* delims=    " %%A IN ('REG QUERY "HKLM\SOFTWARE\Adobe\Photoshop\180.0\PluginPath" ') DO SET ps_2024_arm="%%B"
)
echo.
echo I found these installed versions:
echo.
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo 1.  Photoshop CC  2019     %ps_cc_2019%
 echo 2.  Photoshop     2020     %ps_2020%
 echo 3.  Photoshop     2021     %ps_2021%
 echo 4.  Photoshop     2022     %ps_2022%
 echo 5.  Photoshop     2023     %ps_2023%
 echo 6.  Photoshop     2024     %ps_2024%
) ELSE (
 echo 1.  Photoshop ARM 2021     %ps_2021_arm%
 echo 2.  Photoshop ARM 2022     %ps_2022_arm%
 echo 3.  Photoshop ARM 2023     %ps_2023_arm%
 echo 4.  Photoshop ARM 2024     %ps_2024_arm%
)
echo.
REM set /p userinp=Pick the Photoshop to install into:
REM set userinp=%userinp:~0,1%
set userinp=I
if "%userinp%"=="1" goto CC201964
if "%userinp%"=="2" goto NOCC202064
if "%userinp%"=="3" goto NOCC202164
if "%userinp%"=="4" goto NOCC202264
if "%userinp%"=="5" goto NOCC202364
if "%userinp%"=="6" goto NOCC202464
if "%userinp%"=="I" goto DEFAULTCOMMON
echo Invalid Choice, quitting.
goto END
:CC201964
echo Photoshop CC 2019 64
set ps_install_path=%ps_cc_2019%
goto SDK
:NOCC202064
echo Photoshop 2020 64
set ps_install_path=%ps_2020%
goto SDK
:NOCC202164
echo Photoshop 2021 64
set ps_install_path=%ps_2021%
goto SDK
:NOCC202264
echo Photoshop 2022 64
set ps_install_path=%ps_2022%
goto SDK
:NOCC202364
echo Photoshop 2023 64
set ps_install_path=%ps_2023%
goto SDK
:NOCC202464
echo Photoshop 2024 64
set ps_install_path=%ps_2024%
goto SDK
:DEFAULTCOMMON
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo Photoshop 64 Common
) ELSE (
 echo Photoshop 64 Common ARM
)
set ps_install_path="C:\Program Files\Common Files\Adobe\Plug-Ins\CC\SDK\"
:SDK
REM echo.
REM echo Which SDK build to install:
REM echo.
REM echo 1. Debug   64 bit
REM echo 2. Release 64 bit
REM set /p userinp=Choose a number ( 1 - 2 ):
REM set userinp=%userinp:~0,1%
set userinp=3
if "%userinp%"=="1" goto D64
if "%userinp%"=="2" goto R64
if "%userinp%"=="3" goto DEF
echo Invalid Choice, quitting.
goto END
:D64
echo.
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo Debug 64
 set sdk_source_path=..\Output\x64\Debug\*.8??
) ELSE (
 echo Debug 64 ARM
 set sdk_source_path=..\Output\ARM64\Debug\*.8??
)
goto COPY
:R64
echo.
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo Release 64
 set sdk_source_path=..\Output\x64\Release\*.8??
) ELSE (
 echo Release 64 ARM
 set sdk_source_path=..\Output\ARM64\Release\*.8??
)
goto COPY
:DEF
echo.
IF %PROCESSOR_ARCHITECTURE%==AMD64 (
 echo Common Debug 64
 set sdk_source_path=..\Output\x64\Debug\*.8??
) ELSE (
 echo Common Debug 64 ARM
 set sdk_source_path=..\Output\ARM64\Debug\*.8??
)
goto COPY

:COPY
echo.
echo Copying from here %sdk_source_path% to here %ps_install_path%.
xcopy %sdk_source_path% %ps_install_path%. /E /F /H /R /D /C /Y

:END
set ps_cc_2019=
set ps_2020=
set ps_2021=
set ps_2021_arm=
set ps_2022=
set ps_2022_arm=
set ps_2023=
set ps_2023_arm=
set ps_2024=
set ps_2024_arm=
set ps_install_path=
set sdk_source_path=
set userinp=
set WinVer=
set WinMajor=
set WinMinor=
set WinBuild=
set Var1=
