ARM_ARCH=arm-none-eabi-
ARCH=

CFLAGS=-Wall -g -O0
#ARMFLAGS=-mcpu=cortex-m4
ARMFLAGS=

all: txt2hex chkwww httpsim fw.aeabi hams.aeabi server.aeabi
#all: http_test fw.aeabi hams.aeabi server.aeabi txt2hex chkwww httpsim

hams.o : hams.c hams.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 hams.c -o hams.o

server.o : server.c server.h fw.h hams.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 server.c -o server.o

http_test.o : http_test.c hams.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 http_test.c -o http_test.o

httpsim.o : httpsim.c server.h fw.h hams.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 httpsim.c -o $@

fw.o : fw.c fw.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 fw.c -o fw.o

md5.o : md5.c md5.h
	$(ARCH)gcc $(CFLAGS) -c -std=gnu11 md5.c -o md5.o

httpsim : hams.o httpsim.o server.o md5.o fw.o
	$(ARCH)gcc -Wall -o httpsim hams.o httpsim.o server.o md5.o fw.o

txt2hex: txt2hex.c md5.o fw.h
	$(ARCH)gcc -Wall -std=gnu11 -o txt2hex txt2hex.c md5.o

chkwww: chkwww.c md5.o fw.h
	$(ARCH)gcc -Wall -std=gnu11 -o chkwww chkwww.c md5.o

http_test : hams.o http_test.o
	$(ARCH)gcc -Wall -o http_test hams.o http_test.o

hams.aeabi : hams.c hams.h
	$(ARM_ARCH)gcc $(ARMFLAGS) -c -std=gnu11 hams.c -o hams.aeabi
	$(ARM_ARCH)size hams.aeabi

server.aeabi : server.c server.h fw.h hams.h
	$(ARM_ARCH)gcc $(ARMFLAGS) -c -std=gnu11 server.c -o server.aeabi
	$(ARM_ARCH)size server.aeabi

fw.aeabi : fw.c fw.h
	$(ARM_ARCH)gcc $(ARMFLAGS) -c -std=gnu11 fw.c -o fw.aeabi
	$(ARM_ARCH)size fw.aeabi

txt2hex: ltk_header_gen.c fw.h
	$(ARCH)gcc -Wall -std=gnu11 -o ltk_header_gen ltk_header_gen.c

clean:
	rm -f *.o *.aeabi http_test txt2hex chkwww httpsim
