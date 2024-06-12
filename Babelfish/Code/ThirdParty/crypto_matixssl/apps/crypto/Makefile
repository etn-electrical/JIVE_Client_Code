#
#   Makefile for key/certificate generation apps
#   'make' builds debug (Default).
#   'make gold' builds optimized.
#
#	Copyright (c) 2013-2016 INSIDE Secure Corporation. All Rights Reserved.
#


MATRIXSSL_ROOT:=../..
CERTGEN_SRC:=certgen.c
DERTOMEM_SRC:=dertomem.c
PEMTOMEM_SRC:=pemtomem.c
CERTREQ_SRC:=certrequest.c
ECKEYGEN_SRC:=eckeygen.c
RSAKEYGEN_SRC:=rsakeygen.c
OCSP_SRC:=ocsp.c
DERPARSE_SRC:=derparse.c
VALIDATE_CSR_SRC:=validate_csr.c

SRC:=$(CERTGEN_SRC) $(DERTOMEM_SRC) $(PEMTOMEM_SRC) $(CERTREQ_SRC) $(ECKEYGEN_SRC) $(RSAKEYGEN_SRC) $(OCSP_SRC) $(DERPARSE_SRC) $(VALIDATE_CSR_SRC)

include $(MATRIXSSL_ROOT)/common.mk

# Generated files
CERTGEN_EXE:=matrixCertGen$(E)
DERTOMEM_EXE:=matrixDer2Mem$(E)
PEMTOMEM_EXE:=matrixPem2Mem$(E)
CERTREQ_EXE:=matrixCertReq$(E)
ECKEYGEN_EXE:=matrixECkeygen$(E)
RSAKEYGEN_EXE:=matrixRSAkeygen$(E)
OCSP_EXE:=matrixOCSP$(E)
DERPARSE_EXE:=matrixDerParse$(E)
VALIDATE_CSR_EXE:=matrixValidateCsr$(E)

# Linked files
STATIC:=\
	$(MATRIXSSL_ROOT)/crypto/libcrypt_s.a \
	$(MATRIXSSL_ROOT)/core/libcore_s.a

#ifdef USE_ROT_CRYPTO
STATIC+=$(LIBDRIVER_VAL_UP_PATH)
#endif

all: compile

compile: $(OBJS) $(CERTGEN_EXE) $(DERTOMEM_EXE) $(PEMTOMEM_EXE) $(CERTREQ_EXE) $(ECKEYGEN_EXE) $(RSAKEYGEN_EXE) $(OCSP_EXE) $(DERPARSE_EXE) $(VALIDATE_CSR_EXE)

# Additional Dependencies
$(OBJS): $(MATRIXSSL_ROOT)/common.mk Makefile $(wildcard *.h)

$(CERTGEN_EXE): $(CERTGEN_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DERTOMEM_EXE): $(DERTOMEM_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(PEMTOMEM_EXE): $(PEMTOMEM_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CERTREQ_EXE): $(CERTREQ_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(ECKEYGEN_EXE): $(ECKEYGEN_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(RSAKEYGEN_EXE): $(RSAKEYGEN_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OCSP_EXE) : $(OCSP_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DERPARSE_EXE) : $(DERPARSE_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

$(VALIDATE_CSR_EXE) : $(VALIDATE_CSR_SRC:.c=.o) $(STATIC)
	$(CC) -o $@ $^ $(LDFLAGS)

certgen_simple :
	g++ \
	-std=c++11 \
	-I$(MATRIXSSL_ROOT)/crypto \
	-I$(MATRIXSSL_ROOT)/../core/include \
	-I$(MATRIXSSL_ROOT)/../core/osdep/include \
	-I$(MATRIXSSL_ROOT)/../core/config \
	-O0 -g \
	-o certgen_simple certgen_simple.cc $(STATIC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(CERTGEN_EXE) $(DERTOMEM_EXE) $(PEMTOMEM_EXE) $(CERTREQ_EXE) $(ECKEYGEN_EXE) $(RSAKEYGEN_EXE) $(OCSP_EXE) $(VALIDATE_CSR_EXE)

