#
#	Makefile for MatrixCMS Commercial product package
#   'make' builds optimized (-Os default).
#   'make debug' builds debug mode.
#
#	Copyright (c) 2013-2016 INSIDE Secure Corporation. All Rights Reserved.
#

MATRIXSSL_ROOT:=../..
include $(MATRIXSSL_ROOT)/common.mk

CMS_STATIC = libcms_s$(A)

CMS_OBJECTS = \
	./authEnvelopedDataCreate$(O) \
	./authEnvelopedDataParse$(O) \
	./cdCreate$(O) \
	./cdParse$(O) \
	./commonCms$(O) \
	./eeConfirm$(O) \
	./eeParse$(O) \
	./oidTables$(O) \
	./sdCreate$(O) \
	./sdParse$(O)

#
# Compile options
#
CFLAGS  += -I./ -I../.. -I../../crypto 

all: compile

compile: $(CMS_STATIC)

$(CMS_OBJECTS): *.h *.c 

#
# Build the static CMS library

include $(use_ar_mk)
$(CMS_STATIC): $(CMS_OBJECTS)
	$(AR) $(ARCOMMAND) $@ $^

#
#	Clean up all generated files
#
clean:
	rm -f $(CMS_OBJECTS) $(CMS_STATIC)

