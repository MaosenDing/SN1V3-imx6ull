include ../Makefile.param


#目标
aim = aim.exe
export ldaim = build_in.o

 
#内部目标 
build_in = /build_in.o
path = text time imageProcess errHandle sensorProcess communicate thread datapro spwork
#path = text time imageProcess errHandle communicate thread
pathobj = $(addsuffix $(build_in),$(path))

solib = libsn.so
alib = libsn.a


clockPath = clockd errHandle
clockObj = $(addsuffix $(build_in),$(clockPath))

clockd = mdc.exe

linkJD = -L . -lJD

daemonPath = daemon errHandle
daemonObj = $(addsuffix $(build_in),$(daemonPath))
daemon = daemon.exe

all:aim $(clockd) env $(daemon)
#aim
	-cp -f $(aim) $(cpdir)
	$(cstrip) $(cpdir)/$(aim)
#mdc
	-cp -f $(clockd) $(cpdir)
	$(cstrip) $(cpdir)/$(clockd)
#daemon
	-cp -f $(daemon) $(cpdir)
	$(cstrip) $(cpdir)/$(daemon)

env:
	-cp -f workscrip/* $(cpdir)/


aim:test.o $(path)
	$(cxx) -o $(aim) test.o $(pathobj) $(linkFlag) $(linkJD)
	
#mdc part
$(clockd):$(clockPath)
	$(cxx) -o $@ $(clockObj) $(cppflags) $(linkJD) -lpthread -L $(APP_DIR)/glog -lglog

ECHO2:
	@echo $(clockPath)

$(clockPath):ECHO2
	$(MAKE) -C $@ LDD

#deamon part
$(daemon):$(daemonPath)
	$(cxx) -o $@ $(daemonObj) $(cppflags) $(linkJD) -lpthread -L $(APP_DIR)/glog -lglog

ECHO3:
	@echo $(daemonPath)

$(daemonPath):ECHO3
	$(MAKE) -C $@ LDD


#aim part
ar:$(path)
	$(ar) -rcs $(alib) test.o $(pathobj)

araim:test.o ar
	$(cxx) -o $(aim) test.o -L ./ $(alib) $(linkFlag) $(linkJD)
	$(cstrip) $(aim)
	-cp -f $(aim) $(cpdir)

so:$(path)
	$(cxx) -o $(solib) $(pathobj) $(shareflags)
	$(cstrip) $(solib)

soaim:test.o so 
	$(cxx) -o $(aim) test.o -L./ $(solib) $(linkFlag) $(linkJD)
	$(cstrip) $(aim)
	-cp -f $(aim) $(cpdir)
	-cp -f $(solib) $(cpdir)

ECHO:
	@echo $(path)

$(path):ECHO
	$(MAKE) -C $@ LDD

#$(pathobj):
#	$(MAKE) -C $(dir  $@) LDD

test.o:test.cpp
	$(cxx) -c $< $(cppflags)

.PHONY:clean
clean:
	-rm  -f $(addsuffix /*.o,$(path))
	-rm  -f $(addsuffix /*.o,$(clockPath))
	-rm  -f $(addsuffix /*.o,$(daemonPath))
	-rm -f  ./*.o 
	-rm $(aim) $(daemon) $(clockd)