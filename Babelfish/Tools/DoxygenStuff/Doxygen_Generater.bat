:: Automtic Doxygen File Genartsion 

@ECHO off
echo "Started the Doxygenertor batch file"
set batch_path=%~dp0
cd %batch_path%
set prj_path=%batch_path%..\..\..\..\

::echo %CD%
:echo projectpath  is %prj_path% 

for %%I in (.) do set CurrDirName=%%~nxI
echo CurrDirName is =  %CurrDirName%

if exist docs/%CurrDirName%_doxy (
  echo "allreadyexist"
  DIR
) else (
  echo "NoDoxyFileFound"
  
)

:: This command will generate the doxygen document 
C:\"Program Files"\doxygen\bin\doxygen docs\%CurrDirName%_doxy
echo "Completed doxygenerator batch"
:END
