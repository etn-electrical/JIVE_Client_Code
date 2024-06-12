#
#   Makefile for example external module implementing the psExt API.
#   The example module is useful for testing the external client
#   authentication feature with sslTest and the demo client program.
#
#   Copyright (c) 2016 INSIDE Secure Corporation. All Rights Reserved.
#

MATRIXSSL_ROOT:=../..

SRC:=psExtWrapper.c exampleExtCvSigModule.c
STATIC:=libpsext_example.a

include $(MATRIXSSL_ROOT)/common.mk

all: compile

compile: $(OBJS) $(STATIC)

# Additional Dependencies
$(OBJS): $(MATRIXSSL_ROOT)/common.mk Makefile $(MATRIXSSL_ROOT)/matrixssl/psExt.h

$(STATIC): $(OBJS)
	$(AR) -rcu $@ $^

clean:
	rm -f $(STATIC) $(OBJS)
