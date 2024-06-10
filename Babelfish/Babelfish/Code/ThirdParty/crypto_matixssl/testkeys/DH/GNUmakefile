#/*****************************************************************************
#* Copyright (c) 2018 INSIDE Secure Oy. All Rights Reserved.
#*
#* This confidential and proprietary software may be used only as authorized
#* by a licensing agreement from INSIDE Secure.
#*
#* The entire notice above must be reproduced on all authorized copies that
#* may only be made to the extent permitted by a licensing agreement from
#* INSIDE Secure.
#*****************************************************************************/

# Note: This GNUmakefile is usually not needed, as the pem and header files
# are usually prebuilt.

build-params: ffdhe8192 ffdhe6144 ffdhe4096 ffdhe3072 ffdhe2048

.PHONY: ffdhe8192 ffdhe6144 ffdhe4096 ffdhe3072 ffdhe2048

ffdhe2048: ffdhe2048_DH_PARAMS.pem ffdhe2048_DH_PARAMS.h
ffdhe3072: ffdhe3072_DH_PARAMS.pem ffdhe3072_DH_PARAMS.h
ffdhe4096: ffdhe4096_DH_PARAMS.pem ffdhe4096_DH_PARAMS.h
ffdhe6144: ffdhe6144_DH_PARAMS.pem ffdhe6144_DH_PARAMS.h
ffdhe8192: ffdhe8192_DH_PARAMS.pem ffdhe8192_DH_PARAMS.h

%.der : %.sequence
	openssl asn1parse -genconf $< -noout -out $@

%DH_PARAMS.pem : %DH_PARAMS.der
	openssl dhparam -inform DER -in $< -out $@

%DH_PARAMS.h : %DH_PARAMS.pem %DH_PARAMS.der
	@echo "/**" >$@
	@echo ' *      @file    $@' >>$@
	@echo ' *      @version $$Format:%h%d$$' >>$@
	@echo ' *' >>$@
	@echo ' * Automatically generated header file for $<.' >>$@
	@echo ' */' >>$@
	@echo '' >>$@
	@echo >>$@ "/*****************************************************************************"
	@echo >>$@ " * Copyright (c) `date +%Y` INSIDE Secure Oy. All Rights Reserved."
	@echo >>$@ " *"
	@echo >>$@ " * This confidential and proprietary software may be used only as authorized"
	@echo >>$@ " * by a licensing agreement from INSIDE Secure."
	@echo >>$@ " *"
	@echo >>$@ " * The entire notice above must be reproduced on all authorized copies that"
	@echo >>$@ " * may only be made to the extent permitted by a licensing agreement from"
	@echo >>$@ " * INSIDE Secure."
	@echo >>$@ " *****************************************************************************/"
	@echo '' >>$@
	@echo '/******************************************************************************/' >>$@
	@echo '/* '  >>$@
	@echo '   In-Memory version of the PEM file $@' >>$@
	openssl dhparam -in $< -text >>$@
	@echo '*/'  >>$@
	@echo '' >>$@
	@echo '#define $(@:%.h=%)_SIZE ((int) sizeof($(@:%.h=%)))' >>$@
	@echo 'static const unsigned char $(@:%.h=%)[] =' >>$@
	@echo '{' >>$@
	xxd -i < $(@:%.h=%.der) >>$@
	@echo '};' >>$@
