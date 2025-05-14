# Makefile for csp library for UNIVERSITY OF PALERMO LABSAT

toolchain=/c/ISIS/application/toolchain/
obcdir=/c/ISIS/ISIS-OBC
projectdir=/c/ISIS/workspace/labsat-obc-master

INCLUDEDIRS=-I$(obcdir)/hal/hal/include -I$(obcdir)/hal/hcc/include -I$(obcdir)/hal/at91/include -I$(obcdir)/hal/freertos/include -I$(obcdir)/mission-support/mission-support/include -I$(obcdir)/satellite-subsystems/satellite-subsystems/include -I$(projectdir)/include -I$(projectdir)/csp-include
INCLUDEDIRS+=-I$(obcdir)/hal/freertos/include/freertos

# not including this define -D__ASSEMBLY__ 
DEFINES=-Dsdram -Dat91sam9g20 -DBASE_REVISION_NUMBER=1 -DBASE_REVISION_HASH_SHORT=1rs -DBASE_REVISION_HASH=1r

GCC=$(toolchain)/bin/arm-none-eabi-gcc

CFLAGS=--sysroot=$(toolchain) -mcpu=arm926ej-s -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-pointer-sign -Wno-format -std=c99

EXTRAFLAGS=

CMD=$(GCC) $(CFLAGS) $(INCLUDEDIRS) $(DEFINES)

OBJS=csp_bridge.o csp_buffer.o csp_conn.o csp_crc32.o csp_debug.o csp_dedup.o csp_hex_dump.o csp_id.o csp_iflist.o csp_init.o csp_io.o csp_port.o csp_promisc.o csp_qfifo.o csp_rdp.o csp_rdp_queue.o csp_route.o csp_rtable_cidr.o csp_services.o csp_service_handler.o csp_sfp.o arch/freertos/csp_clock.o arch/freertos/csp_queue.o arch/freertos/csp_semaphore.o arch/freertos/csp_system.o arch/freertos/csp_time.o atomics/atomics_freertos_gcc.o crypto/csp_hmac.o crypto/csp_sha1.o drivers/usart/usart_kiss.o interfaces/csp_if_i2c.o interfaces/csp_if_kiss.o interfaces/csp_if_lo.o interfaces/csp_if_tun.o
# excluded: csp_rtable_stdio.o csp_yaml.o 

all: debug

libcsp.a: $(OBJS)
	ar rcs $@ $^

libcspD.a: $(OBJS)
	ar rcs $@ $^

release: libcsp.a
release: EXTRAFLAGS+=-Os

debug: libcspD.a
debug: EXTRAFLAGS+=-O0 -g3 -DTRACE_LEVEL=5 -DDEBUG=1 

clean:
	rm -f *.[oa]

cleanobjects:
	rm -f *.o

%.o: %.c
	$(CMD) $(EXTRAFLAGS) -o $@ -c $^
