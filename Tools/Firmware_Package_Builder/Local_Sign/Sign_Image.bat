:: Utility to local sign binary
@ECHO off

set sn=8
set prod_name=PXGreen
set format=pem
set csr_extn=pem
set key_extn=pem
set cert_extn=pem
REM set Image=..\..\..\..\..\Build_Output\STM32F427_With_UberLoader_Release\Exe\RTK_Example.bin
set Image=%1

if '%Image%'=='' goto Set_Image
if not '%Image%'=='' goto Skip_Set_Image

:Set_Image
set /p Image=Enter input Image path:
	if '%Image%'=='' (
	goto Set_Image
	)
:Skip_Set_Image

set Signer=%2
if '%Signer%'=='' set Signer=./certs/%prod_name%.chain.cert.%cert_extn%

set Certfile=%3
if '%Certfile%'=='' set Certfile=./certs/ca.chain.cert.%cert_extn%

set Priv_Key=%4
if '%Priv_Key%'=='' set Priv_Key=./private/%prod_name%.key.%key_extn%


ECHO Input file Path -  %Image%
ECHO Output file(p7b) - %Image%.p7b

REM Sign a file 
 
openssl cms -sign -binary -noattr -signer %Signer% -certfile %Certfile% -inkey %Priv_Key% -md sha256 -outform der -in %Image% -out %Image%.p7b


ECHO Input file Path later -  %Image%
ECHO Output file(p7b) later - %Image%.p7b


REM openssl cms -sign -binary -nocerts -noattr -signer ./certs/%prod_name%.chain.cert.%cert_extn% -inkey ./private/%prod_name%.key.%key_extn% -md sha256 -outform der -in %Image% -out %Image%.p7b


REM View certificate store in p7b 
REM openssl pkcs7 -inform der -print_certs -in %Image%.p7b
