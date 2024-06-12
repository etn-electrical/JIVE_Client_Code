:: Utility to verify binary(test pupose - verificaton will a actually happen in firmware)
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


REM # Signature verification using certificate and p7b(CMS - not required any more) 
REM openssl cms -verify -binary -inform der -CAfile ./certs/ca.cert.%cert_extn% -purpose any -out ./temp/null -content %Image% -in %Image%.p7b

REM # Signature verification using public key and signature(dgst - verify) 
openssl dgst -verify ./public/%prod_name%.pub.%key_extn% -signature %Image%.sig %Image%

pause