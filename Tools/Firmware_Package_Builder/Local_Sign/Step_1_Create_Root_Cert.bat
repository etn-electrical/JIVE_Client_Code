:: Utility to Generate Root Private Key and elf signed certificate
@ECHO off

set sn=8
set DN="/O=Eaton/CN=Local_Root-CA_1"
set curve=prime256v1
set format=pem
set key_extn=pem
set cert_extn=pem



if not exist ./private/ca.key.%key_extn% (

REM Create keypair file
openssl genpkey -algorithm ec -pkeyopt ec_paramgen_curve:%curve% -outform %format% -pkeyopt ec_param_enc:named_curve -out ./private/ca.key.%key_extn%
ECHO Root Key ./private/ca.key.%key_extn%

) else (
	ECHO Root Key ./private/ca.key.%key_extn% already exists
)

REM chmod 400 ./ca.key.pem
REM openssl rand -hex %sn% > ./ca_serial

openssl req -config ./openssl-root.cnf -keyform %format% -outform %format% -key ./private/ca.key.%key_extn% -subj %DN% -new -x509 -days 9125 -sha256 -extensions v3_ca -out ./certs/ca.cert.%cert_extn%
ECHO Self signed (Root) Certificate ./certs/ca.cert.%cert_extn%

REM extact Public key from Root certificate (to be used in signature verification process by device, also bootloader will have this copy dumped)
openssl x509 -pubkey -noout -in ./certs/ca.cert.%cert_extn% > ./public/ca.pub.%key_extn%
REM convert public PEM key format to DER Key format
openssl ec -inform %format% -pubin -in ./public/ca.pub.%key_extn% -pubout -outform DER -out ./public/ca.pub.der

pause