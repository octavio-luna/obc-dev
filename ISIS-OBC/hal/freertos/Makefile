# Makefile for csp library for UNIVERSITY OF PALERMO LABSAT

toolchain=../../../applications/toolchain/
obcdir=../../../ISIS-OBC
projectdir=../../../workspace/labsat-obc-master

INCLUDEDIRS=-I$(obcdir)/hal/hal/include -I$(obcdir)/hal/hcc/include -I$(obcdir)/hal/at91/include -I$(obcdir)/hal/freertos/include -I$(obcdir)/mission-support/mission-support/include -I$(obcdir)/satellite-subsystems/satellite-subsystems/include -I$(projectdir)/include -I$(projectdir)/csp-include
INCLUDEDIRS+=-I$(obcdir)/hal/freertos/include/freertos

# not including this define -D__ASSEMBLY__ 
DEFINES=-Dsdram -Dat91sam9g20 -DBASE_REVISION_NUMBER=1 -DBASE_REVISION_HASH_SHORT=1rs -DBASE_REVISION_HASH=1r

GCC=$(toolchain)/bin/arm-none-eabi-gcc

CFLAGS=--sysroot=$(toolchain) -mcpu=arm926ej-s -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-pointer-sign -Wno-format -std=gnu99

EXTRAFLAGS=

CMD=$(GCC) $(CFLAGS) $(INCLUDEDIRS) $(DEFINES)

OBJS=./src/timers.o ./src/tasks.o ./src/queue.o ./src/list.o ./src/croutine.o ./src/portable/hooks.o ./src/portable/GCC/ARM9_AT91SAM9G20/port.o ./src/portable/MemMang/standardMemMang.o

all: debug

lib/libFreeRTOSalt.a: $(OBJS)
	ar rcs $@ $^

lib/libFreeRTOSaltD.a: $(OBJS)
	ar rcs $@ $^

release: lib/libFreeRTOSalt.a
release: EXTRAFLAGS+=-Os

debug: lib/libFreeRTOSaltD.a
debug: EXTRAFLAGS+=-O0 -g3 -DTRACE_LEVEL=5 -DDEBUG=1 

clean:
	rm -f $(OBJS) lib/libFreeRTOSalt*.a

cleanobjects:
	rm -f $(OBJS)

%.o: %.c
	$(CMD) $(EXTRAFLAGS) -o $@ -c $^
