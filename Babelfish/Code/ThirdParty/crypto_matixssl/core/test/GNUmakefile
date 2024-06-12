#
#   Makefile for core testing
#
#   Copyright (c) 2013-2016 INSIDE Secure Corporation. All Rights Reserved.
#

ifeq ($(wildcard Makefile),)

EXTRA_PROGRAMS=test
test_SOURCES=test.c
test_API=core testsupp

ifneq '$(ENABLE_GLIB)' ''
test_CFLAGS+=`pkg-config --cflags glib-2.0` -DENABLE_GLIB=1
test_LDFLAGS+=`pkg-config --libs glib-2.0`
endif

CORE_DIR=../../core
include $(CORE_DIR)/makefiles/rules.mk

else

# Makefile exists. Let it override this build file.
# (Note: This modifies priority of make built-in lookup priority.)
# This rule will allow the user to override this GNUmakefile with
# build rules from eg. cmake.

include Makefile

endif
