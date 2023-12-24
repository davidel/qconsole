PATH := $(SYMSDK_ROOT)/bin:$(GCC_ROOT)/bin:$(GCC_ROOT)/arm-epoc-pe/bin:$(GCC_ROOT)/lib/gcc-lib/arm-epoc-pe/2.9-psion-98r2:$(PATH)
AMODE = armi
SDKLINK = $(SYMSDK_ROOT)/release/$(AMODE)


ifeq ($(CFG), deb)

all : __tmp __deb  deb_binaries 

else

all : __tmp __rel  rel_binaries 

endif

rel_binaries : __rel/qconsole.exe

deb_binaries : __deb/qconsole.exe

__rel/qconsole.exe : __rel/qconsole.in $(SDKLINK)/urel/eexe.lib $(SDKLINK)/urel/euser.lib $(SDKLINK)/urel/hal.lib $(SDKLINK)/urel/efsrv.lib $(SDKLINK)/urel/c32.lib $(SDKLINK)/urel/esock.lib $(SDKLINK)/urel/bluetooth.lib $(SDKLINK)/urel/apgrfx.lib $(SDKLINK)/urel/btmanclient.lib $(SDKLINK)/urel/sdpdatabase.lib $(SDKLINK)/urel/charconv.lib $(SDKLINK)/urel/estlib.lib  
	ld -s -e _E32Startup -u _E32Startup --base-file __rel/qconsole.bas -o __rel/qconsole.exe $(SDKLINK)/urel/eexe.lib --whole-archive __rel/qconsole.in --no-whole-archive $(SDKLINK)/urel/euser.lib $(SDKLINK)/urel/hal.lib $(SDKLINK)/urel/efsrv.lib $(SDKLINK)/urel/c32.lib $(SDKLINK)/urel/esock.lib $(SDKLINK)/urel/bluetooth.lib $(SDKLINK)/urel/apgrfx.lib $(SDKLINK)/urel/btmanclient.lib $(SDKLINK)/urel/sdpdatabase.lib $(SDKLINK)/urel/charconv.lib $(SDKLINK)/urel/estlib.lib 
	dlltool -m arm_interwork --base-file __rel/qconsole.bas --output-exp __rel/qconsole.exp
	ld -s -e _E32Startup -u _E32Startup __rel/qconsole.exp -Map __rel/qconsole.exe.map -o __rel/qconsole.exe.pe $(SDKLINK)/urel/eexe.lib --whole-archive __rel/qconsole.in  --no-whole-archive $(SDKLINK)/urel/euser.lib $(SDKLINK)/urel/hal.lib $(SDKLINK)/urel/efsrv.lib $(SDKLINK)/urel/c32.lib $(SDKLINK)/urel/esock.lib $(SDKLINK)/urel/bluetooth.lib $(SDKLINK)/urel/apgrfx.lib $(SDKLINK)/urel/btmanclient.lib $(SDKLINK)/urel/sdpdatabase.lib $(SDKLINK)/urel/charconv.lib $(SDKLINK)/urel/estlib.lib 
	arm-epoc-pe-objcopy -X __rel/qconsole.exe.pe __rel/qconsole.exe.sym
	petran -nocall   -uid1 0x1000007A -uid2 0x00000000 -uid3 0x00000000 __rel/qconsole.exe.pe __rel/qconsole.exe
	cp -f __rel/qconsole.exe qconsole.exe

__deb/qconsole.exe : __deb/qconsole.in $(SDKLINK)/udeb/eexe.lib $(SDKLINK)/udeb/euser.lib $(SDKLINK)/udeb/hal.lib $(SDKLINK)/udeb/efsrv.lib $(SDKLINK)/udeb/c32.lib $(SDKLINK)/udeb/esock.lib $(SDKLINK)/udeb/bluetooth.lib $(SDKLINK)/udeb/apgrfx.lib $(SDKLINK)/udeb/btmanclient.lib $(SDKLINK)/udeb/sdpdatabase.lib $(SDKLINK)/udeb/charconv.lib $(SDKLINK)/udeb/estlib.lib  
	ld  -e _E32Startup -u _E32Startup --base-file __deb/qconsole.bas -o __deb/qconsole.exe $(SDKLINK)/udeb/eexe.lib --whole-archive __deb/qconsole.in --no-whole-archive $(SDKLINK)/udeb/euser.lib $(SDKLINK)/udeb/hal.lib $(SDKLINK)/udeb/efsrv.lib $(SDKLINK)/udeb/c32.lib $(SDKLINK)/udeb/esock.lib $(SDKLINK)/udeb/bluetooth.lib $(SDKLINK)/udeb/apgrfx.lib $(SDKLINK)/udeb/btmanclient.lib $(SDKLINK)/udeb/sdpdatabase.lib $(SDKLINK)/udeb/charconv.lib $(SDKLINK)/udeb/estlib.lib 
	dlltool -m arm_interwork --base-file __deb/qconsole.bas --output-exp __deb/qconsole.exp
	ld  -e _E32Startup -u _E32Startup __deb/qconsole.exp -Map __deb/qconsole.exe.map -o __deb/qconsole.exe.pe $(SDKLINK)/udeb/eexe.lib --whole-archive __deb/qconsole.in  --no-whole-archive $(SDKLINK)/udeb/euser.lib $(SDKLINK)/udeb/hal.lib $(SDKLINK)/udeb/efsrv.lib $(SDKLINK)/udeb/c32.lib $(SDKLINK)/udeb/esock.lib $(SDKLINK)/udeb/bluetooth.lib $(SDKLINK)/udeb/apgrfx.lib $(SDKLINK)/udeb/btmanclient.lib $(SDKLINK)/udeb/sdpdatabase.lib $(SDKLINK)/udeb/charconv.lib $(SDKLINK)/udeb/estlib.lib 
	arm-epoc-pe-objcopy -X __deb/qconsole.exe.pe __deb/qconsole.exe.sym
	petran -nocall   -uid1 0x1000007A -uid2 0x00000000 -uid3 0x00000000 __deb/qconsole.exe.pe __deb/qconsole.exe
	cp -f __deb/qconsole.exe qconsole.exe

__rel/qconsole-exe.o : ./qconsole-exe.cpp
	gcc -s -fomit-frame-pointer -O -DNDEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __rel/qconsole-exe.o ./qconsole-exe.cpp

__deb/qconsole-exe.o : ./qconsole-exe.cpp
	gcc -g -O -D_DEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __deb/qconsole-exe.o ./qconsole-exe.cpp

__rel/qconsole.o : ./qconsole.cpp
	gcc -s -fomit-frame-pointer -O -DNDEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __rel/qconsole.o ./qconsole.cpp

__deb/qconsole.o : ./qconsole.cpp
	gcc -g -O -D_DEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __deb/qconsole.o ./qconsole.cpp

__rel/qcommands.o : ./qcommands.cpp
	gcc -s -fomit-frame-pointer -O -DNDEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __rel/qcommands.o ./qcommands.cpp

__deb/qcommands.o : ./qcommands.cpp
	gcc -g -O -D_DEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __deb/qcommands.o ./qcommands.cpp

__rel/qsha1.o : ./qsha1.cpp
	gcc -s -fomit-frame-pointer -O -DNDEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __rel/qsha1.o ./qsha1.cpp

__deb/qsha1.o : ./qsha1.cpp
	gcc -g -O -D_DEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __deb/qsha1.o ./qsha1.cpp

__rel/qutf7.o : ./qutf7.cpp
	gcc -s -fomit-frame-pointer -O -DNDEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __rel/qutf7.o ./qutf7.cpp

__deb/qutf7.o : ./qutf7.cpp
	gcc -g -O -D_DEBUG -D_UNICODE -march=armv4t -mthumb-interwork -pipe -c -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__ $(USERDEFS) -D__EXE__  -I$(SYMSDK_ROOT)/include -I./qtty -I. -I$(SYMSDK_ROOT)/include -I$(SYMSDK_ROOT)/include/libc -I. -c -o __deb/qutf7.o ./qutf7.cpp

__tmp :
	mkdir __tmp

__rel :
	mkdir __rel

__deb :
	mkdir __deb

__deb/qconsole.in : __deb/qconsole-exe.o __deb/qconsole.o __deb/qcommands.o __deb/qsha1.o __deb/qutf7.o
	rm -f __deb/qconsole.in
	ar cr __deb/qconsole.in __deb/qconsole-exe.o __deb/qconsole.o __deb/qcommands.o __deb/qsha1.o __deb/qutf7.o

__rel/qconsole.in : __rel/qconsole-exe.o __rel/qconsole.o __rel/qcommands.o __rel/qsha1.o __rel/qutf7.o
	rm -f __rel/qconsole.in
	ar cr __rel/qconsole.in __rel/qconsole-exe.o __rel/qconsole.o __rel/qcommands.o __rel/qsha1.o __rel/qutf7.o

ifeq ($(CFG), deb)

clean :
	rm -rf __deb __tmp qconsole.exe  

else

clean :
	rm -rf __rel __tmp qconsole.exe  

endif

