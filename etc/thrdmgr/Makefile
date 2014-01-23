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

BUILDGO4LIBS = $(GO4BASE_LIB) \
               $(THRDMNGR_LIB)

MODULES  = Go4Exceptions Go4LockGuard Go4Log Go4ThreadManager Go4Dict

EXMODULES = Go4ThreadManagerExample

FASTRULES += clean-mainlibs clean-bin

include $(patsubst %,%/Module.mk,$(MODULES))

include $(patsubst %,%/Makefile, $(EXMODULES))

build/dummy.d: Makefile $(ALLHDRS)
	@(if [ ! -f $@ ] ; then touch $@; fi)
	@(if [ ! -f lib ] ; then mkdir -p lib; fi)
	@(if [ ! -f bin ] ; then mkdir -p bin; fi)
	@(if [ ! -f $(GO4MAP) ] ; then touch $(GO4MAP); fi)

libs::    $(BUILDGO4LIBS)

examples:  $(patsubst %,all-%,$(EXMODULES)) $(EXAMPLEEXECS)

clean:: clean-bin clean-mainlibs
	@rm -f $(GO4MAP)
	@$(CleanLib) $(GO4BASE_LIBNAME) $(GO4DLLPATH)
	@echo "Clean go4 threadmanager done"

clean-mainlibs:
	@$(CleanLib) $(GO4BASE_LIBNAME) $(GO4DLLPATH)
	$(CleanLib) $(THRDMNGR_LIBNAME) $(GO4DLLPATH)

clean-bin::
	@echo "Clean binary (generated) files"

Go4_Makefile_Rules =
include Makefile.rules

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include build/dummy.d
endif
