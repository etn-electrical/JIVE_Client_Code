::Builds select files into the BCM
@set folder=prod-gzip
@set listfile=list.txt
@set versionFile=.\app\scripts\version.js

::Set Variables
@FOR /F tokens^=2^ delims^=^" %%i IN (%versionFile%) DO @(
    @set v=1%%i
)
@set /a v=%v%+1
@set v=%v:~1%
::@echo var UI_VERSION="%v%"; > %versionFile%

@ECHO OFF

call npm run build

::Allow usage of ! and delayed expansion in for-loop
setlocal ENABLEDELAYEDEXPANSION

echo Creating %folder%\list.txt from all files in %folder%

pushd %folder%
del /Q %listfile%

::List must start with default, index, and not_found
ECHO default.html>>%listfile%
ECHO index.html>>%listfile%
ECHO not_found.html>>%listfile%	

SET "r=%__CD__:\=/%"
FOR /R . %%F IN (*.html;*.css;*.js;*.png;*.gif;*.svg;*.woff) DO (
        SET "p=%%F"
        SET "p=!p:\=/!"
        SET "p=!p:%r%=!"
        IF NOT !p!==default.html IF NOT !p!==index.html IF NOT !p!==not_found.html ECHO !p!>>%listfile%
)

copy /Y ..\mimes.txt .

::Now use txt2hex to convert the files listed in "list.txt" into www.bin
CALL ..\txt2upgrade.exe ..\www.bin

@ECHO ON

@popd

@FOR /F "tokens=2* delims='" %%i IN (%versionFile%) DO @(
    @set v=%%i
)
@echo Built ui_version %v% (%v:~0,2%.%v:~2,2%.%v:~4%)

::Build for srec
.\bin2srec\srec_cat.exe .\www.bin -binary -offset 0x08080000 -o www.hex -intel --line-length=44

