#
# Created automatically from source MMP my  mkfmake.pl
#

PATH := $(SYMSDK_ROOT)/bin:$(SYMSDK_ROOT)/gcc/bin:$(PATH)
SDKLINK = $(SYMSDK_ROOT)/lib

LD_LIBDIRS = -L$(SYMSDK_ROOT)/gcc/arm-none-symbianelf/lib
LD_LIBDIRS := $(LD_LIBDIRS) -L$(SYMSDK_ROOT)/gcc/lib/gcc/arm-none-symbianelf/4.3.2


CROSS_SPEC ?= arm-none-symbianelf-
CC = $(CROSS_SPEC)gcc
LD = $(CROSS_SPEC)ld
AR = $(CROSS_SPEC)ar


ELF2E32_FLAGS =
ifneq ($(E32IMG_COMPRESSED), 1)

ELF2E32_FLAGS := $(ELF2E32_FLAGS) --uncompressed

endif

ifeq ($(CFG), deb)

all : __tmp __deb  deb_binaries 

else

all : __tmp __rel  rel_binaries 

endif

./resource/apps/qconsole.rsc : ./qconsole.rss ./resource/apps
	perl -S epocrc.pl -I $(SYMSDK_ROOT)/include -I .  -I ./resource/apps -I private/10003a3f/apps -I .. -I $(SYMSDK_ROOT)/include -I $(SYMSDK_ROOT)/include/libc -DLANGUAGE_EN -u ./qconsole.rss -o./resource/apps/qconsole.rsc   -h./resource/apps/qconsole.rsg -t./__tmp

private/10003a3f/apps/qconsole_reg.rsc : ./qconsole_reg.rss private/10003a3f/apps
	perl -S epocrc.pl -I $(SYMSDK_ROOT)/include -I .  -I ./resource/apps -I private/10003a3f/apps -I .. -I $(SYMSDK_ROOT)/include -I $(SYMSDK_ROOT)/include/libc -DLANGUAGE_EN -u ./qconsole_reg.rss -oprivate/10003a3f/apps/qconsole_reg.rsc   -hprivate/10003a3f/apps/qconsole_reg.rsg -t./__tmp

./resource/apps/qconsole_loc.rsc : ./qconsole_loc.rss ./resource/apps
	perl -S epocrc.pl -I $(SYMSDK_ROOT)/include -I .  -I ./resource/apps -I private/10003a3f/apps -I .. -I $(SYMSDK_ROOT)/include -I $(SYMSDK_ROOT)/include/libc -DLANGUAGE_EN -u ./qconsole_loc.rss -o./resource/apps/qconsole_loc.rsc   -h./resource/apps/qconsole_loc.rsg -t./__tmp

rel_binaries : ./resource/apps/qconsole.rsc private/10003a3f/apps/qconsole_reg.rsc ./resource/apps/qconsole_loc.rsc   __rel/qconsole.exe

deb_binaries : ./resource/apps/qconsole.rsc private/10003a3f/apps/qconsole_reg.rsc ./resource/apps/qconsole_loc.rsc   __deb/qconsole.exe

__rel/qconsole.exe : __rel/qconsole.in $(SDKLINK)/estlib.dso $(SDKLINK)/euser.dso $(SDKLINK)/hal.dso $(SDKLINK)/efsrv.dso $(SDKLINK)/c32.dso $(SDKLINK)/esock.dso $(SDKLINK)/bluetooth.dso $(SDKLINK)/btmanclient.dso $(SDKLINK)/sdpdatabase.dso $(SDKLINK)/charconv.dso $(SDKLINK)/apgrfx.dso $(SDKLINK)/usrt2_2.lib $(SDKLINK)/liblsym9.lib $(SDKLINK)/drtaeabi.dso $(SDKLINK)/dfpaeabi.dso $(SDKLINK)/drtrvct2_2.dso $(SDKLINK)/scppnwdl.dso  
	$(LD) $(LD_LIBDIRS) --target1-abs --no-undefined -nostdlib -shared -Ttext 0x8000 -Tdata 0x400000 --default-symver   --entry _E32Startup -u _E32Startup -soname qconsole{000a0000}[E0001671].exe $(SDKLINK)/eexe.lib  -L$(SDKLINK) --whole-archive __rel/qconsole.in --no-whole-archive $(SDKLINK)/estlib.dso $(SDKLINK)/euser.dso $(SDKLINK)/hal.dso $(SDKLINK)/efsrv.dso $(SDKLINK)/c32.dso $(SDKLINK)/esock.dso $(SDKLINK)/bluetooth.dso $(SDKLINK)/btmanclient.dso $(SDKLINK)/sdpdatabase.dso $(SDKLINK)/charconv.dso $(SDKLINK)/apgrfx.dso $(SDKLINK)/usrt2_2.lib $(SDKLINK)/liblsym9.lib $(SDKLINK)/drtaeabi.dso $(SDKLINK)/dfpaeabi.dso $(SDKLINK)/drtrvct2_2.dso $(SDKLINK)/scppnwdl.dso  -lsupc++ -lgcc  -o __rel/qconsole.elf -Map __rel/qconsole.map
	elf2e32 --fpu=softvfp --ignorenoncallable  --elfinput=__rel/qconsole.elf --output=__rel/qconsole.exe --targettype=EXE --uid1=0x1000007A --uid2=0x100039CE --uid3=0xE0001671 --libpath=$(SDKLINK) --sid=0xE0001671 --linkas=qconsole{000a0000}[E0001671].exe --vid=0x00000000   --capability=All-DRM $(ELF2E32_FLAGS) 
	cp -f __rel/qconsole.exe qconsole.exe

__deb/qconsole.exe : __deb/qconsole.in $(SDKLINK)/estlib.dso $(SDKLINK)/euser.dso $(SDKLINK)/hal.dso $(SDKLINK)/efsrv.dso $(SDKLINK)/c32.dso $(SDKLINK)/esock.dso $(SDKLINK)/bluetooth.dso $(SDKLINK)/btmanclient.dso $(SDKLINK)/sdpdatabase.dso $(SDKLINK)/charconv.dso $(SDKLINK)/apgrfx.dso $(SDKLINK)/usrt2_2.lib $(SDKLINK)/liblsym9.lib $(SDKLINK)/drtaeabi.dso $(SDKLINK)/dfpaeabi.dso $(SDKLINK)/drtrvct2_2.dso $(SDKLINK)/scppnwdl.dso  
	$(LD) $(LD_LIBDIRS) --target1-abs --no-undefined -nostdlib -shared -Ttext 0x8000 -Tdata 0x400000 --default-symver   --entry _E32Startup -u _E32Startup -soname qconsole{000a0000}[E0001671].exe $(SDKLINK)/eexe.lib  -L$(SDKLINK) --whole-archive __deb/qconsole.in --no-whole-archive $(SDKLINK)/estlib.dso $(SDKLINK)/euser.dso $(SDKLINK)/hal.dso $(SDKLINK)/efsrv.dso $(SDKLINK)/c32.dso $(SDKLINK)/esock.dso $(SDKLINK)/bluetooth.dso $(SDKLINK)/btmanclient.dso $(SDKLINK)/sdpdatabase.dso $(SDKLINK)/charconv.dso $(SDKLINK)/apgrfx.dso $(SDKLINK)/usrt2_2.lib $(SDKLINK)/liblsym9.lib $(SDKLINK)/drtaeabi.dso $(SDKLINK)/dfpaeabi.dso $(SDKLINK)/drtrvct2_2.dso $(SDKLINK)/scppnwdl.dso  -lsupc++ -lgcc  -o __deb/qconsole.elf -Map __deb/qconsole.map
	elf2e32 --fpu=softvfp --ignorenoncallable  --elfinput=__deb/qconsole.elf --output=__deb/qconsole.exe --targettype=EXE --uid1=0x1000007A --uid2=0x100039CE --uid3=0xE0001671 --libpath=$(SDKLINK) --sid=0xE0001671 --linkas=qconsole{000a0000}[E0001671].exe --vid=0x00000000   --capability=All-DRM $(ELF2E32_FLAGS) 
	cp -f __deb/qconsole.exe qconsole.exe

__rel/qconsole-exe.o : ../qconsole-exe.cpp
	$(CC) -O2 -DNDEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __rel/qconsole-exe.o ../qconsole-exe.cpp

__deb/qconsole-exe.o : ../qconsole-exe.cpp
	$(CC) -g -O0 -D_DEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __deb/qconsole-exe.o ../qconsole-exe.cpp

__rel/qconsole.o : ../qconsole.cpp
	$(CC) -O2 -DNDEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __rel/qconsole.o ../qconsole.cpp

__deb/qconsole.o : ../qconsole.cpp
	$(CC) -g -O0 -D_DEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __deb/qconsole.o ../qconsole.cpp

__rel/qcommands.o : ../qcommands.cpp
	$(CC) -O2 -DNDEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __rel/qcommands.o ../qcommands.cpp

__deb/qcommands.o : ../qcommands.cpp
	$(CC) -g -O0 -D_DEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __deb/qcommands.o ../qcommands.cpp

__rel/qsha1.o : ../qsha1.cpp
	$(CC) -O2 -DNDEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __rel/qsha1.o ../qsha1.cpp

__deb/qsha1.o : ../qsha1.cpp
	$(CC) -g -O0 -D_DEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __deb/qsha1.o ../qsha1.cpp

__rel/qutf7.o : ../qutf7.cpp
	$(CC) -O2 -DNDEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __rel/qutf7.o ../qutf7.cpp

__deb/qutf7.o : ../qutf7.cpp
	$(CC) -g -O0 -D_DEBUG -c -nostdinc -Wall -march=armv5t -mapcs -msoft-float -fexceptions -pipe -D__SYMBIAN32__ -D__EABI__ -D__MARM_ARMV5__ -D__MARM__ -D__SUPPORT_CPP_EXCEPTIONS__ -D_UNICODE -I$(SYMSDK_ROOT)/include/variant -D__PRODUCT_INCLUDE__ -include $(SYMSDK_ROOT)/include/gcce/gcce.h -D__EPOC32__ $(USERDEFS) -D__EXE__ -DSYMBIAN9  -I$(SYMSDK_ROOT)/include -I./resource/apps -Iprivate/10003a3f/apps -I.. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I.. -c -o __deb/qutf7.o ../qutf7.cpp

__tmp :
	mkdir __tmp

__rel :
	mkdir __rel

__deb :
	mkdir __deb

./resource/apps :
	mkdir -p ./resource/apps

private/10003a3f/apps :
	mkdir -p private/10003a3f/apps

__deb/qconsole.in : __deb/qconsole-exe.o __deb/qconsole.o __deb/qcommands.o __deb/qsha1.o __deb/qutf7.o
	rm -f __deb/qconsole.in
	$(AR) cr __deb/qconsole.in __deb/qconsole-exe.o __deb/qconsole.o __deb/qcommands.o __deb/qsha1.o __deb/qutf7.o

__rel/qconsole.in : __rel/qconsole-exe.o __rel/qconsole.o __rel/qcommands.o __rel/qsha1.o __rel/qutf7.o
	rm -f __rel/qconsole.in
	$(AR) cr __rel/qconsole.in __rel/qconsole-exe.o __rel/qconsole.o __rel/qcommands.o __rel/qsha1.o __rel/qutf7.o

ifeq ($(CFG), deb)

clean :
	rm -rf __deb __tmp qconsole.exe  ./resource/apps/qconsole.rsg ./resource/apps/qconsole.rsc private/10003a3f/apps/qconsole_reg.rsg private/10003a3f/apps/qconsole_reg.rsc ./resource/apps/qconsole_loc.rsg ./resource/apps/qconsole_loc.rsc 

else

clean :
	rm -rf __rel __tmp qconsole.exe  ./resource/apps/qconsole.rsg ./resource/apps/qconsole.rsc private/10003a3f/apps/qconsole_reg.rsg private/10003a3f/apps/qconsole_reg.rsc ./resource/apps/qconsole_loc.rsg ./resource/apps/qconsole_loc.rsc 

endif

