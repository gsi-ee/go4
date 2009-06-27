ifndef GO4SYS
  GO4SYS = .
endif

include Makefile.config

Go4_Makefile_Rules = included

ifdef GO4_WIN32
  GO4SYS = .
endif   


GO4PACKAGE = go4

# rules which are used to produce packages
PACKAGERULES    = compress go4-package fit-package \
                  thrd-package task-package win-bin win-src\
                  fast-packages packages 

ifneq ($(findstring $(MAKECMDGOALS), $(PACKAGERULES)),)
DOPACKAGE = true
endif

# comment to switch off debugging
ifndef GO4_WIN32
DOOPTIMIZATION = false
QMAKEOPTFLAG = "CONFIG+=debug" "QMAKE_CXXFLAGS+=-Wall"
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

VERSION_LIBNAME  = $(LIB_PREFIX)Go4Version
VERSION_LIB      = $(GO4DLLPATH)/$(VERSION_LIBNAME).$(DllSuf)

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
               $(VERSION_LIB) \
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
           
EXMODULES = Go4ExampleSimple Go4Example1Step Go4Example2Step \
            Go4ExampleUserSource Go4ExampleMesh Go4FitExample \
            Go4ThreadManagerExample Go4TaskHandlerExample Go4EventServerExample

.PHONY:         all noqt includes libs gui plugin \
                clean clean-qt3 clean-qt4 clean-bak clean-plugin clean-mainlibs\
                package $(PACKAGERULES)

FASTRULES    += clean-qt3 clean-qt4 clean-bak clean-dep clean-plugin clean-bin \
                $(PACKAGERULES)

all::           gui 

include $(patsubst %,%/Module.mk,$(MODULES))

include $(patsubst %,%/Makefile, $(EXMODULES))

-include qt3/Module.mk

-include qt4/Module.mk


build/dummy.d: Makefile $(GO4QTHEADS) $(ALLHDRS)
	@(if [ ! -f $@ ] ; then touch $@; fi)
	@(if [ ! -f lib ] ; then mkdir -p lib; fi)
	@(if [ ! -f bin ] ; then mkdir -p bin; fi)
	@(if [ ! -f $(GO4MAP) ] ; then touch $(GO4MAP); fi)

libs::          $(BUILDGO4LIBS)

gui::           libs 

noqt:           all

clean::  clean-bin clean-mainlibs clean-plugin
	@rm -f $(GO4MAP)
	@rm -f $(GO4SYS)/include/*.h
	@rm -rf bin lib
	@rm -f build/dummy.d
	@echo "Clean go4 done"

clean-mainlibs:
	@$(CleanLib) $(GO4FIT_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4BASE_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(THRDMNGR_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4TSKH_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4ANBASE_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(GO4AN_LIBNAME) $(GO4DLLPATH)
	@$(CleanLib) $(VERSION_LIBNAME) $(GO4DLLPATH)
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

GO4BASE_O = $(LOCKGRD_O) $(LOCKGRD_DO) \
            $(GO4LOG_O) $(GO4LOG_DO) \
            $(EXCEPT_O) \
            $(COMBASE_O) $(COMBASE_DO) \
            $(STATBASE_O) $(STATBASE_DO) \
            $(CONDBASE_O) $(CONDBASE_DO)


$(GO4BASE_O): CXXFLAGS+=-DBUILDING_GO4BASE_DLL

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
            $(STATANAL_O) $(STATANAL_DO)

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
	@$(MakeLibrary) $(GO4ANBASE_LIBNAME) "$(GO4ANBASE_O)" $(GO4DLLPATH) "$(GO4ANBASE_LINKDEFS)" "$(VERSION_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

$(GO4AN_LIB): $(GO4AN_O)
	@$(MakeLibrary) $(GO4AN_LIBNAME) "$(GO4AN_O)" $(GO4DLLPATH) "$(GO4AN_LINKDEFS)" "$(VERSION_LIB) $(GO4ANBASE_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

$(VERSION_LIB): $(VERSION_O)
	@$(MakeLibrary) $(VERSION_LIBNAME) "$(VERSION_O)" $(GO4DLLPATH)

$(GO4BGUI_LIB): $(GO4BGUI_O)
	@$(MakeLibrary) $(GO4BGUI_LIBNAME) "$(GO4BGUI_O)" $(GO4DLLPATH) "$(GO4BGUI_LINKDEFS)" "$(VERSION_LIB) $(GO4ANBASE_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

ifdef DOPACKAGE

package: go4-package

ifeq ($(MAKECMDGOALS), fast-packages)
FASTPACKAGING = true
endif

fast-packages: packages

packages: go4-package fit-package thrd-package task-package win-src

GO4PACK_VERS     = go4-$(MAJOR).$(MINOR)
GO4TAR_NAME      = go4-$(MAJOR).$(MINOR).tar
WINPACK_VERS     = go4
WINTAR_NAME      = go4win.tar
WINBIN_NAME      = go4winbin.zip
FITTAR_NAME      = Go4Fit.tar
QTROOTTAR_NAME   = Go4QtRoot.tar
THRDTAR_NAME     = threadmgr.tar
TASKTAR_NAME     = taskhandler.tar

DISTR_DIR  = ~/go4_temp_packaging
GO4DISTR_DIR  = $(DISTR_DIR)/$(GO4PACK_VERS)
WINDISTR_DIR  = $(DISTR_DIR)/$(WINPACK_VERS)

PACKAGE_DIR = ./packages

HDISTFILES = $(filter %.h %.cxx %.cpp %.c,$(subst $(GO4SYS),$(GO4DISTR_DIR),$(DISTRFILES)))

go4-package:
	@echo "Creating package $(GO4TAR_NAME) ..."
	@tar chf $(GO4TAR_NAME) Makefile.config Makefile.rules go4.init
	@tar rhf $(GO4TAR_NAME) build/*.sh build/Makefile.*
	@tar rhf $(GO4TAR_NAME) $(patsubst %,%/Module.mk,$(MODULES))
	@tar rhf $(GO4TAR_NAME) $(patsubst %,%/Makefile,$(EXMODULES))
	@tar rhf $(GO4TAR_NAME) $(subst $(GO4SYS),.,$(DISTRFILES))
	@tar rhf $(GO4TAR_NAME) README.txt CHANGES.txt Go4License.txt
	@tar rhf $(GO4TAR_NAME) etc/*.ksh etc/*.txt etc/*.C
	@tar rhf $(GO4TAR_NAME) docs/*.pdf
	@tar rhf $(GO4TAR_NAME) macros/*.C 
	@tar rhf $(GO4TAR_NAME) icons --exclude=*.svn
	@mkdir -p $(DISTR_DIR); cd $(DISTR_DIR); mkdir -p $(GO4PACK_VERS)
	@mv $(GO4TAR_NAME) $(GO4DISTR_DIR)
	@cd $(GO4DISTR_DIR); tar xf $(GO4TAR_NAME); rm -f $(GO4TAR_NAME)
	@cp -f distr/Makefile.distr $(GO4DISTR_DIR)/Makefile
	@cd $(GO4DISTR_DIR)/etc; mkdir -p win
	@cp -f distr/go4login.sh $(GO4DISTR_DIR)
	@cp -f distr/go4init.bat $(GO4DISTR_DIR)/etc/win
	@cp -f distr/README_win.txt $(GO4DISTR_DIR)/etc/win
ifndef FASTPACKAGING
	@for FILENAME in $(HDISTFILES); do . $(GO4SYS)/build/pack.ksh $$FILENAME; done
endif
	@mkdir -p $(GO4DISTR_DIR)/include
	@touch -f $(GO4DISTR_DIR)/include/.dummy
	@cd $(DISTR_DIR); chmod u+w *; chmod u+w */*; chmod u+w */*/*; tar chf $(GO4TAR_NAME) $(GO4PACK_VERS) ; gzip -f $(GO4TAR_NAME)
	@mkdir -p $(PACKAGE_DIR)
	@mv -f $(DISTR_DIR)/$(GO4TAR_NAME).gz $(PACKAGE_DIR)
	@rm -f -r $(DISTR_DIR)/*
	@rmdir $(DISTR_DIR)
	@echo "Package $(GO4TAR_NAME).gz done in $(PACKAGE_DIR)"


WINDISTRFILES = $(filter-out $(GO4SYS)/qt%, $(DISTRFILES))

HWINDISTFILES = $(filter %.h %.cxx %.cpp %.c,$(subst $(GO4SYS),$(WINDISTR_DIR),$(WINDISTRFILES)))

win-src:
	@echo "Creating package $(WINTAR_NAME) ..."
	@tar chf $(WINTAR_NAME) Makefile.rules Makefile.config go4.init
	@tar rhf $(WINTAR_NAME) build/*.sh build/Makefile.*
	@tar rhf $(WINTAR_NAME) $(patsubst %,%/Module.mk,$(MODULES))
	@tar rhf $(WINTAR_NAME) $(patsubst %,%/Makefile,$(EXMODULES))
	@tar rhf $(WINTAR_NAME) $(subst $(GO4SYS),.,$(WINDISTRFILES))
	@tar rhf $(WINTAR_NAME) CHANGES.txt Go4License.txt
	@tar rhf $(WINTAR_NAME) etc/*.ksh etc/*.txt etc/*.C
	@tar rhf $(WINTAR_NAME) docs/*.pdf
	@tar rhf $(WINTAR_NAME) macros/*.C
	@tar rhf $(WINTAR_NAME) icons --exclude=*.svn
	@mkdir -p $(DISTR_DIR); cd $(DISTR_DIR); mkdir -p $(WINPACK_VERS)
	@mv $(WINTAR_NAME) $(WINDISTR_DIR)
	@cd $(WINDISTR_DIR); tar xf $(WINTAR_NAME); rm -f $(WINTAR_NAME)
	@cp -f Makefile $(WINDISTR_DIR)/Makefile
	@cp -f distr/go4init.bat $(WINDISTR_DIR)
	@cp -f distr/README_win.txt $(WINDISTR_DIR)/README.txt
ifndef FASTPACKAGING
	@for FILENAME in $(HWINDISTFILES); do . $(GO4SYS)/build/pack.ksh $$FILENAME; done
endif
	@mkdir -p $(WINDISTR_DIR)/include
	@touch -f $(WINDISTR_DIR)/include/.dummy
	@cd $(DISTR_DIR); chmod u+w *; chmod u+w */*; chmod u+w */*/*; tar chf $(WINTAR_NAME) $(WINPACK_VERS) --exclude=$(WINTAR_NAME)*; gzip -f $(WINTAR_NAME)
	@mkdir -p $(PACKAGE_DIR)
	@mv -f $(DISTR_DIR)/$(WINTAR_NAME).gz $(PACKAGE_DIR)
	@rm -f -r $(DISTR_DIR)/*
	@rmdir $(DISTR_DIR)
#	@cd $(PACKAGE_DIR); gunzip -f $(WINTAR_NAME).gz
	@echo "Package $(WINTAR_NAME).gz done in $(PACKAGE_DIR)"

win-bin: 
	@echo "Creating binary Go4 Windows package ..."
	@cd ..; zip $(WINBIN_NAME) go4/.rootmap
	@for MMM in $(EXMODULES); do cd ..; echo "pack $$MMM"; rm -f fff.txt; \
	ls -dA1 go4/$$MMM/*.$(ObjSuf) go4/$$MMM/*.$(DepSuf) go4/$$MMM/$(DICT_PREFIX)* go4/$$MMM/.localmap > fff.txt 2>/dev/null; \
	zip -r $(WINBIN_NAME) go4/$$MMM -x@fff.txt; \
	rm -f fff.txt; cd go4; done
ifeq ($(wildcard ../root/etc/system.rootmap),)
	cp .rootmap ../root/etc/system.rootmap
else
	mv -f ../root/etc/system.rootmap ../root/etc/system.rootmap.copy
	cat .rootmap ../root/etc/system.rootmap.copy > ../root/etc/system.rootmap
endif    
	cp -f go4init.bat ..
	cd ..; zip -r $(WINBIN_NAME) \
	go4init.bat go4/README.txt go4/Go4License.txt \
	go4/Makefile.config go4/Makefile.rules \
	go4/include/*.h go4/build go4/images \
	go4/etc go4/bin go4/lib go4/docs go4/macros root
	rm -f ../go4init.bat
ifneq ($(wildcard ../root/etc/system.rootmap.copy),)
	mv -f ../root/etc/system.rootmap.copy ../root/etc/system.rootmap
else
	rm -f ../root/etc/system.rootmap
endif
	@echo "Binary package $(WINBIN_NAME) done"

COMPFILES = $(filter %.h %.cxx %.cpp %.c, $(DISTRFILES))

compress:
	@for FILENAME in $(COMPFILES); do . $(GO4SYS)/build/compress.ksh $$FILENAME; done
	@echo "Compression done"


FITHDISTFILES = $(filter %.h %.cxx %.cpp %.c,$(subst $(GO4SYS),$(DISTR_DIR),$(FITDISTRFILES) $(FITEXDISTRFILES)))

fit-package:
	@echo "Creating package $(FITTAR_NAME) ..."
	@tar chf $(FITTAR_NAME) Go4Fit/changes.txt
	@tar rhf $(FITTAR_NAME) Go4Fit/init_script.sh
	@tar rhf $(FITTAR_NAME) $(subst $(GO4SYS),.,$(FITDISTRFILES))
	@tar rhf $(FITTAR_NAME) $(subst $(GO4SYS),.,$(FITEXDISTRFILES))
	@mkdir -p $(DISTR_DIR)
	@mv $(FITTAR_NAME) $(DISTR_DIR)
	@cd $(DISTR_DIR); tar xf $(FITTAR_NAME)
	@cp -f Go4Fit/Makefile.standalone $(DISTR_DIR)/Go4Fit/Makefile
	@cp -f Go4FitExample/Makefile.standalone $(DISTR_DIR)/Go4FitExample/Makefile
ifndef FASTPACKAGING
	@for FILENAME in $(FITHDISTFILES); do . $(GO4SYS)/build/pack.ksh $$FILENAME $(GO4SYS)/Go4Fit/header.txt; done
endif
	@cd $(DISTR_DIR); rm -f $(FITTAR_NAME); chmod u+w *; chmod u+w */*; tar chf $(FITTAR_NAME) Go4Fit Go4FitExample --exclude=$(FITTAR_NAME)*; gzip -f $(FITTAR_NAME)
	@mkdir -p $(PACKAGE_DIR)
	@mv -f $(DISTR_DIR)/$(FITTAR_NAME).gz $(PACKAGE_DIR)
	@rm -f -r $(DISTR_DIR)/*
	@rmdir $(DISTR_DIR)
	@echo "Package $(FITTAR_NAME).gz done in $(PACKAGE_DIR)"

THRDMODULES = Go4Exceptions Go4LockGuard Go4Log Go4ThreadManager 
THRDMODULESEX = Go4ThreadManagerExample
THRDDISTRFILES = $(EXCEPT_DISTRFILES) $(LOCKGRD_DISTRFILES) $(GO4LOG_DISTRFILES) \
                 $(THRDMNGR_DISTRFILES) $(THRDMNGREXAMP_DISTRFILES) 
THRDHDISTFILES = $(filter %.h %.cxx %.cpp %.c,$(subst $(GO4SYS),$(DISTR_DIR),$(THRDDISTRFILES)))

thrd-package:
	@echo "Creating package $(THRDTAR_NAME) ..."
	@tar chf $(THRDTAR_NAME) Makefile.config
	@tar rhf $(THRDTAR_NAME) Makefile.rules
	@tar rhf $(THRDTAR_NAME) ./build/*.sh build/Makefile.*
	@tar rhf $(THRDTAR_NAME) $(patsubst %,%/Module.mk,$(THRDMODULES))
	@tar rhf $(THRDTAR_NAME) $(patsubst %,%/Makefile,$(THRDMODULESEX))
	@tar rhf $(THRDTAR_NAME) $(subst $(GO4SYS),.,$(THRDDISTRFILES))
	@tar rhf $(THRDTAR_NAME) ./Go4License.txt
	@tar rhf $(THRDTAR_NAME) ./etc/Go4ClientStartup.ksh
	@mkdir -p $(DISTR_DIR)
	@mv $(THRDTAR_NAME) $(DISTR_DIR)
	@cd $(DISTR_DIR); tar xf $(THRDTAR_NAME)
	@mkdir -p $(DISTR_DIR)/include
	@touch -f $(DISTR_DIR)/include/.dummy
	@cp -f distr/Makefile_threadmanager $(DISTR_DIR)/Makefile
	@cp -f distr/go4.taskh.init $(DISTR_DIR)/go4.init
	@cp -f distr/README_threadm.txt $(DISTR_DIR)/README.txt
ifndef FASTPACKAGING
	@for FILENAME in $(THRDHDISTFILES); do . $(GO4SYS)/build/pack.ksh $$FILENAME; done
endif
	@cd $(DISTR_DIR); rm -f $(THRDTAR_NAME); chmod u+w *; chmod u+w */*; tar chf $(THRDTAR_NAME) * */* --exclude=$(THRDTAR_NAME)*; gzip -f $(THRDTAR_NAME)
	@mkdir -p $(PACKAGE_DIR)
	@mv -f $(DISTR_DIR)/$(THRDTAR_NAME).gz $(PACKAGE_DIR)
	@rm -f -r $(DISTR_DIR)/*
	@rmdir $(DISTR_DIR)
	@echo "Package $(THRDTAR_NAME).gz done in $(PACKAGE_DIR)"

TASKMODULES = Go4Exceptions Go4LockGuard Go4Log \
              Go4ThreadManager \
              Go4Queue Go4Socket Go4StatusBase \
              Go4CommandsBase Go4CommandsTaskHandler \
              Go4TaskHandler 
TASKMODULESEX = Go4ThreadManagerExample Go4TaskHandlerExample
              
TASKDISTRFILES = $(EXCEPT_DISTRFILES) $(LOCKGRD_DISTRFILES) $(GO4LOG_DISTRFILES) \
                 $(THRDMNGR_DISTRFILES) $(THRDMNGREXAMP_DISTRFILES) \
                 $(GO4QUEUE_DISTRFILES) $(GO4SOCKET_DISTRFILES) $(STATBASE_DISTRFILES) \
                 $(COMBASE_DISTRFILES) $(CMDTASKHANDL_DISTRFILES) \
                 $(TASKHAND_DISTRFILES) $(TASKHANDEXAMP_DISTRFILES)
                 
TASKHDISTFILES = $(filter %.h %.cxx %.cpp %.c,$(subst $(GO4SYS),$(DISTR_DIR),$(TASKDISTRFILES)))

task-package:
	@echo "Creating package $(TASKTAR_NAME) ..."
	@tar chf $(TASKTAR_NAME) Makefile.config
	@tar rhf $(TASKTAR_NAME) Makefile.rules
	@tar rhf $(TASKTAR_NAME) ./build/*.sh  build/Makefile.*
	@tar rhf $(TASKTAR_NAME) $(patsubst %,%/Module.mk,$(TASKMODULES))
	@tar rhf $(TASKTAR_NAME) $(patsubst %,%/Makefile,$(TASKMODULESEX))
	@tar rhf $(TASKTAR_NAME) $(subst $(GO4SYS),.,$(TASKDISTRFILES))
	@tar rhf $(TASKTAR_NAME) ./Go4License.txt
	@tar rhf $(TASKTAR_NAME) ./etc/Go4ClientStartup.ksh
	@mkdir -p $(DISTR_DIR)
	@mv $(TASKTAR_NAME) $(DISTR_DIR)
	@cd $(DISTR_DIR); tar xf $(TASKTAR_NAME)
	@cp -f distr/Makefile_taskhandler $(DISTR_DIR)/Makefile
	@cp -f distr/go4.taskh.init $(DISTR_DIR)/go4.init
	@cp -f distr/README_taskh.txt $(DISTR_DIR)/README.txt
	@mkdir -p $(DISTR_DIR)/include
	@touch -f $(DISTR_DIR)/include/.dummy
ifndef FASTPACKAGING
	@for FILENAME in $(TASKHDISTFILES); do . $(GO4SYS)/build/pack.ksh $$FILENAME; done
endif
	@cd $(DISTR_DIR); rm -f $(TASKTAR_NAME); chmod u+w *; chmod u+w */*; tar chf $(TASKTAR_NAME)  * */* --exclude=$(TASKTAR_NAME)*; gzip -f $(TASKTAR_NAME)
	@mkdir -p $(PACKAGE_DIR)
	@mv -f $(DISTR_DIR)/$(TASKTAR_NAME).gz $(PACKAGE_DIR)
	@rm -f -r $(DISTR_DIR)/*
	@rmdir $(DISTR_DIR)
	@echo "Package $(TASKTAR_NAME).gz done in $(PACKAGE_DIR)"

endif

Go4_Makefile_Rules =
include Makefile.rules

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include build/dummy.d
endif
