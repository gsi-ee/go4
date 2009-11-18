ifndef GO4SYS
  GO4SYS = .
endif

GO4PACKAGE = go4

include Makefile.config

ifdef QTPATH
ifeq ($(wildcard $(QTPATH)/*),)
$(error No files found in $(QTPATH), check your Qt installation)
endif
endif


Go4_Makefile_Rules = included

ifdef GO4_WIN32
  GO4SYS = .
endif   

# rules which are used to produce packages
PACKAGERULES    = compress go4-package fit-package \
                  thrd-package task-package win-bin \
                  fast-packages packages 

ifneq ($(findstring $(MAKECMDGOALS), $(PACKAGERULES)),)
DOPACKAGE = true
endif

# new staff concerning big libraries

GO4FIT_LIBNAME  = $(LIB_PREFIX)Go4Fit
GO4FIT_LIB      = $(GO4DLLPATH)/$(GO4FIT_LIBNAME).$(DllSuf)

GO4BASE_LIBNAME = $(LIB_PREFIX)Go4Base
GO4BASE_LIB     = $(GO4DLLPATH)/$(GO4BASE_LIBNAME).$(DllSuf)

THRDMNGR_LIBNAME= $(LIB_PREFIX)Go4ThreadManager
THRDMNGR_LIB    = $(GO4DLLPATH)/$(THRDMNGR_LIBNAME).$(DllSuf)

GO4TSKH_LIBNAME = $(LIB_PREFIX)Go4TaskHandler
GO4TSKH_LIB     = $(GO4DLLPATH)/$(GO4TSKH_LIBNAME).$(DllSuf)

GO4ANBASE_LIBNAME = $(LIB_PREFIX)Go4AnalBase
GO4ANBASE_LIB     = $(GO4DLLPATH)/$(GO4ANBASE_LIBNAME).$(DllSuf)

GO4AN_LIBNAME   = $(LIB_PREFIX)Go4Analysis
GO4AN_LIB       = $(GO4DLLPATH)/$(GO4AN_LIBNAME).$(DllSuf)

GO4BGUI_LIBNAME = $(LIB_PREFIX)Go4GUI
GO4BGUI_LIB     = $(GO4DLLPATH)/$(GO4BGUI_LIBNAME).$(DllSuf)

BUILDGO4LIBS = $(GO4FIT_LIB) \
               $(GO4BASE_LIB) \
               $(THRDMNGR_LIB) \
               $(GO4TSKH_LIB) \
               $(GO4ANBASE_LIB) \
               $(GO4AN_LIB) \
               $(GO4BGUI_LIB)

MODULES  = MbsAPIbase MbsAPI RawAPI Go4Analysis Go4AnalysisClient \
           Go4CommandsAnalysis Go4CommandsBase \
           Go4CommandsTaskHandler Go4TaskHandler \
           Go4ConditionsBase Go4DynamicList Go4Event \
           Go4EventServer Go4Exceptions \
           Go4Display Go4Proxies \
           Go4Fit Go4ObjectManager \
           Go4HistogramServer Go4LockGuard Go4Log \
           Go4Queue Go4Socket Go4StatusAnalysis Go4StatusBase \
           Go4ThreadManager
           
EXMODULES = Go4ExampleSimple Go4Example1Step Go4Example2Step Go4ExampleAdvanced \
            Go4ExampleUserSource Go4ExampleMesh Go4FitExample \
            Go4ThreadManagerExample Go4TaskHandlerExample Go4EventServerExample

.PHONY:         all includes libs gui plugin install uninstall \
                clean clean-qt3 clean-qt4 clean-bak clean-plugin clean-mainlibs clean-prefix \
                package $(PACKAGERULES)


FASTRULES    += clean-qt3 clean-qt4 clean-bak clean-dep clean-plugin clean-bin clean-prefix \
                $(PACKAGERULES)

all::           gui 
ifdef GO4PREFIX
	@echo Call make install to copy all binary files into $(GO4PREFIX)
endif

include $(patsubst %,%/Module.mk,$(MODULES))

include $(patsubst %,%/Makefile, $(EXMODULES))

-include qt3/Module.mk

-include qt4/Module.mk

build/dummy.d: Makefile $(GO4QTHEADS) $(ALLHDRS)
	@(if [ ! -f $@ ] ; then touch $@; fi)
	@(if [ ! -f lib ] ; then mkdir -p lib; fi)
	@(if [ ! -f bin ] ; then mkdir -p bin; fi)
	@(if [ ! -f bin/go4-config ] ; then   \
		sed -e "s|@go4arch@|$(GO4_OS)|"    \
		-e "s|@go4vers@|$(VERSSUF)|"       \
		-e "s|@go4topdir@|$(GO4TOPPATH)|"  \
		-e "s|@go4bindir@|$(GO4EXEPATH)|"  \
		-e "s|@go4libdir@|$(GO4LIBPATH)|"  \
		-e "s|@go4incdir@|$(GO4INCPATH)|"  \
		< build/go4-config.ini > bin/go4-config; chmod 755 bin/go4-config; echo "create go4-config"; fi)
	@(if [ ! -f $(GO4MAP) ] ; then touch $(GO4MAP); fi)

libs::          $(BUILDGO4LIBS)

gui::           libs 

ifndef GO4PREFIX
install:
	@echo "Go4 compiled without prefix set - install is not allowed"

uninstall:
	@echo "Go4 compiled without prefix set - uninstall is not allowed"
else
uninstall:
	@rm -rf $(GO4TOPPATH)
	@rm -f $(GO4EXEPATH)/go4 $(GO4EXEPATH)/go4analysis $(GO4EXEPATH)/go4-config
	@rm -f $(GO4LIBPATH)/libGo4*.*
	@rm -rf $(GO4INCPATH)

install: uninstall
	@echo "Installing Go4 in directory $(GO4PREFIX) ..."
	@mkdir -p $(GO4EXEPATH); cp bin/go4 bin/go4analysis bin/go4-config $(GO4EXEPATH)
	@mkdir -p $(GO4LIBPATH); cp lib/* $(GO4LIBPATH)
	@mkdir -p $(GO4INCPATH); cp include/* $(GO4INCPATH)
	@mkdir -p $(GO4TOPPATH); cp Makefile.config Makefile.rules $(GO4TOPPATH)
	@mkdir -p $(GO4TOPPATH)/build; cp build/*.sh build/Makefile.* $(GO4TOPPATH)/build
	@mkdir -p $(GO4TOPPATH)/etc; cp -r etc/* $(GO4TOPPATH)/etc
	@mkdir -p $(GO4TOPPATH)/macros; cp macros/* $(GO4TOPPATH)/macros
	@mkdir -p $(GO4TOPPATH)/docs; cp docs/* $(GO4TOPPATH)/docs
ifeq ($(GO4_QT), 4)
	@mkdir -p $(GO4TOPPATH)/qt4; cp qt4/go4.conf $(GO4TOPPATH)/qt4
	@mkdir -p $(GO4TOPPATH)/qt4/Go4UserGUI; cp qt4/Go4UserGUI/libGo4UserGui.$(DllSuf) $(GO4TOPPATH)/qt4/Go4UserGUI
endif
ifeq ($(GO4_QT), 3)
	@mkdir -p $(GO4TOPPATH)/qt3/etc; cp qt3/etc/* $(GO4TOPPATH)/qt3/etc
	@mkdir -p $(GO4TOPPATH)/qt3/Go4UserGUI; cp qt3/Go4UserGUI/libGo4UserGui.$(DllSuf) $(GO4TOPPATH)/qt3/Go4UserGUI
endif
	@mkdir -p $(GO4TOPPATH)/icons; cp icons/* $(GO4TOPPATH)/icons
	@echo "Installation completed"
endif

clean::  clean-bin clean-mainlibs clean-plugin
	@rm -f $(GO4MAP)
	@rm -f include/*.h
	@rm -rf bin lib
	@rm -f build/dummy.d build/Makefile.gener go4login go4login.bat go4.bat
	@echo "Clean go4 done"

clean-mainlibs:
	@$(CleanLib) $(GO4FIT_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4BASE_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(THRDMNGR_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4TSKH_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4ANBASE_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4AN_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4BGUI_LIBNAME) $(GO4DLLPATH)

clean-bin::
	@echo "Clean binary (generated) files"

clean-bak:
	@echo "Delete bak files"
	@rm -f $(patsubst %,%/*.bak,$(MODULES))
	@rm -f $(patsubst %,%/*.*~,$(MODULES))
	@rm -f $(patsubst %,%/*.bak,$(EXMODULES))
	@rm -f $(patsubst %,%/*.*~,$(EXMODULES))
	@rm -f go4logfile.txt $(patsubst %,%/go4logfile.txt,$(EXMODULES))

clean-dep:
	@rm -f $(LIBDEPENDENC) $(EXAMPDEPENDENCS)
	@echo "Delete all dependency files"

clean-prefix:
	@rm -f Go4Log/TGo4Log.o Go4Log/TGo4Log.d
	@rm -f qt3/Go4GUI/Makefile.qt qt4/Go4GUI/Makefile.qt 
	@rm -f build/Makefile.gener go4login go4login.bat go4.bat
	@rm -f bin/*
	@echo "Clean prefix-dependend files done"

GO4BASE_O = $(LOCKGRD_O) $(LOCKGRD_DO) \
            $(GO4LOG_O) $(GO4LOG_DO) \
            $(EXCEPT_O) \
            $(COMBASE_O) $(COMBASE_DO) \
            $(STATBASE_O) $(STATBASE_DO) \
            $(CONDBASE_O) $(CONDBASE_DO)

ifdef GO4_WIN32
$(GO4BASE_O): CXXFLAGS += -DBUILDING_GO4BASE_DLL
endif

GO4BASE_LINKDEFS = $(LOCKGRD_LINKDEF) \
                   $(GO4LOG_LINKDEF) \
                   $(COMBASE_LINKDEF) \
                   $(STATBASE_LINKDEF) \
                   $(CONDBASE_LINKDEF)

GO4TSKH_O = $(GO4SOCKET_O) \
            $(GO4QUEUE_O) \
            $(TASKHAND_O) $(TASKHAND_DO) \
            $(CMDTASKHANDL_O) $(CMDTASKHANDL_DO)

GO4TSKH_LINKDEFS = $(TASKHAND_LINKDEF) $(CMDTASKHANDL_LINKDEF)

GO4ANBASE_O = $(MBSAPIBASE_O)  \
            $(GO4EVENTPAR_O) $(GO4EVENTPAR_DO) \
            $(EVENTSERVPAR_O) $(EVENTSERVPAR_DO) \
            $(DYNLIST_O) $(DYNLIST_DO) \
            $(STATANAL_O) $(STATANAL_DO) \
            $(VERSION_O) 

GO4ANBASE_LINKDEFS = $(GO4EVENTPAR_LINKDEF) \
                     $(EVENTSERVPAR_LINKDEF) \
                     $(DYNLIST_LINKDEF) \
                     $(STATANAL_LINKDEF)

GO4AN_O   = $(MBSAPI_O) $(RAWAPI_O) \
            $(GO4EVENT_O) $(GO4EVENT_DO) \
            $(EVENTSERV_O) $(EVENTSERV_DO) \
            $(HISTSERV_O) $(HISTSERV_DO) \
            $(GO4ANAL_O) $(GO4ANAL_DO) \
            $(CMDANAL_O) $(CMDANAL_DO) \
            $(ANALCL_O) $(ANALCL_DO)

GO4AN_LINKDEFS = $(GO4EVENT_LINKDEF) \
                 $(EVENTSERV_LINKDEF) \
                 $(HISTSERV_LINKDEF) \
                 $(GO4ANAL_LINKDEF) \
                 $(CMDANAL_LINKDEF) \
                 $(ANALCL_LINKDEF)

GO4BGUI_O = $(GO4OBJM_O) $(GO4OBJM_DO) \
            $(GO4DISPL_O) $(GO4DISPL_DO) \
            $(GO4PROX_O) $(GO4PROX_DO)

GO4BGUI_LINKDEFS = $(GO4OBJM_LINKDEF) \
                   $(GO4PROX_LINKDEF)

$(GO4FIT_LIB):   $(GO4FIT_O) $(GO4FIT_DO)
	@$(MakeLibrary) $(GO4FIT_LIBNAME) "$(GO4FIT_O) $(GO4FIT_DO)" $(GO4DLLPATH) $(GO4FIT_LINKDEF) "$(BASIC_LIB_DEP)"

$(GO4BASE_LIB):  $(GO4BASE_O)
	@$(MakeLibrary) $(GO4BASE_LIBNAME) "$(GO4BASE_O)" $(GO4DLLPATH) "$(GO4BASE_LINKDEFS)" "$(BASIC_LIB_DEP)"

$(THRDMNGR_LIB):   $(THRDMNGR_O) $(THRDMNGR_DO)
	@$(MakeLibrary) $(THRDMNGR_LIBNAME) "$(THRDMNGR_O) $(THRDMNGR_DO)" $(GO4DLLPATH) $(THRDMNGR_LINKDEF) "$(GO4BASE_LIB) $(BASIC_LIB_DEP)"

$(GO4TSKH_LIB):   $(GO4TSKH_O)
	@$(MakeLibrary) $(GO4TSKH_LIBNAME) "$(GO4TSKH_O)" $(GO4DLLPATH) "$(GO4TSKH_LINKDEFS)" "$(THRDMNGR_LIB) $(GO4BASE_LIB) $(BASIC_LIB_DEP)"

$(GO4ANBASE_LIB): $(GO4ANBASE_O)
	@$(MakeLibrary) $(GO4ANBASE_LIBNAME) "$(GO4ANBASE_O)" $(GO4DLLPATH) "$(GO4ANBASE_LINKDEFS)" "$(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

$(GO4AN_LIB): $(GO4AN_O)
	@$(MakeLibrary) $(GO4AN_LIBNAME) "$(GO4AN_O)" $(GO4DLLPATH) "$(GO4AN_LINKDEFS)" "$(GO4ANBASE_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

$(GO4BGUI_LIB): $(GO4BGUI_O)
	@$(MakeLibrary) $(GO4BGUI_LIBNAME) "$(GO4BGUI_O)" $(GO4DLLPATH) "$(GO4BGUI_LINKDEFS)" "$(GO4ANBASE_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"


ifdef DOPACKAGE
-include build/Makefile.packaging
endif

Go4_Makefile_Rules =
include Makefile.rules

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include build/dummy.d
endif
