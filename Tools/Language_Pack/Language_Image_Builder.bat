::/**
:: **************************************************************************************************
:: *  @file   Language_Image_Builder.bat
:: *  @details  This file has set of commands to create language binary image in two steps
:: *			1. Lang_Bin_Generator.exe : Collect all languages data (language header + language block) in a binary file Language_data.bin
:: * 			2. ltk_header_gen.exe : Add image header to the languages data
:: *
:: *  @copyright 2020 Eaton Corporation. All Rights Reserved.
:: *
:: **************************************************************************************************
:: */

@ECHO off

IF "%1" == "-help" (
GOTO HELP
)
IF "%1" == "--help" (
GOTO HELP
)
IF "%1%" == "-h" (
GOTO HELP
)
IF "%1%" == "--h" (
GOTO HELP
)

::Set required paths
set batch_path=%~dp0
set wbuild_prj_path=%batch_path%..\..\

:: Add all Language data in a binary file ******************************************************************************************

set ini_file_path=%1
if '%ini_file_path%'=='' set ini_file_path="%batch_path%Language_INI\\"

set output_bin_name=%2
:: output_bin_name this variable holds the name of the resultant language binary file generated after addition of all 
:: language INI and JSON files available in Language_INI folder
if '%output_bin_name%'=='' set output_bin_name="%wbuild_prj_path%Build_Output\Language\Language_data.bin"

cd %wbuild_prj_path%Babelfish\Tools\LanguagePackGenerator

Lang_Bin_Generator.exe -ini_path %ini_file_path% -out %output_bin_name%

:: Add image header *******************************************************************************************************************

cd %wbuild_prj_path%Babelfish\Tools\MaintenancePageGenerator

set output_language_bin_name=%3
:: output_language_bin_name this variable holds the name of the resultant language binary file generated after addition of header to bin
if '%output_language_bin_name%'=='' set output_language_bin_name="%wbuild_prj_path%Build_Output\Language\Language.bin"

set input_header_xml_name=%4
:: This is the name of the xml file that serves as an argument to ltk_header_gen.exe using which it can create the header for the binary file
if '%input_header_xml_name%'=='' set input_header_xml_name="%wbuild_prj_path%Tools\Language_Pack\language_header_linker.xml"

ltk_header_gen.exe -input_file %output_bin_name% -output_file %output_language_bin_name% -input_xml %input_header_xml_name%

cd..
cd..
cd..
cd Tools\Firmware_Package_Builder

goto END

:: **********************************************************************************************************************************
:HELP
ECHO Command Format : 
ECHO .
ECHO Language_Image_Builder.bat <ini_file_path> output_bin_name.bin output_language_bin_name.bin input_header_xml_name.xml
ECHO     <ini_file_path>          : Path of language INI files having language header.
ECHO     output_bin_name 		  : Language image is initially generated with this file name 
ECHO     output_language_bin_name : output_bin_name is then added with header information and saved as output_language_bin_name
ECHO     input_header_xml_name    : Language image header input file
ECHO If file name is not provided, Language_Image_Builder takes the default arugment for input/output files as below - 
ECHO param 1 - Language_INI\
ECHO param 2 - ..\..\Build_Output\Language\Language_data.bin
ECHO param 3 - ..\..\Build_Output\Language\Language.bin
ECHO param 4 - ..\..\Tools\Language_Pack\language_header_linker.xml

:END