
prefix = /share/nuc/buildroot-2019.02.1/output/host/bin/arm-buildroot-linux-gnueabi-
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

VERSION = _1.0,2.45,2.1
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


aim = update$(VERSION).bin


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
	-rm -f update*.bin
	-rm -f aim/* 
