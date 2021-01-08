
prefix = /home/dms/linux/IMX6ULL/ALPHA/buildroot-2020.02.3/output/host/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
OBJCOPY = $(prefix)objcopy
cxx = $(prefix)c++

sos = $(wildcard *.so)
sosection = $(patsubst %.so,%.section,$(sos))

exes = $(wildcard *.exe)
exesection = $(patsubst %.exe,%.section,$(exes))

cfgs = $(wildcard *.txt)
cfgsection = $(patsubst %.txt,%.section,$(cfgs))

file = main.cpp 
#flags = -Wl,-Tupdate.lds 

VERSION ?= _5.1
SCG := 1
EXE := 1

ifeq ($(EXE),1)
file += $(sosection) $(exesection) 
flags += -DEXE
endif

ifeq ($(SCG),1)
file += $(cfgsection)
flags += -DSCG
endif


aim = MDC2_SV_1$(VERSION).bin


$(aim):$(file)
	$(cxx) -o $@ $^ $(flags)
	cp $(aim) aim/



$(filter %.section , $(sosection)): %.section: %.so
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@

$(filter %.section , $(exesection)): %.section: %.exe
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@

$(filter %.section , $(cfgsection)): %.section: %.txt
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@



.PHONY:clean
clean:
	-rm -f  ./*.o ./*.section
	-rm -f ./*.txt
	-rm -f ./*.EXE
	-rm -f ./*.exe
	-rm -f ./*.so
	-rm -f aim/* 
	-rm -f ./*.bin
