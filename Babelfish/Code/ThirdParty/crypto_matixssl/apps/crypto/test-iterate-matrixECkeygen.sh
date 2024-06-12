#!/bin/bash

# Iterate over EC key + cert gen.
# Initial use was to catch the non-deterministic EC public key encoding bug,
# where leading zeros in coordinate OCTET STRING were skipped.

if [ $# != 2 ]; then
    echo "Usage: ${BASH_SOURCE} [curve] [num iterations]"
    echo "Where curve is one of secp192r1, secp256r1, secp384r1, secp521r1, etc."
    exit 1
fi

CURVE=$1
NUM_ITER=$2

if [ ! -e matrixECkeygen ]; then
    cd apps/crypto
    if [ ! -e matrixECkeygen]; then
        echo "Not found: matrixECkeygen"
    fi
fi

n=0
while [ $n -lt $NUM_ITER ]; do

    n=$((n + 1))
    echo "=== Iteration $n ==="

    rm -f key.pem cert.pem

    ./matrixECkeygen -out key.pem -curve $CURVE
    if [ $? != 0 ]; then
        echo "matrixECkeygen failed after $n runs"
        exit 1
    fi

    ./matrixCertGen -out cert.pem -key key.pem <<EOF
1
y
0
30
sha256
test
FI
Uusimaa
Helsinki
SafeZone
dev
EOF

    if [[ $? -ne 0 ]]; then
        echo "matrixCertGen failed after $n runs"
        exit 1
    fi

   if `openssl x509 -text -noout -in cert.pem |grep -q error`; then
       echo "openssl x509 error while parsing cert.pem on iteration $n"
       exit 1
   fi

done

echo "All ok after $n iterations"
