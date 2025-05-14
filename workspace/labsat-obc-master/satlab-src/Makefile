# Makefile for csp library for UNIVERSITY OF PALERMO LABSAT

toolchain=../../../applications/toolchain/
obcdir=../../../ISIS-OBC
projectdir=../../../workspace/labsat-obc-master

INCLUDEDIRS=-I$(obcdir)/hal/hal/include -I$(obcdir)/hal/hcc/include -I$(obcdir)/hal/at91/include -I$(obcdir)/hal/freertos/include -I$(obcdir)/mission-support/mission-support/include -I$(obcdir)/satellite-subsystems/satellite-subsystems/include -I$(projectdir)/include -I$(projectdir)/csp-include -I$(projectdir)/satlab-include
INCLUDEDIRS+=-I$(obcdir)/hal/freertos/include/freertos

# not including this define -D__ASSEMBLY__ 
DEFINES=-Dsdram -Dat91sam9g20 -DBASE_REVISION_NUMBER=1 -DBASE_REVISION_HASH_SHORT=1rs -DBASE_REVISION_HASH=1r

GCC=$(toolchain)/bin/arm-none-eabi-gcc

CFLAGS=--sysroot=$(toolchain) -mcpu=arm926ej-s -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-pointer-sign -Wno-format -std=c99

EXTRAFLAGS=

CMD=$(GCC) $(CFLAGS) $(INCLUDEDIRS) $(DEFINES)

OBJS=bitops.o bounds.o client.o crc32.o error.o prop_client.o prop_client_helpers.o prop_query.o prop_spec.o srs4_boot.o srs4_shell.o

all: debug

libsatlab.a: $(OBJS)
	ar rcs $@ $^

libsatlabD.a: $(OBJS)
	ar rcs $@ $^

release: libsatlab.a
release: EXTRAFLAGS+=-Os

debug: libsatlabD.a
debug: EXTRAFLAGS+=-O0 -g3 -DTRACE_LEVEL=5 -DDEBUG=1 

clean:
	rm -f $(OBJS) lib*.a

cleanobjects:
	rm -f $(OBJS)

%.o: %.c
	$(CMD) $(EXTRAFLAGS) -o $@ -c $^
