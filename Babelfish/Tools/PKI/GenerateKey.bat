:: Utility to generate Key
@ECHO off

:: Create a Certificate directory in Build_Output folder if not already present
cd ../../../Build_Output/
mkdir Certificate
set certDir=%CD%\Certificate
cd ../Babelfish/Tools/PKI/OpenSSL/
cls

:keyType
ECHO.
ECHO 1. RSA
ECHO 2. ECC
set keyType=
set /p keyType=Enter the number for your choice of Key:
if not '%keyType%'=='' set choice=%keyType:~0,1%
if '%keyType%'=='1' goto RSA
if '%keyType%'=='2' goto ECC
ECHO "%keyType%" is not valid, please try again!
ECHO.
goto keyType

:RSA
ECHO.
ECHO /********************Generate RSA Key********************/
ECHO.

:RSAKeyStrength
ECHO 1. 1024
ECHO 2. 2048
ECHO 3. 4096
ECHO 4. 8192
set bits=0
set input=
set /p input=Enter the number for your choice of RSA Key Strength:
if not '%input%'=='' set choice=%input:~0,1%
if '%input%'=='1' set bits=1024
if '%input%'=='2' set bits=2048
if '%input%'=='3' set bits=4096
if '%input%'=='4' set bits=8192
if '%bits%'=='0' ( 
	ECHO "%input%" is not valid, please try again!
	ECHO.
	goto RSAKeyStrength
)

ECHO.
:RSAKeyExtension
ECHO 1. pem
ECHO 2. der
::ECHO 3. pub
::ECHO 4. key
set keyFormat=none
set input=
set /p input=Enter the number for your choice of Key Format:
if '%input%'=='1' set keyFormat=pem
if '%input%'=='2' set keyFormat=der
::if '%input%'=='3' set keyFormat=pub
::if '%input%'=='4' set keyFormat=key
if '%keyFormat%'=='none' ( 
	ECHO "%input%" is not supported, please try again!
	ECHO.
	goto RSAKeyExtension
)

ECHO.
set keyFileName=
set /P keyFileName=Enter key file name with extension(default: PrivKey.pem):
if not defined keyFileName set keyFileName=PrivKey.pem

:: Generate key pair
openssl genrsa -out %keyFileName%.pem %bits%

:: Separate out the public key
openssl rsa -in %keyFileName%.pem -pubout > %keyFileName%.pub
for /f "delims=." %%a in ("%keyFileName%") do set fileName=%%a
set pubKeyFileName=%fileName%_pubKey.pub

if '%keyFormat%'=='pem' (
	::Rename the public key and private key files
	REN %keyFileName%.pem %keyFileName%
	REN %keyFileName%.pub %pubKeyFileName%
	goto cleanUp
)

if '%keyFormat%'=='der' (
	::Convert PEM to DER format
	openssl rsa -in %keyFileName%.pem -outform DER -out %keyFileName%.der
	openssl rsa -in %keyFileName%.pub -pubin -outform DER -out %pubKeyFileName%
	REN %keyFileName%.der %keyFileName%
	del %keyFileName%.pem
	del %keyFileName%.pub
	goto cleanUp
)

::if '%keyFormat%'=='pub' (
::	::Convert PEM to PUB format
::	openssl rsa -in %keyFileName%.pem -outform PUB -out %keyFileName%.pub
::	del %keyFileName%.pem
::	goto cleanUp
::)

::if '%keyFormat%'=='key' (
::	::Convert PEM to KEY format
::	openssl rsa -in %keyFileName%.pem -outform KEY -out %keyFileName%.key
::	del %keyFileName%.pem
::	goto cleanUp
::)


:ECC
ECHO.
ECHO /********************Generate ECC Key********************/
ECHO.

:::ECCKeyStrength
::ECHO 1. 128
::ECHO 2. 256
::ECHO 3. 512
::set bits=0
::set input=
::set /p input=Enter the number for your choice of ECC Key Strength:
::if not '%input%'=='' set choice=%input:~0,1%
::if '%input%'=='1' set bits=128
::if '%input%'=='2' set bits=256
::if '%input%'=='3' set bits=512
::if '%bits%'=='0' ( 
::	ECHO "%input%" is not valid, please try again!
::	ECHO.
::	goto ECCKeyStrength
::)

ECHO.
:ECCCurveType
ECHO 1.  SECP112R1
ECHO 2.  SECP112R2
ECHO 3.  SECP128R1
ECHO 4.  SECP128R2
ECHO 5.  SECP160K1
ECHO 6.  SECP160R1
ECHO 7.  SECP160R2
ECHO 8.  SECP192K1
ECHO 9.  SECP224K1
ECHO 10. SECP224R1
ECHO 11. SECP256K1
ECHO 12. SECP384R1
ECHO 13. SECP521R1
ECHO 14. PRIME192V1
ECHO 15. PRIME192V2
ECHO 16. PRIME192V3
ECHO 17. PRIME239V1
ECHO 18. PRIME239V2
ECHO 19. PRIME239V3
ECHO 20. PRIME256V1/SECP256R1 
ECHO 21. SECT113R1
ECHO 22. SECT113R2
ECHO 23. SECT131R1
ECHO 24. SECT131R2
ECHO 25. SECT163K1
ECHO 26. SECT163R1
ECHO 27. SECT163R2
ECHO 28. SECT193R1
ECHO 29. SECT193R2
ECHO 30. SECT233K1
ECHO 31. SECT233R1
ECHO 32. SECT239K1
ECHO 33. SECT283K1
ECHO 34. SECT283R1
ECHO 35. SECT409K1
ECHO 36. SECT409R1
ECHO 37. SECT571K1
ECHO 38. SECT571R1
ECHO 39. C2PNB163V1
ECHO 40. C2PNB163V2
ECHO 41. C2PNB163V3
ECHO 42. C2PNB176V1
ECHO 43. C2TNB191V1
ECHO 44. C2TNB191V2
ECHO 45. C2TNB191V3
ECHO 46. C2PNB208W1
ECHO 47. C2TNB239V1
ECHO 48. C2TNB239V2
ECHO 49. C2TNB239V3
ECHO 50. C2PNB272W1

set curveType=none
set input=
set /p input=Enter the number for your choice of Curve Type:
if '%input%'=='1' set curveType=secp112r1
if '%input%'=='2' set curveType=secp112r2
if '%input%'=='3' set curveType=secp128r1
if '%input%'=='4' set curveType=secp128r2
if '%input%'=='5' set curveType=secp160k1
if '%input%'=='6' set curveType=secp160r1
if '%input%'=='7' set curveType=secp160r2
if '%input%'=='8' set curveType=secp192k1
if '%input%'=='9' set curveType=secp224k1
if '%input%'=='10' set curveType=secp224r1
if '%input%'=='11' set curveType=secp256k1
if '%input%'=='12' set curveType=secp384r1
if '%input%'=='13' set curveType=secp521r1
if '%input%'=='14' set curveType=prime192v1
if '%input%'=='15' set curveType=prime192v2
if '%input%'=='16' set curveType=prime192v3
if '%input%'=='17' set curveType=prime239v1
if '%input%'=='18' set curveType=prime239v2
if '%input%'=='19' set curveType=prime239v3
if '%input%'=='20' set curveType=prime256v1
if '%input%'=='21' set curveType=sect113r1
if '%input%'=='22' set curveType=sect113r2
if '%input%'=='23' set curveType=sect131r1
if '%input%'=='24' set curveType=sect131r2
if '%input%'=='25' set curveType=sect163k1
if '%input%'=='26' set curveType=sect163r1
if '%input%'=='27' set curveType=sect163r2
if '%input%'=='28' set curveType=sect193r1
if '%input%'=='29' set curveType=sect193r2
if '%input%'=='30' set curveType=sect233k1
if '%input%'=='31' set curveType=sect233r1
if '%input%'=='32' set curveType=sect239k1
if '%input%'=='33' set curveType=sect283k1
if '%input%'=='34' set curveType=sect283r1
if '%input%'=='35' set curveType=sect409k1
if '%input%'=='36' set curveType=sect409r1
if '%input%'=='37' set curveType=sect571k1
if '%input%'=='38' set curveType=sect571r1
if '%input%'=='39' set curveType=c2pnb163v1
if '%input%'=='40' set curveType=c2pnb163v2
if '%input%'=='41' set curveType=c2pnb163v3
if '%input%'=='42' set curveType=c2pnb176v1
if '%input%'=='43' set curveType=c2tnb191v1
if '%input%'=='44' set curveType=c2tnb191v2
if '%input%'=='45' set curveType=c2tnb191v3
if '%input%'=='46' set curveType=c2pnb208w1
if '%input%'=='47' set curveType=c2tnb239v1
if '%input%'=='48' set curveType=c2tnb239v2
if '%input%'=='49' set curveType=c2tnb239v3
if '%input%'=='50' set curveType=c2pnb272w1

if '%curveType%'=='none' ( 
	ECHO "%input%" is not supported, please try again!
	ECHO.
	goto ECCCurveType
)

ECHO.
:ECCKeyExtension
ECHO 1. pem
ECHO 2. der
set keyFormat=none
set input=
set /p input=Enter the number for your choice of Key Format:
if '%input%'=='1' set keyFormat=pem
if '%input%'=='2' set keyFormat=der
if '%keyFormat%'=='none' ( 
	ECHO "%input%" is not supported, please try again!
	ECHO.
	goto ECCKeyExtension
)

:: Get File name of ECC key
ECHO.
set keyFileName=
set /P keyFileName=Enter key file name with extension(default: PrivKey.pem):
if not defined keyFileName set keyFileName=PrivKey.pem

:: Generate key pair
openssl ecparam -genkey -name %curveType% -noout -out %keyFileName%.pem

:: Separate out the public key
openssl ec -in %keyFileName%.pem -pubout -out %keyFileName%.pub

for /f "delims=." %%a in ("%keyFileName%") do set fileName=%%a
set pubKeyFileName=%fileName%_pubKey.pub

if '%keyFormat%'=='pem' (
	::Rename the public key and private key files
	REN %keyFileName%.pem %keyFileName%
	REN %keyFileName%.pub %pubKeyFileName%
	goto cleanUp
)

if '%keyFormat%'=='der' (
	::Convert PEM to DER format
	openssl ec -in %keyFileName%.pem -outform DER -out %keyFileName%.der
	openssl ec -in %keyFileName%.pub -pubin -outform DER -out %pubKeyFileName%
	REN %keyFileName%.der %keyFileName%
	del %keyFileName%.pem
	del %keyFileName%.pub
	goto cleanUp
)

:cleanUp
move %keyFileName% ../../../../Build_Output/Certificate
move %pubKeyFileName% ../../../../Build_Output/Certificate

ECHO.
ECHO NOTE: Output Private and Public keys are located at %certDir% folder.
ECHO.
goto keyFin

:keyFin
pause