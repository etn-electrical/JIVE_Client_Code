##
# Export Makefile definitions. Build binary set of libraries and headers.
#
# Copyright (c) 2017 INSIDE Secure Corporation. All Rights Reserved.
#
#-------------------------------------------------------------------------------

ALL_DIRS:=

# Copy and remove files, allow missing
COPY_FILES=rsync -avzh --ignore-missing-args
REMOVE_FILES=rm -f

all: export

crypto/cms/libcms_s.a: # Ignored if not available.
	echo -n

../CL/libsafezone-sw-common.a: # Ignored if not available.
	echo -n

core/libcore_s.a: ../core/libcore_s.a
	cp $< $@

export/lib: crypto/cms/libcms_s.a crypto/libcrypt_s.a matrixssl/libssl_s.a \
	    core/libcore_s.a ../CL/libsafezone-sw-common.a
	mkdir --parents export/lib
	$(COPY_FILES) $^ export/lib/
	mkdir export/matrixssl
	$(COPY_FILES) export/lib/libssl_s.a export/matrixssl/ # Alias
	mkdir export/crypto
	$(COPY_FILES) export/lib/libcrypt_s.a export/crypto/ # Alias
	mkdir export/core
	$(COPY_FILES) export/lib/libcore_s.a export/core/ # Alias

export/apps/ssl: apps/ssl/Makefile.export apps/ssl/client.c apps/ssl/server.c apps/ssl/http.c apps/ssl/app.h
	mkdir --parents export/apps/ssl
	$(COPY_FILES) $^ export/apps/ssl/
	mv export/apps/ssl/Makefile.export export/apps/ssl/Makefile
	$(REMOVE_FILES) export/apps/ssl/""*~ export/apps/ssl/""*.o export/apps/ssl/""*.a
	mv export/apps/ssl/client.c export/apps/ssl/client.cc
	mv export/apps/ssl/server.c export/apps/ssl/server.cc
	mv export/apps/ssl/http.c export/apps/ssl/http.cc
	if [ -e matrixssl ]; then if $(MAKE) --directory=matrixssl parse-config | grep -q '#define USE_SERVER_SIDE_SSL';then echo "USE_SERVER_SIDE_SSL=1" >>export/apps/ssl/Makefile.config; else echo "USE_SERVER_SIDE_SSL=0" >>export/apps/ssl/Makefile.config;fi;fi
	if [ -e matrixssl ]; then if $(MAKE) --directory=matrixssl parse-config | grep -q '#define USE_CLIENT_SIDE_SSL';then echo "USE_CLIENT_SIDE_SSL=1" >>export/apps/ssl/Makefile.config; else echo "USE_CLIENT_SIDE_SSL=0" >>export/apps/ssl/Makefile.config;fi;fi

export/apps/common: apps/common
	mkdir --parents export/apps/common
	$(COPY_FILES) apps/common/""* export/apps/common/
	$(REMOVE_FILES) export/apps/common/""*~ export/apps/common/""*.o export/apps/common/""*.a
	mv export/apps/common/Makefile.export export/apps/common/Makefile
	mv export/apps/common/client_common.c export/apps/common/client_common.cc
	mv export/apps/common/clientconfig.c export/apps/common/clientconfig.cc
	mv export/apps/common/load_keys.c export/apps/common/load_keys.cc

vpath %.h ../core/config ../core/include ../core/osdep/include crypto \
          crypto/cms crypto-cl matrixssl

CORE_H=coreConfig.h coreApi.h
CRYPTO_H=cryptoApi.h cryptoConfig.h cryptolib.h cryptoCheck.h cryptoImpl.h \
ps_chacha20poly1305ietf.h
CRYPTO_CMS_H=matrixCms.h matrixCmsApi.h matrixCmsConfig.h
MATRIXSSL_H=matrixsslCheck.h matrixssllib.h psExt.h matrixsslNet.h \
matrixsslImpl.h matrixsslSocket.h matrixsslApi.h version.h matrixsslConfig.h

export/common.mk: common.mk.export
	cp $< $@
	mkdir --parents export/core
	mkdir --parents export/core/makefiles
	cp -r ../core/Makefile.inc export/core/
	cp -r ../core/makefiles/""* export/core/makefiles/
	mkdir --parents export/CL
	cp ../CL/use_cl.mk export/CL/

export/include/matrixssl: $(MATRIXSSL_H)
	mkdir --parents export/include/matrixssl
	echo 'This directory is to support #include "matrixssl/*.h"' $@/README.txt
	cp -r $^ export/include/matrixssl/

export/include/crypto: $(CRYPTO_H)
	mkdir --parents export/include/crypto
	echo 'This directory is to support #include "crypto/*.h"' $@/README.txt
	cp -r $^ export/include/crypto

SYMMETRIC_H=symmetric.h aes_aesni.h symmetric_libsodium.h aes_fips.h aes_matrix.h
export/include/internal/crypto/symmetric: $(addprefix crypto/symmetric/,$(SYMMETRIC_H))
	mkdir --parents $@ && cp -r $^ $@/

DIGEST_H=digest.h digest_fips.h digest_matrix.h digest_libsodium.h
export/include/internal/crypto/digest: $(addprefix crypto/digest/,$(DIGEST_H))
	mkdir --parents $@ && cp -r $^ $@/

ALL_DIRS+=export/include/internal/crypto/keyformat
KEYFORMAT_H=asn1.h x509.h
export/include/internal/crypto/keyformat: $(addprefix crypto/keyformat/,$(KEYFORMAT_H))
	mkdir --parents $@ && cp -r $^ $@/

define internal_export_dir
DIR=export/include/internal/$(1)
ALL_DIRS+=$$(DIR)
$$(DIR): $$(addprefix $(1)/,$(2))
	mkdir --parents $$@ && cp -r $$^ $$@/
endef

$(eval $(call internal_export_dir,crypto/math,pstm.h pstmnt.h pstm_str.h))
$(eval $(call internal_export_dir,crypto/pubkey,pubkey_fips.h pubkey.h pubkey_matrix.h))
$(eval $(call internal_export_dir,crypto/prng,prng.h))


CRYPTO_CL_H=clCommon.h clConfig.h symmetric/aesCl.h symmetric/des3Cl.h
export/include/internal/crypto-cl: $(addprefix crypto-cl/,$(CRYPTO_CL_H))
	mkdir --parents $@ && cp --parents $^ export/include/internal/

CL_H=cl_arc4.h cl_bu.h cl_chacha20poly1305ietf.h cl_config.h cl_constants.h \
cl_cryptopp_integer_base.h cl_der.h cl.h cl_key_format.h cl_pem.h cl_pk.h \
cl_platform.h cl_policy.h cl_rc2.h cl_rp.h cl_rv.h cls.h cl_sk.h cls_only.h \
cl_sysconf.h cl_types.h cl_user_config.h cl_va.h

export/include/CL: $(addprefix ../CL/include/,$(CL_H))
	mkdir --parents $@ && cp -r $^ $@/

ALL_DIRS+=export/testkeys
export/testkeys: testkeys
	cp -r $< $@

#Consider providing internal version of CL includes.
#mkdir --parents export/include/internal/CL
#cp -r export/include/CL/""*.h export/include/internal/CL

export/include: $(CORE_H) $(CRYPTO_H) $(MATRIXSSL_H)
	mkdir --parents export/include export/include/internal export/include/internal/sfzcl export/include/internal/private export/include/internal/crypto/layer
	cp -r ../core/include/""*.h export/include/internal/
	cp -r ../core/include/core export/include/core
	cp -r ../core/include/sfzcl/""*.h export/include/internal/sfzcl/
	cp -r ../core/include/private/""*.h export/include/internal/private/
	cp -r crypto/layer/layer.h export/include/internal/crypto/layer/
	cp -r $^ export/include/internal/

export/include/osdep: $(CORE_OSDEP_H)
	mkdir --parents export/include/osdep/include
	cp ../core/osdep/include/""*.h export/include/osdep/include

ALL_DIRS += \
export/include export/include/osdep export/lib export/apps/ssl export/apps/common export/include/matrixssl export/include/crypto \
export/include/internal/crypto/symmetric \
export/include/internal/crypto/digest \
export/include/internal/crypto-cl \
export/include/CL

$(warning $(ALL_DIRS))

export: $(ALL_DIRS) export/common.mk
	mv export/include/internal/""*Api.h export/include/
	mv export/include/internal/""*Config.h export/include/
	cp export/apps/ssl/Makefile.config export/apps/common/Makefile.config 

.PHONY: export $(ALL_DIRS)

clean:
	rm -rf export
