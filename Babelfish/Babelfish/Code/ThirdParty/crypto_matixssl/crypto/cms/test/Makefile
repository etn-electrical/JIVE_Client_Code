#
#	Makefile for CMS testing
#
#	Copyright (c) 2013-2016 INSIDE Secure Corporation. All Rights Reserved.
#

MATRIXSSL_ROOT:=../../..

SRC:=cmsTest.c

include $(MATRIXSSL_ROOT)/common.mk

STATICS     = ../../../core/libcore_s$(A) ../../../crypto/libcrypt_s$(A) ../libcms_s$(A)

# Generated files
TEST_EXE    = cmsTest$(E)
TEST_OBJS 	= cmsTest$(O)

CFLAGS  += -I../.. -I../../.. -I../../../testkeys -I../../../sampleCerts
LIBZ = -lz
LDFLAGS +=
LDADD += $(LIBZ)

# Linked files
STATICS:=$(MATRIXSSL_ROOT)/crypto/libcrypt_s.a $(MATRIXSSL_ROOT)/core/libcore_s.a ../libcms_s.a

all: compile

compile: $(OBJS) $(TEST_EXE)

# Additional Dependencies
$(OBJS): $(MATRIXSSL_ROOT)/common.mk Makefile $(wildcard *.h)

$(TEST_EXE): $(SRC:.c=.o) $(STATICS)
	$(CC) -o $@ $^ $(LDFLAGS) $(STATICS) $(LDADD)

#
#	Clean up all generated files
#
clean:
	rm -f $(TEST_EXE) $(TEST_OBJS) mcms*.der aed.bin
