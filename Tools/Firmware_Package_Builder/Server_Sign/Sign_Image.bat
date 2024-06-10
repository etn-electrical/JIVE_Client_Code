@ECHO off

REM Prerequisites
REM 1. install openssl - https://slproweb.com/download/Win64OpenSSL-1_1_0L.exe
REM 2. download signserver client - https://signserver.tcc.etn.com/clients/SignServer-ClientCLI-4.2.1-bin.zip
REM 3. extract the content of zip into C drive such that below path is accessible - C:\signserver\bin
REM In Windows: 
REM Add environmet variable JAVA_HOME pointing to JDK or JRE installed like 'C:\Program Files\Java\jdk1.8.0_65'
REM In Windows: Add environment variable SIGNSERVER_HOME pointing to the SignServer installation path 'C:\Program REM Files\signserver\'
REM Add JAVA_HOME AND SIGNSERVER_HOME/bin path to PATH variable


set infile=%1

REM //Ahmed Ahmed changing the code from local signing to server signing
if '%infile%'=='' set infile=Smart_Breaker_2_0.bin
set SIGNSERVER_HOME=C:\signserver
set PATH=%PATH%;%SIGNSERVER_HOME%\bin
set worker=FW-Smartbreaker2-CMS
set shafile=%infile%.sha
set outfile=%infile%.p7b
set username="E0624432"

ECHO Input file - %infile%
ECHO Outputfile(p7b) - %outfile%

if '%username%'=='' goto Set_Username
if not '%username%'=='' goto Skip_Set_Username

:Set_Username
set /p username=Enter User Name (Eaton ID):
	if '%username%'=='' (
	goto Set_Username
	)
:Skip_Set_Username

REM Generate Hash
REM openssl dgst -sha256 -binary -out %infile%.sha %infile%
	
	
%SIGNSERVER_HOME%\bin\signclient.cmd signdocument -workername %worker% ^
-servlet /signserver/worker/%worker% ^
-truststore %SIGNSERVER_HOME%\eaton-truststore.jks ^
-truststorepwd eaton ^
-infile %infile% ^
-host signserver.tcc.etn.com -port 443 ^
-username %username% ^
-outfile %outfile%
REM -metadata USING_CLIENTSUPPLIED_HASH=true ^
REM -metadata CLIENTSIDE_HASHDIGESTALGORITHM=SHA-256


REM output file name - %infile%.p7b