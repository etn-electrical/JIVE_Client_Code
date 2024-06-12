#
#	Makefile for unit testing applications.
#
#	Copyright (c) 2018 INSIDE Secure Corporation. All Rights Reserved.
#
#

# Environment autodetection (to get AES-NI correctly).
CORE_DIR=../../../core
include $(CORE_DIR)/makefiles/get_CCARCH.mk

# Guess availability of AES-NI on x86/i686 targets.
# Can be overriden by specifically setting CFLAGS_ENABLE_AES_X86.
ifneq (,$(findstring x86_64-,$(CCARCH)))
CFLAGS_ENABLE_AES_X86=-maes
endif

ifneq (,$(findstring i586-,$(CCARCH)))
CFLAGS_ENABLE_AES_X86=-maes
endif

ifneq (,$(findstring i686-,$(CCARCH)))
CFLAGS_ENABLE_AES_X86=-maes
endif

# API spec for software using psCrypto API only.
# Supplement for rules in core/makefiles/rules.mk.
psCrypto_API_CPPFLAGS=-I$(PSCRYPTO_PATH) $(CL_API_CPPFLAGS) $(CFLAGS_ENABLE_AES_X86)
psCrypto_API_LIBADD=$(PSCRYPTO_PATH)/libcrypt_s.a $(core_API_LIBADD) $(DL) $(PTHREAD) $(LINKER_ADD_GC)
LINKER_ADD_GC=-Wl,--gc-sections

# Underlying APIs to include in test
API=psCrypto core

ifeq ($(wildcard Makefile),)

# Source library files (detect automatically)
LIB_C:=$(wildcard lib_*.c)
LIB_CC:=$(wildcard lib_*.cc)
ifneq '$(LIB_C) $(LIB_CC)' ' '
LIB_TEST_COMMON=libtest_common_s.a
noinst_LIBRARIES=libtest_common_s.a
libtest_common_s_a_SOURCES=$(LIB_C) $(LIB_CC)
lib_catch_main.o: CXXFLAGS=-O0 $(FLAGS_ARCHITECTURE_VARIANT) $(CFLAGS_ARCHITECTURE_VARIANT) # Faster compilation of catch main program
libtest_common_s_a_API=$(API) testsupp
endif

# Source files (detect automatically)
# All sources depend on and include libtest_common_s.a
TESTS_C:=$(patsubst %.c,%,$(wildcard test_*.c))
TESTS_CC:=$(patsubst %.cc,%,$(wildcard test_*.cc))
EXTRA_PROGRAMS = $(TESTS_C) $(TESTS_CC)
$(foreach test,$(TESTS_C),$(eval $(test)_SOURCES = $(test:%=%.c)))
$(foreach test,$(TESTS_C),$(eval $(test)_CFLAGS = -g))
$(foreach test,$(TESTS_C),$(eval $(test)_LDFLAGS = -g))
$(foreach test,$(TESTS_C),$(eval $(test)_API = test_common $(API) testsupp core))
ifneq '$(LIB_C) $(LIB_CC)' ' '
$(foreach test,$(TESTS_C),$(eval $(test): libtest_common_s.a))
endif
$(foreach test,$(TESTS_CC),$(eval $(test)_SOURCES = $(test:%=%.cc)))
$(foreach test,$(TESTS_CC),$(eval $(test)_CPPFLAGS = -g))
$(foreach test,$(TESTS_CC),$(eval $(test)_LDFLAGS = -g))
$(foreach test,$(TESTS_CC),$(eval $(test)_API = test_common $(API) testsupp core))
ifneq '$(LIB_C) $(LIB_CC)' ' '
$(foreach test,$(TESTS_CC),$(eval $(test): libtest_common_s.a))
endif
CFLAGS_STANDARD=-std=c99

# API spec for common testing aid library.
test_common_API_CFLAGS:=
test_common_API_CXXFLAGS:=
test_common_API_LIBADD=$(LIB_TEST_COMMON)

# Construct build rules using makefiles/rules.mk
CORE_DIR=../../../core
include $(CORE_DIR)/makefiles/rules.mk

$(foreach test,$(TESTS_C),$(eval $(test:%=check_%): $(test);	./$(test)))
$(foreach test,$(TESTS_CC),$(eval $(test:%=check_%): $(test);	./$(test)))

check: $(TESTS_C:%=check_%) $(TESTS_CC:%=check_%)

else

# Makefile exists. Let it override this build file.
# (Note: This modifies priority of make built-in lookup priority.)
# This rule will allow the user to override this GNUmakefile with
# build rules from eg. cmake.

include Makefile

endif
