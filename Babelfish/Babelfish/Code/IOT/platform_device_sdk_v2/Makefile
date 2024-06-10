#######################################
# Copyright Eaton Corporation
######################################

ifndef TOP
TOP:=$(CURDIR)/../../..
export TOP
endif

ifndef NO_MAKEFILE_INCLUDE
include $(TOP)/Makefile.include
endif

.PHONY: all install clean mrproper doxygen uncrustify
all clean mrproper doxygen:
	$(MAKE) -C make $@

# Additionally for the install, copy our headers
install:
	$(MAKE) -C make $@
	mkdir -p $(TOOLKIT_DEST)/include/etn_iot_sdk
	cp -uv include/*.h $(TOOLKIT_DEST)/include/etn_iot_sdk

# Note that both PX Green and Red share this library, and have different
# styles for uncrustify, but we'll use the Red style by default.
uncrustify:
	find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec uncrustify -l cpp -c ../../uncrustify.cfg --no-backup {} \;

# These files not logically guaranteed to exist but they are never generated.
# Don't waste time with implicit rules.
ifndef NO_MAKEFILE_INCLUDE
Makefile $(TOP)/Makefile.include:: ;
endif
