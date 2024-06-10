#
#    paths.mk: Relative directory path information for make.
#
#    Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
#

PACKAGE=matrixssl
LOCAL_DIR=crypto/cms/testpp
LOCAL_HEADERS=$(wildcard )
MATRIXSSL_ROOT=../../..

# Convenience path to common Makefile
use_common_mk=$(MATRIXSSL_ROOT)/common.mk

# Optional extra path information file (e.g. versions)
-include paths-versions.mk
