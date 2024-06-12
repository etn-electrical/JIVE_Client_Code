@ECHO off

set sn=8
set DN="/O=Eaton/CN=Local_Signing-CA_1_1"
set curve=prime256v1
set format=pem
set csr_extn=pem
set key_extn=pem
set cert_extn=pem


if not exist ./private/intermediate.key.%key_extn% (

REM Create keypair file
openssl genpkey -algorithm ec -pkeyopt ec_paramgen_curve:%curve% -outform %format% -pkeyopt ec_param_enc:named_curve -out ./private/intermediate.key.%key_extn%
ECHO Intermediate Key ./private/intermediate.key.%key_extn%

) else (
	ECHO Intermediate Key ./private/intermediate.key.%key_extn% already exists
)

REM Create the CSR
openssl req -config ./openssl-root.cnf -key ./private/intermediate.key.%key_extn% -keyform %format% -outform %format% -subj %DN% -new -sha256 -out ./csr/intermediate.csr.%csr_extn%

REM # Generate serial number (random)
openssl rand -hex %sn% > ./serial

REM Create Intermediate Certificate file
openssl ca -config ./openssl-root.cnf -days 3650 -extensions v3_intermediate_ca -notext -md sha256 -in ./csr/intermediate.csr.%csr_extn% -out ./certs/intermediate.cert.%cert_extn%


REM Verify Intermediate certificate using Root CA Certificate
openssl verify -CAfile ./certs/ca.cert.%cert_extn% ./certs/intermediate.cert.%cert_extn%

REM Convert PEM format Intermediate to DER format (to be used to insert IVK DER certificates into code sign info package )
openssl x509 -outform der -in ./certs/intermediate.cert.%cert_extn% -out ./certs/intermediate.cert.der


REM Create certificate chain file
CD certs
COPY /b intermediate.cert.%cert_extn%+ca.cert.%cert_extn% ca.chain.cert.%cert_extn%
cd..


pause

