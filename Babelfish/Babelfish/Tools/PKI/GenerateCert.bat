:: Utility to generate Private Key and Certificate
@ECHO off

:: Extract value of days from CsrConfig file
set line=
for /f "delims=" %%a in ('findstr "days" CsrConfig.conf') do set line=%%a
for /f "tokens=1,2,3 delims= " %%x in ("%line%") do set tag=%%x&set sign=%%y&set days=%%z

:: Create a Certificate directory in Build_Output folder
cd ../../../Build_Output/
mkdir Certificate
cd ../Babelfish/Tools/PKI/OpenSSL/

ECHO.
ECHO /********************Generating Key and CSR********************/
ECHO.

set keyFileName=
set /P keyFileName=Enter private key file name(default: PrivKey):
if not defined keyFileName goto defaultKeyName
if defined keyFileName goto customKeyName

:defaultKeyName
openssl.exe req -new -config ../CsrConfig.conf -out CSR.csr -days %days%
set keyFileName=default
goto keyFin

:customKeyName
openssl.exe req -new -config ../CsrConfig.conf -keyout %keyFileName%.key -out CSR.csr -days %days%
goto keyFin

:keyFin

ECHO.
ECHO /*********************Signing Certificate*********************/
ECHO.
set certFileName=
set /P certFileName=Enter certificate file name(default: Cert):
if not defined certFileName (
	set certFileName=Cert
)

if %keyFileName%==default (
	openssl.exe x509 -signkey PrivKey.key -req -days %days% -in CSR.csr -sha256 -out %certFileName%.crt -extfile openssl.cnf -extensions v3_req
	openssl.exe rsa -in PrivKey.key -outform DER -out PrivKey.der
	move privKey.der ../../../../Build_Output/Certificate
	del PrivKey.key
)
if NOT %keyFileName%==default (
	openssl.exe x509 -signkey %keyFileName%.key -req -days %days% -in CSR.csr -sha256 -out %certFileName%.crt -extfile openssl.cnf -extensions v3_req
	openssl.exe rsa -in %keyFileName%.key -outform DER -out %keyFileName%.der
	move %keyFileName%.der ../../../../Build_Output/Certificate
	del %keyFileName%.key
)

openssl.exe x509 -outform der -in %certFileName%.crt -out %certFileName%.der
move %certFileName%.der ../../../../Build_Output/Certificate
del %certFileName%.crt
del CSR.csr

pause