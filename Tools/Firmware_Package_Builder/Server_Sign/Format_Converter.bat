
set CA_cert="Eaton_Commercial_Root_CA_1"
set CA_cert_extn=crt
set intermediate_cert="Eaton_Firmware_Signing_MA_Issuing_CA_1"
set intermediate_cert_extn=crt
set product_cert="FW-Smartbreaker2-CMS"
set product_cert_extn=crt


REM extact Public key from Root certificate
openssl x509 -pubkey -noout -inform DER -in ./certs/%CA_cert%.%CA_cert_extn%  > ./public/%CA_cert%.pub.pem

REM convert public PEM key format to DER Key format
openssl ec -inform pem -pubin -in ./public/%CA_cert%.pub.pem -pubout -outform DER -out ./public/%CA_cert%.pub.der



REM extact Public key from Intermediate certificate
::openssl x509 -pubkey -noout -inform DER -in ./certs/%intermediate_cert%.%intermediate_cert_extn%  > ./public/%intermediate_cert%.pub.pem

REM convert public PEM key format to DER Key format
::openssl ec -inform pem -pubin -in ./public/%intermediate_cert%.pub.pem -pubout -outform DER -out ./public/%intermediate_cert%.pub.der


REM ################## CAUTION -- product certifcate provided by COE sign Server is in PEM format ######################

REM convert product certificate PEM to DER format
openssl x509 -outform der -in ./certs/%product_cert%.%product_cert_extn% -out ./certs/%product_cert%.der


REM extact Public key from Product certificate
openssl x509 -pubkey -noout -inform PEM -in ./certs/%product_cert%.%product_cert_extn%  > ./public/%product_cert%.pub.pem

REM convert public PEM key format to DER Key format
openssl ec -inform pem -pubin -in ./public/%product_cert%.pub.pem -pubout -outform DER -out ./public/%product_cert%.pub.der

REM Delete temporary files i.e public PEM key format files
cd ./public
del %CA_cert%.pub.pem
del %product_cert%.pub.pem
cd ../