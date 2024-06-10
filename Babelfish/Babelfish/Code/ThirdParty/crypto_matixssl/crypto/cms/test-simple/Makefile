# Simple CMS unit test.

MATRIXSSL_ROOT:=../../..
CORE_DIR=$(MATRIXSSL_ROOT)/../core
include $(MATRIXSSL_ROOT)/common.mk

CFLAGS  += -I./ -I../.. -I../../crypto -I$(CORE_DIR)/include/testsupp/thirdparty
CFLAGS+=-pthread # Core seems to require this.

STATIC:=../libcms_s.a
STATIC+=../../libcrypt_s.a
STATIC+=$(CORE_DIR)/libcore_s.a


cms-test-simple: ../libcms_s.a cms-test-simple.cc
	g++ $(CFLAGS) cms-test-simple.cc $(STATIC) -o cms-test-simple

compile: cms-test-simple

clean:
	rm -f cms-test-simple.o
	rm -f cms-test-simple
