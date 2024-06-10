@ECHO off
:: builder utility to add keys to firmware

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
REM Ahmed
set DEFAULT_PROJECT_FILE_NAME=Smart_Breaker_2_0
:: Instead of hard coding "app_bin_name" to "RTK_Example", project file name without extension is given as fifth argument so that it gets correct hex file for further processing.
set choice=%1
set option=%2
set code_pack_choice=%3
set ui_selection=%4
set app_bin_name=%5

:: making cross sign default to 0. Adopters needs to modify it to 1 if they want to use cross signing. Note If cross sign is being used check that RVK is different in both public\ as well as trusted public\ key folders 
:: RVK, IVK , FVK keys and certs of both public and trusted folders should be different.Trusted public folders should contain existing signing keys/certs and pubic folder should contain new sets of signing keys/certs
set CROSS_SIGN=0


:: local sign certs and keys path
set LOCAL_FVK_NAME=Smartbreaker2
set LOCAL_FVK_KEY=%LOCAL_FVK_NAME%.pub.der
set LOCAL_RVK_KEY=ca.pub.der
set LOCAL_KEY_PATH=Local_Sign\public\
set LOCAL_FVK_CERT_DER=%LOCAL_FVK_NAME%.cert.der
set LOCAL_IVK_CERT_DER=intermediate.cert.der
set LOCAL_CERT_PATH=Local_Sign\certs\
set LOCAL_TRUSTED_CERT_PATH=Local_Sign\trusted\certs\
set LOCAL_TRUSTED_FVK_CERT_DER=%LOCAL_FVK_NAME%.cert.der
set LOCAL_TRUSTED_KEY_PATH=Local_Sign\trusted\public\
set LOCAL_TRUSTED_RVK_KEY=ca.pub.der

:: server sign certs and keys path
set SERVER_FVK_KEY=FW-Smartbreaker2-CMS.pub.der
set SERVER_RVK_KEY=Eaton_Commercial_Root_CA_1.pub.der
set SERVER_KEY_PATH=Server_Sign\public\
set SERVER_FVK_CERT_DER=FW-Smartbreaker2-CMS.der
set SERVER_IVK_CERT_DER=Eaton_Firmware_Signing_MA_Issuing_CA_1.crt
set SERVER_CERT_PATH=Server_Sign\certs\
REM will have a folder in Server_Sign which will have trusted certs
set SERVER_TRUSTED_CERT_PATH=Server_Sign\trusted\certs\
set SERVER_TRUSTED_FVK_CERT_DER=FW-Smartbreaker2-CMS.der
set SERVER_TRUSTED_KEY_PATH=Server_Sign\trusted\public\
set SERVER_TRUSTED_RVK_KEY=Eaton_Commercial_Root_CA_1.pub.der

set bin_extn=bin
set hex_extn=hex

set batch_path=%~dp0
cd %batch_path%
set prj_path=%batch_path%..\..\

set Firmware_Package_Builder_Path=%batch_path%
set app_path=

set code_sign_info_ini=Code_Sign_Info_header.ini
set Builder_Utl=%prj_path%Babelfish\Tools\FW_Upgrade\New_Code_Sign\Builder\
set Code_Pack_Utl=%prj_path%Babelfish\Tools\LTKCodePackGenerator\
set Code_Pack_INI_Path=%batch_path%Code_Pack_INI\
set CRC_Gen_Utl=%prj_path%Babelfish\Tools\CRC_Generator\

set web_path=%prj_path%build\Web\

for %%i in (git.exe) do set "git_command=%%~$PATH:i"


:: **************** Processor selection start ****************

:processor_selection

if exist %prj_path%\Tools\Firmware_Package_Builder\errorcode.txt ( 
	del /s /q /f "%prj_path%\Tools\Firmware_Package_Builder\errorcode.txt"
)
if '%choice%'=='' (
	ECHO.
	ECHO.
	ECHO 1. Create Code Pack for ESP32 PROCESSOR
	set /p choice=Enter the number for your choice of PROCESSOR:
	if not '%choice%'=='' set choice=%choice:~0,1%
)
ECHO you selected choice %choice%

if '%choice%'=='1' (
	set app_path=%prj_path%build\
	::ESP32 has two OTA partitions, size of both partitions is same hence you can provide start and end address of any partition.. These addresses are used to verify that length of application is within range.
	set App_fw_HexBaseAddr=0x00000000
        set App_fw_HexEndAddr=0x0017ffff
	set Web_Fw_HexBaseAddr=0x00000000
	set Web_Fw_HexEndAddr=0x00080000
	set App_Code_Pack_INI_Path=%Code_Pack_INI_Path%ESP32_INI
	set proc_codepack_name=esp32
	
	goto sign_method_selection
)

ECHO %choice% is not a valid option. Please try again...
ECHO.
ECHO.
set choice=
goto processor_selection

:: **************** Processor selection end ****************

:sign_method_selection
if '%option%'=='' (
ECHO.
ECHO.
ECHO 1. Local Sign
ECHO 2. Server Sign
set /p option=Enter the number for your choice of signing method:
if not '%option%'=='' set option=%option:~0,1%
)
if '%option%'=='1' goto local_sign
if '%option%'=='2' goto server_sign
ECHO "%option%" is not valid, please try again!
ECHO.
ECHO.
set option=
goto sign_method_selection


:local_sign
:: local sign certs and keys path
set fvk_key=%LOCAL_FVK_KEY%
set rvk_key=%LOCAL_RVK_KEY%
set key_path=%LOCAL_KEY_PATH%
set fvk_cert_der=%LOCAL_FVK_CERT_DER%
set ivk_cert_der=%LOCAL_IVK_CERT_DER%
set cert_path=%LOCAL_CERT_PATH%
REM will have a folder in Local_Sign which will have trusted certs
set trusted_cert_path=%LOCAL_TRUSTED_CERT_PATH%
set tfvk_cert_der=%LOCAL_TRUSTED_FVK_CERT_DER%
set trusted_key_path=%LOCAL_TRUSTED_KEY_PATH%
set trvk_key=%LOCAL_TRUSTED_RVK_KEY%
set sign_method=l_sign
set sign_symbol=l
goto Sign_Start


:server_sign
:: server sign certs and keys path
set fvk_key=%SERVER_FVK_KEY%
set rvk_key=%SERVER_RVK_KEY%
set key_path=%SERVER_KEY_PATH%
set fvk_cert_der=%SERVER_FVK_CERT_DER%
set ivk_cert_der=%SERVER_IVK_CERT_DER%
set cert_path=%SERVER_CERT_PATH%
REM will have a folder in Server_Sign which will have trusted certs
set trusted_cert_path=%SERVER_TRUSTED_CERT_PATH%
set tfvk_cert_der=%SERVER_TRUSTED_FVK_CERT_DER%
set trusted_key_path=%SERVER_TRUSTED_KEY_PATH%
set trvk_key=%SERVER_TRUSTED_RVK_KEY%
set sign_method=s_sign
set sign_symbol=s
goto Sign_Start


:Sign_Start
if %CROSS_SIGN%==1 (
goto cross_sign_check
) else (
goto code_pack_selection
)
REM ********************************************************************
REM ******************* Cross signing error check start ****************
REM ********************************************************************
:cross_sign_check
fc  /b "%trusted_key_path%%trvk_key%" "%key_path%%rvk_key%">NUL
if %ERRORLEVEL%==0 (
ECHO RVK keys same.Do you want to continue with cross signing:
goto cross_sign_warning
) else (
goto code_pack_selection
)
:cross_sign_warning
set alternative=
if '%alternative%'=='' (
ECHO 1. Continue with Cross signing
ECHO 2. Exit
set /p alternative=Enter the number for your choice of signing method:
if not '%alternative%'=='' set alternative=%alternative:~0,1%
)
if '%alternative%'=='1' goto code_pack_selection
if '%alternative%'=='2' goto End
ECHO "%alternative%" is not valid, please try again!
ECHO.
ECHO.
goto cross_sign_warning
REM ********************************************************************
REM ******************* Cross signing error check end ****************
REM ********************************************************************



:: **************** Code pack selection start ****************

:code_pack_selection

if '%code_pack_choice%'=='' (
ECHO.
ECHO.
ECHO 1. Create Code Pack for Web UI
ECHO 2. Create Code Pack for Application
ECHO 3. Create Code Pack for both
set /p code_pack_choice=Enter the number for your choice of Code pack generation:
if not '%code_pack_choice%'=='' set code_pack_choice=%code_pack_choice:~0,1%
)

if '%code_pack_choice%'=='1' (
	goto web_ui_selection
)
if '%code_pack_choice%'=='2' (
	goto Sign_APP_Image
)
if '%code_pack_choice%'=='3' (
	goto Sign_APP_Image
)

ECHO "%code_pack_choice%" is not a valid choice. Please try again...
ECHO.
ECHO.
set code_pack_choice=
goto code_pack_selection

:: **************** Code pack selection end  ****************


:: **************** WEB binary Generation start ****************

:web_ui_selection

if '%ui_selection%'=='' (
	ECHO.
	ECHO.
	ECHO 1. Create Code Pack for Web UI 1.0
	ECHO 2. Create Code Pack for Web UI 2.0
	ECHO 3. Create Code Pack for Web UI 2.0 with i18n
	set /p ui_selection=Enter the number for your choice of Web UI:
	if not '%ui_selection%'=='' set ui_selection=%ui_selection:~0,1%
)

if '%ui_selection%'=='1' (
	set Code_Pack_INI_Web_Path=%Code_Pack_INI_Path%Web_1_INI
	set web_bin_name=web
	call "%batch_path%Web_Image_Builder.bat"
	goto Sign_WEB_Image
)
if '%ui_selection%'=='2' (
	set Code_Pack_INI_Web_Path=%Code_Pack_INI_Path%Web_2_INI
	set web_bin_name=web2
	call "%batch_path%Web_2_Image_Builder.bat" 0
	goto Sign_WEB_Image
)
if '%ui_selection%'=='3' (
	set Code_Pack_INI_Web_Path=%Code_Pack_INI_Path%Web_2_INI
	set web_bin_name=web2
	call "%batch_path%Web_2_Image_Builder.bat" 1
	goto Sign_WEB_Image
)

ECHO "%ui_selection%" is not a valid choice. Please try again...
ECHO.
ECHO.
set ui_selection=
goto web_ui_selection


:: **************** WEB binary Generation end  ****************




:Sign_WEB_Image

if exist "%prj_path%\Tools\Firmware_Package_Builder\errorcode.txt" ( 
	goto End
)

ECHO.
ECHO.
ECHO ***Starting Code Signing for Web Image***
REM -CS %CROSS_SIGN% is an argument suggesting whether cross signing is enabled or not
REM Cross signing is reqired for app image only.In cross signing app image should be updated first in order to update keys and certs in cert store area.After that web image should be done using normal signing.
"%Builder_Utl%code_sign_info.exe" -bin "%web_path%%web_bin_name%.%bin_extn%" -fvkc "%cert_path%%fvk_cert_der%" -fvk "%key_path%%fvk_key%" -ivkc "%cert_path%%ivk_cert_der%" -ini %code_sign_info_ini% -rvk "%key_path%%rvk_key%"  -out "%web_path%%web_bin_name%.tmp.%bin_extn%" -CS 0

if '%sign_method%'=='l_sign' (
	call %cert_path%..\Sign_Image.bat "%web_path%%web_bin_name%.tmp.%bin_extn%" "%cert_path%%LOCAL_FVK_NAME%.chain.cert.pem" "%cert_path%ca.chain.cert.pem" "%cert_path%..\private\%LOCAL_FVK_NAME%.key.pem"
) else (
	call %cert_path%..\Sign_Image.bat "%web_path%%web_bin_name%.tmp.%bin_extn%" 
)

if not exist "%web_path%%web_bin_name%.tmp.%bin_extn%.p7b" (
	ECHO "%web_path%%web_bin_name%.tmp.%bin_extn%.p7b" could not be generated...
	goto End
)

call "%Builder_Utl%Signature_Extractor.bat" "%web_path%%web_bin_name%.tmp.%bin_extn%.p7b"


"%Builder_Utl%append_sign_info.exe" -bin "%web_path%%web_bin_name%.tmp.%bin_extn%" -fwsign "%web_path%%web_bin_name%.tmp.%bin_extn%.p7b.sig" -tfvkc "%trusted_cert_path%%tfvk_cert_der%" -out "%web_path%%web_bin_name%.signed.%bin_extn%" -CS 0

ECHO ***Code Signing Completed for Web Image***
ECHO.
ECHO.

:: Getting size of web bin file
set web_file="%web_path%%web_bin_name%.signed.%bin_extn%"
call :setsize_web %web_file%
ECHO Size of web bin file is %size_of_web_bin% bytes

:: Calculating the memory allocated for Web Image
set /A web_mem_available = %Web_Fw_HexEndAddr% - %Web_Fw_HexBaseAddr%+1
ECHO Memory available for web image is %web_mem_available% bytes

if %web_mem_available% LSS %size_of_web_bin% ( 
	ECHO Error : Size of Web image is greater than the available memory
	goto End
) else ( 
	"%Builder_Utl%..\..\bin2srec\srec_cat.exe" "%web_path%%web_bin_name%.signed.%bin_extn%" -binary -offset %Web_Fw_HexBaseAddr% -o "%web_path%%web_bin_name%.signed.%hex_extn%" -intel --line-length=44
	
	cd %Code_Pack_Utl%
	
	if not exist "%prj_path%build\Code_Pack" mkdir %prj_path%build\Code_Pack
	
	LTKCodePackGen.exe silent createcodepackfile inifolder="%Code_Pack_INI_Web_Path%" xmlfile="%prj_path%build\Code_Pack\%web_bin_name%.%proc_codepack_name%.%sign_symbol%.signed.pack.xml"
	
	cd %Firmware_Package_Builder_Path%
)

ECHO.
ECHO.

REM ********************************************************************
REM ******************* Process WEB Image - End ************************
REM ********************************************************************
goto :eof



REM ********************************************************************
REM ******************* Process APP Image - Start **********************
REM ********************************************************************
:Sign_APP_Image

:: **************** APP binary Generation start ****************
REM Ahmed
if '%app_bin_name%'=='' (
	ECHO.
	ECHO.
	ECHO Default Project App name assigned is "Smart_Breaker_2_0".
	ECHO a. Press Enter to continue:
	ECHO b. If different, Provide correct project app file name and press Enter: 
	set /p app_bin_name=Enter the Project App File Name, without extension: 
)

if '%app_bin_name%'=='' (
	set app_bin_name=%DEFAULT_PROJECT_FILE_NAME%
)
set app_bin_name_crc=%app_bin_name%_crc

if not exist "%app_path%%app_bin_name%.%bin_extn%" (
	ECHO "%app_path%%app_bin_name%.%bin_extn%" not found...
	set choice=
	goto processor_selection
)
:: **************** APP binary Generation start ****************

ECHO.
ECHO.
ECHO ***Starting Code Signing for App Image***

::ECHO ----Generating crc-----
::ECHO CRC_Generator tool path : %CRC_Gen_Utl%
::In ESP32, PXGreen custom header is added after esp32 headers.
::Offset for PXGreen header is sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t) = 24+ 8 + 256 = 288.
::set crc_offset=288
::Data start offset is (crc_offset + 4)
::set data_start_offset=292
::"%CRC_Gen_Utl%CRC_Generator.exe" -bin "%app_path%%app_bin_name%.%bin_extn%" -out "%app_path%%app_bin_name_crc%.%bin_extn%" -offset %crc_offset% -algo 1 -data_start %data_start_offset% -data_log 0	
::ECHO ----crc generation end-----

"%Builder_Utl%code_sign_info.exe" -bin "%app_path%%app_bin_name%.%bin_extn%" -fvkc "%cert_path%%fvk_cert_der%" -fvk "%key_path%%fvk_key%" -ivkc "%cert_path%%ivk_cert_der%" -ini %code_sign_info_ini% -rvk "%key_path%%rvk_key%" -out "%app_path%%app_bin_name%.tmp.%bin_extn%" -CS %CROSS_SIGN%

if '%sign_method%'=='l_sign' (
	call %cert_path%..\Sign_Image.bat "%app_path%%app_bin_name%.tmp.%bin_extn%" "%cert_path%%LOCAL_FVK_NAME%.chain.cert.pem" "%cert_path%ca.chain.cert.pem" "%cert_path%..\private\%LOCAL_FVK_NAME%.key.pem"
) else (
	call %cert_path%..\Sign_Image.bat "%app_path%%app_bin_name%.tmp.%bin_extn%" 
)

if not exist "%app_path%%app_bin_name%.tmp.%bin_extn%.p7b" (
	ECHO "%app_path%%app_bin_name%.tmp.%bin_extn%.p7b" could not be generated...
	goto End
)

call "%Builder_Utl%Signature_Extractor.bat" "%app_path%%app_bin_name%.tmp.%bin_extn%.p7b"

"%Builder_Utl%append_sign_info.exe" -bin "%app_path%%app_bin_name%.tmp.%bin_extn%" -fwsign "%app_path%%app_bin_name%.tmp.%bin_extn%.p7b.sig" -tfvkc "%trusted_cert_path%%tfvk_cert_der%" -out "%app_path%%app_bin_name%.signed.%bin_extn%" -CS %CROSS_SIGN%

REM ********************************************************************
REM ******************* Cross Signing Process APP Image - Start ********
REM ********************************************************************
REM If cross sign is enable we will have to sign the bin with added extended sign information and adding trusted fw sign
if %CROSS_SIGN%==1 (
if '%sign_method%'=='l_sign' (
	call %cert_path%..\Sign_Image.bat "%app_path%%app_bin_name%.signed.%bin_extn%" "%trusted_cert_path%%LOCAL_FVK_NAME%.chain.cert.pem" "%trusted_cert_path%ca.chain.cert.pem" "%trusted_cert_path%..\private\%LOCAL_FVK_NAME%.key.pem"
) else (
	call %cert_path%..\Sign_Image.bat "%app_path%%app_bin_name%.signed.%bin_extn%" )
	
	if not exist "%app_path%%app_bin_name%.signed.%bin_extn%.p7b" (
	ECHO "%app_path%%app_bin_name%.signed.%bin_extn%.p7b" could not be generated...
	goto End
)

call "%Builder_Utl%Signature_Extractor.bat" "%app_path%%app_bin_name%.signed.%bin_extn%.p7b"

"%Builder_Utl%append_cross_sign_info.exe" -bin "%app_path%%app_bin_name%.signed.%bin_extn%" -fwsign "%app_path%%app_bin_name%.signed.%bin_extn%.p7b.sig" -tfvkc "%trusted_cert_path%%tfvk_cert_der%"
set sign_symbol=%sign_symbol%.c
ECHO CROSS SIGN Utility executed
)

REM ********************************************************************
REM ******************* Cross Signing Process APP Image - END **********
REM ********************************************************************

ECHO ***Code Signing Completed for App Image***
ECHO.
ECHO.

:: Getting size of application bin file
set app_file="%app_path%%app_bin_name%.signed.%bin_extn%"
call :setsize_app %app_file%
ECHO Size of app bin file is %size_of_app_bin% bytes

:: Calculating the memory allocated for Application Image
set /A app_mem_available = %App_fw_HexEndAddr% - %App_fw_HexBaseAddr%+1
ECHO Memory available for application is %app_mem_available% bytes

if %app_mem_available% LSS %size_of_app_bin% ( 
	ECHO Error : Size of App image is greater than the available memory
	goto End
) else ( 
	"%Builder_Utl%..\..\bin2srec\srec_cat.exe" "%app_path%%app_bin_name%.signed.%bin_extn%" -binary -offset %app_Fw_HexBaseAddr% -o "%app_path%%app_bin_name%.signed.%hex_extn%" -intel --line-length=44
	
	cd %Code_Pack_Utl%
	
	if not exist "%prj_path%build\Code_Pack" mkdir %prj_path%build\Code_Pack
	
	LTKCodePackGen.exe silent createcodepackfile inifolder="%App_Code_Pack_INI_Path%" xmlfile="%prj_path%build\Code_Pack\%app_bin_name%.%proc_codepack_name%.%sign_symbol%.signed.pack.xml"
	
	cd %Firmware_Package_Builder_Path%
)
ECHO.
ECHO.

if '%code_pack_choice%'=='3' (
	goto web_ui_selection
)

REM ********************************************************************
REM ******************* Process APP Image - End ************************
REM ********************************************************************

goto End

:setsize_app
set size_of_app_bin=%~z1
goto :eof

:setsize_web
set size_of_web_bin=%~z1
goto :eof

:HELP
ECHO Command Format:
ECHO Package_Buider.bat processor_id(optional) signing_method(optional) codepack_selection(optional) webui_type(optional) app_name(optional) 
ECHO where "processor_id" is - 
ECHO 	1. Create Code Pack for ESP32 PROCESSOR
ECHO where "signing_method" is - 
ECHO 	1. Local Sign
ECHO 	2. Server Sign
ECHO where "codepack_selection" is - 
ECHO	1. Create Code Pack for Web UI
ECHO	2. Create Code Pack for Application (currently only this option is supported)
ECHO	3. Create Code Pack for both
ECHO where "webui_type" is - 
ECHO	1. Create Code Pack for Web UI 1.0
ECHO	2. Create Code Pack for Web UI 2.0
ECHO	3. Create Code Pack for Web UI 2.0 with i18n
ECHO and "app_name" is -
ECHO 	Application image name ( without extension ). This name generally matches with IAR Project name. 
ECHO    Refer - "IAR Project -> Options -> Output Converter -> Output file".
REM Ahmed
ECHO 	If no option is provided then Default name "Smart_Breaker_2_0" is considered.
ECHO For e.g. to generate signed binary for application using local sign method on ESP32 processor having project name PX_Green, the command should be entered as - 
ECHO Package_Buider.bat 1 1 2 0 PX_Green
ECHO NOTE: By default cross signing is disable.In order to enable cross signing make CROSS_SIGN as 1 in this utility.

GOTO End

:End
