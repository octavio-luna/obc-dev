# Definitions for Labsat Makefile (to use without Eclipse)

toolchain=../../../application/toolchain/
obcdir=../../../ISIS-OBC
projectdir=../../../workspace/labsat-obc-master

SRCDIRS=$(projectdir)/src $(projectdir)/csp-src

INCLUDEDIRS=-I$(obcdir)/hal/hal/include -I$(obcdir)/hal/hcc/include -I$(obcdir)/hal/at91/include -I$(obcdir)/hal/freertos/include -I$(obcdir)/mission-support/mission-support/include -I$(obcdir)/satellite-subsystems/satellite-subsystems/include -I$(projectdir)/include -I$(projectdir)/csp-include -I$(obcdir)/hal/freertos/include/freertos

LIBDIRS=-L$(obcdir)/hal/at91/lib -L$(obcdir)/hal/freertos/lib -L$(obcdir)/hal/hal/lib -L$(obcdir)/hal/hcc/lib -L$(obcdir)/mission-support/mission-support/lib -L$(obcdir)/satellite-subsystems/satellite-subsystems/lib -L$(projectdir)/csp-src

# not including this define -D__ASSEMBLY__ 
DEFINES=-Dsdram -Dat91sam9g20 -DBASE_REVISION_NUMBER=1 -DBASE_REVISION_HASH_SHORT=1rs -DBASE_REVISION_HASH=1r

GCC=$(toolchain)/bin/arm-none-eabi-gcc

CFLAGS=--sysroot=$(toolchain) -mcpu=arm926ej-s -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-pointer-sign -Wno-format

LINKFLAGS=-Xlinker --gc-sections -Wl,-Map,labsat-obc-master.map

EXTRAFLAGS=


CMD=$(GCC) $(CFLAGS) $(INCLUDEDIRS) $(DEFINES)

OBJS=CSPManager.o LogManager.o PowerManager.o PowerManagerUart.o SDManager.o TimerManager.o main.o DevelTest.o

EXECS=fsw


all: debug

fsw: $(OBJS)
	$(CMD) $(EXTRAFLAGS) $(LINKFLAGS) -o $@ $(LIBDIRS) $^ $(LIBS)

release: $(EXECS)
release: EXTRAFLAGS+=-Os
release: LIBS+=-lAt91 -lFreeRTOS -lHCC -lHAL -lSatelliteSubsystems -lMissionSupport -lcsp

debug: $(EXECS)
debug: EXTRAFLAGS+=-O0 -g3 -DTRACE_LEVEL=5 -DDEBUG=1 
debug: LIBS=-lAt91D -lFreeRTOSD -lHCCD -lHALD -lSatelliteSubsystemsD -lMissionSupportD -lcspD

clean:
	rm -f *.o

%.o: %.c
	$(CMD) $(EXTRAFLAGS) -o $@ -c $<

