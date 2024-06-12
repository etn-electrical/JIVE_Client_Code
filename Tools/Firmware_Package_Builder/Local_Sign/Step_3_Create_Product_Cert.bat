@ECHO off

set sn=8
set DN="/O=Eaton/CN=Local_PXGreen_1_1_1"
set prod_name=PXGreen
set curve=prime256v1
set format=pem
set csr_extn=pem
set key_extn=pem
set cert_extn=pem


if not exist ./private/%prod_name%.key.%key_extn% (

REM Create keypair file
openssl genpkey -algorithm ec -pkeyopt ec_paramgen_curve:%curve% -outform %format% -pkeyopt ec_param_enc:named_curve -out ./private/%prod_name%.key.%key_extn%
ECHO %prod_name% Key ./private/%prod_name%.key.%key_extn%

) else (
	ECHO %prod_name% Key ./private/%prod_name%.key.%key_extn% already exists
)


REM Create the CSR 
openssl req -config ./openssl-intermediate.cnf -key ./private/%prod_name%.key.%key_extn% -subj %DN% -new -sha256 -out ./csr/%prod_name%.csr.%csr_extn%

REM generate serial number (random)
openssl rand -hex %sn% > ./serial

openssl ca -config ./openssl-intermediate.cnf -days 1095 -extensions server_cert -notext -md sha256 -in ./csr/%prod_name%.csr.%csr_extn% -out ./certs/%prod_name%.cert.%cert_extn%


REM Verify Intermediate certificate using Root CA Certificate
openssl verify -CAfile ./certs/ca.chain.cert.%cert_extn% ./certs/%prod_name%.cert.%cert_extn%

REM Convert PEM format Product Certificate to DER format (to be used to insert IVK and FVK DER certificates into code sign info package )
openssl x509 -outform der -in ./certs/%prod_name%.cert.%cert_extn% -out ./certs/%prod_name%.cert.der

REM extact Public key from product certificate (to be used in signature verification process - only for test purpose, PC utility)
openssl x509 -pubkey -noout -in ./certs/%prod_name%.cert.%cert_extn% > ./public/%prod_name%.pub.%key_extn%
REM convert public PEM key format to DER Key format
openssl ec -inform %format% -pubin -in ./public/%prod_name%.pub.%key_extn% -pubout -outform DER -out ./public/%prod_name%.pub.der


REM (partial chain verification - optional) Verify Intermediate certificate using Root CA Certificate
REM openssl verify -partial_chain -CAfile ./certs/intermediate.cert.%cert_extn% ./certs/%prod_name%.cert.%cert_extn%


REM Create certificate chain file 
CD certs
COPY /b %prod_name%.cert.%cert_extn%+intermediate.cert.%cert_extn%+ca.cert.%cert_extn% %prod_name%.chain.cert.%cert_extn%
cd..


pause