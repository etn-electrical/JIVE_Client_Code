
set infile=%1
SETLOCAL ENABLEDELAYEDEXPANSION

if '%infile%'=='' set infile=RTK_Example.bin.p7b

REM run the following and get the last bit position
REM set the value to last_bit_pos
REM openssl asn1parse -inform DER -in %infile%
FOR /F "tokens=1,3 delims=:" %%G IN ('openssl asn1parse -inform DER -in %infile%') DO (
SET _test=%%H
SET "_oc=OCTET STRING "
REM ECHO !_test!
SET _name=!_test:~1,12! 
REM ECHO '!_name!'
REM ECHO '!_oc!'
	IF '!_oc!' == '!_name!' (
		ECHO Octet string found
		ECHO '%%G' '%%H'
		::remove the leading space from the Address
		FOR /F "tokens=*" %%s in ("%%G") do set _ADDRESS=%%s
		
	) ELSE (
		ECHO Not Octet string
		ECHO '%%G' '%%H'
	)
)

ECHO !_ADDRESS!
set last_bin_pos=!_ADDRESS!

 
if !_ADDRESS!=='' (
openssl asn1parse -inform DER -in %infile%
ECHO .
ECHO ############################################################################################
ECHO Enter the left most address value from the above last row which has "OCTET STRING" mentioned
ECHO For Example in below sample snippet the address value should be 3010
ECHO 3000:d=6  hl=2 l=   8 prim: OBJECT        :ecdsa-with-SHA256
ECHO 3010:d=5  hl=2 l=  71 prim: OCTET STRING  [HEX DUMP]:3045022028C745833B34E30DD82DD56ADFC8DE8184D...
ECHO 3083:d=3  hl=2 l=   0 prim: EOC
ECHO 3085:d=2  hl=2 l=   0 prim: EOC
ECHO .
ECHO .
set /p last_bit_pos=address:
openssl asn1parse -inform DER -in %infile% -out %infile%.sig -noout -strparse %last_bit_pos%
) else (
openssl asn1parse -inform DER -in %infile% -out %infile%.sig -noout -strparse !_ADDRESS!
)

