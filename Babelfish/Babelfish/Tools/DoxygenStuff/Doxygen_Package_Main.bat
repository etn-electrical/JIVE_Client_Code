:: This is main batch file trigger python script and generate the doxygen document
@ECHO OFF





if "%~1"=="" (
    ::echo No parameters have been provided. so assiging default 
    set OUTPUT_PATH=%~dp0..\..\Docs\Generated\edge-rtos-app-Babelfish-doxygen
) else (
    set OUTPUT_PATH=%*   
)
echo output path = %OUTPUT_PATH%

set batch_path=%~dp0
:: echo m_batch_path is %batch_path%
cd %batch_path%
set prj_path=%batch_path%..\
::echo %CD%
::echo %prj_path% 
echo "Python file called"


 call  python3 .\DoxyGen.py %OUTPUT_PATH%
echo "Main Batch file execution completed "

