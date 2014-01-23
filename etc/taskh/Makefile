ifndef GO4SYS
  GO4SYS = .
endif

.PHONY:         all libs clean clean-mainlibs

all::       libs

include build/Makefile.discover

include Makefile.config

Go4_Makefile_Rules = included

GO4PACKAGE = go4threadmanager

GO4BASE_LIBNAME = $(LIB_PREFIX)Go4Base
GO4BASE_LIB     = $(GO4DLLPATH)/$(GO4BASE_LIBNAME).$(DllSuf)

THRDMNGR_LIBNAME= $(LIB_PREFIX)Go4ThreadManager
THRDMNGR_LIB    = $(GO4DLLPATH)/$(THRDMNGR_LIBNAME).$(DllSuf)

GO4TSKH_LIBNAME = $(LIB_PREFIX)Go4TaskHandler
GO4TSKH_LIB     = $(GO4DLLPATH)/$(GO4TSKH_LIBNAME).$(DllSuf)

BUILDGO4LIBS = $(GO4BASE_LIB) \
               $(THRDMNGR_LIB) \
               $(GO4TSKH_LIB)

MODULES  = Go4Exceptions Go4LockGuard Go4Log \
           Go4ThreadManager \
           Go4Queue Go4Socket Go4StatusBase \
           Go4CommandsBase Go4CommandsTaskHandler \
           Go4TaskHandler Go4Dict

EXMODULES = Go4ThreadManagerExample Go4TaskHandlerExample 

FASTRULES += clean-mainlibs clean-bin

include $(patsubst %,%/Module.mk,$(MODULES))

include $(patsubst %,%/Makefile, $(EXMODULES))

lib:
	@(if [ ! -d $@ ] ; then mkdir -p $@; fi)

bin:
	@(if [ ! -d $@ ] ; then mkdir -p $@; fi)

bin/go4-config: bin Makefile
	@echo Producing $@
	@sed -e "s|@go4arch@|$(GO4_OS)|"        \
		  -e "s|@go4vers@|$(VERSSUF)|"       \
		  -e "s|@go4topdir@|$(GO4TOPPATH)|"  \
		  -e "s|@go4bindir@|$(GO4EXEPATH)|"  \
		  -e "s|@go4libdir@|$(GO4LIBPATH)|"  \
		  -e "s|@go4incdir@|$(GO4INCPATH)|"  \
		  -e "s|@go4mainlibs@|'$(subst -Llib,-L$(GO4LIBPATH),$(LIBS_FULLSET))'|"  \
		  -e "s|@go4guilibs@|'$(subst -L../../lib,-L$(GO4LIBPATH),$(LIBS_GUISET))'|"  \
		  -e "s|@go4cflags@|'$(subst -Iinclude -I.,-I$(GO4INCPATH),$(OPTFLAGS) $(CXXFLAGS))'|" \
		     < build/go4-config.ini > $@
	@chmod 755 $@

build/dummy.d: Makefile bin lib $(ALLHDRS)
	@(if [ ! -f $@ ] ; then touch $@; fi)
	@(if [ ! -f $(GO4MAP) ] ; then touch $(GO4MAP); fi)

libs::    $(BUILDGO4LIBS) go4login bin/go4-config 

examples:  $(patsubst %,all-%,$(EXMODULES)) $(EXAMPLEEXECS)

clean:: clean-bin clean-mainlibs
	@rm -f $(GO4MAP)
	@echo "Clean go4 taskhandler done"


clean-bin::
	@echo "Clean binary (generated) files"

Go4_Makefile_Rules =
include Makefile.rules

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include build/dummy.d
endif
