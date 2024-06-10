#
#	Makefile for CMS testing
#
#	Copyright (c) 2013-2016 INSIDE Secure Corporation. All Rights Reserved.
#

include paths.mk

TEST_PROGRAMS = cmsut
cmsut_SOURCES = cmsut.cc

# Extraneous include paths and flags
LOCAL_CPPFLAGS = -I$(MATRIXSSL_ROOT) -I$(MATRIXSSL_ROOT)/crypto
LOCAL_LDADD = $(LIBCMS_S_A) $(LIBCRYPT_S_A) $(LIBCORE_S_A) $(LIBZ)
include $(use_common_mk)

# List resources to use in testing (these are in data directory).
PEM_FILES := EC/256_EC.pem EC/384_EC.pem RSA/2048_RSA.pem RSA/2048_RSA_KEY.pem \
crls/intermediate.crl.pem pkcs7_crl_basic.pem messageSmall.txt.encrypted.pem
ECC_PEM_FILES := EC/256_EC_KEY.pem EC/384_EC_KEY.pem
VERBATIM_FILES := messageMedium.txt messageSmall.txt signed.bin \
authenv_ext.data.bin
include $(use_prepkg_mk)

# Perform testing with testsupp
include $(use_testsupp_mk)

# Get generic rules file
include $(use_rules_mk)
