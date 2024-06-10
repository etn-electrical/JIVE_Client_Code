:: The below code is only for creation of web related bin files. The Build_Web2_Binary.bat files creates bin for web.
@ECHO OFF

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

set isI18n=%1
set batch_path=%~dp0
set wbuild_prj_path=%batch_path%..\..\

set generate_bin_name=%2
if '%generate_bin_name%'=='' set generate_bin_name="%wbuild_prj_path%build\Web\www2.bin"

set output_web_bin_name=%3
:: output_web_bin_name this variable holds the name of the resultant web binary file generated after addition of header to bin
if '%output_web_bin_name%'=='' set output_web_bin_name="%wbuild_prj_path%build\Web\web2.bin"


set input_header_xml_name=%4
:: this is the name of the xml file that serves as an argument to ltk_header_gen.exe using which it can create the header for the binary file
if '%input_header_xml_name%'=='' set input_header_xml_name="%wbuild_prj_path%WebUI_2.0\web_2_header_linker.xml"


cd %wbuild_prj_path%WebUI_2.0

call Build_Web2_Binary.bat %generate_bin_name% %isI18n%
ECHO "Build_Web2_Binary executed"
if not exist %generate_bin_name% ( 
ECHO "%generate_bin_name%" is not generated/available.
goto ProcessError
)

CALL "ltk_header_gen.exe" -input_file %generate_bin_name% -output_file %output_web_bin_name% -input_xml %input_header_xml_name%

cd..
cd Tools\Firmware_Package_Builder
goto :eof

:HELP
ECHO Command Format : 
ECHO .
ECHO Web_2_Image_Builder.bat isI18n Generate_input_name.bin output_web_bin_name.bin input_header.xml
ECHO isI18n - This is a mandatory argument whose value should be 1 if i18n needs to be included otherwise 0
ECHO Generate_input_name - Web image is initially generated with this file name 
ECHO output_web_bin_name - Generate_input_name is then added with header information and saved with this name
ECHO If file name is not provided, Web_2_Image_Builder takes the default arugment for input/output file as below - 
ECHO argument 2 - build\Web\www2.bin
ECHO argument 3 - build\Web\web2.bin
ECHO argument 4 - ..\..\WebUI_2.0\web_header_linker.xml

:ProcessError 
echo 1 > "%wbuild_prj_path%\Tools\Firmware_Package_Builder\errorcode.txt"

:END
