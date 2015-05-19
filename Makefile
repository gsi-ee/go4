ifndef GO4SYS
  GO4SYS = .
endif

GO4PACKAGE = go4

# rules which are used to produce packages
PACKAGERULES    = compress go4-package fit-package \
                  thrd-package task-package win-bin \
                  fast-packages packages 

ifneq ($(findstring $(MAKECMDGOALS), $(PACKAGERULES)),)
DOPACKAGE = true
endif

FASTRULES   += clean-qt3 clean-qt4 clean-bak clean-dep \
               clean-plugin clean-bin clean-prefix clean-svn \
               $(PACKAGERULES)

.PHONY:  all includes libs gui plugin install uninstall \
         clean clean-qt3 clean-qt4 clean-bak clean-plugin clean-mainlibs clean-prefix clean-svn \
         package $(PACKAGERULES)


all::        gui


include build/Makefile.discover

ifeq ($(GO4_OS), Win32)
GO4_GENERATED_FILES = go4login.bat  go4.bat
else
GO4_GENERATED_FILES = go4login  bin/go4-config
endif

include Makefile.config


# scan old dict to exclude them from makefile 
OLD_DICT =  $(wildcard */$(DICT_PREFIX)*.*)


ifdef QTPATH
ifeq ($(wildcard $(QTPATH)/*),)
$(error No files found in $(QTPATH), check your Qt installation)
endif
endif


Go4_Makefile_Rules = included

ifdef GO4_WIN32
  GO4SYS = .
endif   

# new staff concerning big libraries

GO4BASE_LIBNAME  = $(LIB_PREFIX)Go4Base
GO4BASE_LIB      = $(GO4DLLPATH)/$(GO4BASE_LIBNAME).$(DllSuf)

THRDMNGR_LIBNAME = $(LIB_PREFIX)Go4ThreadManager
THRDMNGR_LIB     = $(GO4DLLPATH)/$(THRDMNGR_LIBNAME).$(DllSuf)

GO4TSKH_LIBNAME  = $(LIB_PREFIX)Go4TaskHandler
GO4TSKH_LIB      = $(GO4DLLPATH)/$(GO4TSKH_LIBNAME).$(DllSuf)

GO4ANBASE_LIBNAME = $(LIB_PREFIX)Go4AnalBase
GO4ANBASE_LIB     = $(GO4DLLPATH)/$(GO4ANBASE_LIBNAME).$(DllSuf)

GO4AN_LIBNAME   = $(LIB_PREFIX)Go4Analysis
GO4AN_LIB       = $(GO4DLLPATH)/$(GO4AN_LIBNAME).$(DllSuf)

GO4BGUI_LIBNAME = $(LIB_PREFIX)Go4GUI
GO4BGUI_LIB     = $(GO4DLLPATH)/$(GO4BGUI_LIBNAME).$(DllSuf)


BUILDGO4LIBS += $(GO4FIT_LIB) \
                $(GO4BASE_LIB) \
                $(THRDMNGR_LIB) \
                $(GO4TSKH_LIB) \
                $(GO4ANBASE_LIB) \
                $(GO4AN_LIB) \
                $(GO4BGUI_LIB)
               
MODULES  = MbsAPIbase MbsAPI RawAPI  \
           Go4Fit Go4Log Go4LockGuard \
           Go4ThreadManager \
           Go4CommandsAnalysis Go4CommandsBase  \
           Go4CommandsTaskHandler Go4TaskHandler \
           Go4ConditionsBase Go4DynamicList Go4Event \
           Go4EventServer Go4Exceptions \
           Go4Display Go4Proxies \
           Go4ObjectManager \
           Go4Queue Go4Socket Go4StatusAnalysis Go4StatusBase \
           Go4Http Go4HistogramServer Go4Analysis Go4AnalysisClient Go4Dict
           
EXMODULES = Go4ExampleSimple Go4Example1Step Go4Example2Step Go4ExampleAdvanced \
            Go4ExampleUserSource Go4ExampleMesh Go4FitExample \
            Go4ThreadManagerExample Go4TaskHandlerExample Go4EventServerExample

include $(patsubst %,%/Module.mk,$(MODULES))

include $(patsubst %,%/Makefile, $(EXMODULES))

-include qt3/Module.mk

-include qt4/Module.mk

bin/go4-config: Makefile $(GO4SYS)/build/dummy.d
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

$(GO4SYS)/build/dummy.d: Makefile $(GO4QTHEADS) $(ALLHDRS)
	@(if [ ! -d bin ] ; then mkdir -p bin; fi)
	@(if [ ! -d lib ] ; then mkdir -p lib; fi)
	@(if [ ! -f $(GO4MAP) ] ; then touch $(GO4MAP); fi)
	@(if [ ! -f $@ ] ; then touch $@; fi)

libs::    $(BUILDGO4LIBS) $(GO4_GENERATED_FILES)

gui::    libs 
ifdef ISROOT6
		@echo "detect ROOT6"
endif
ifdef GO4PREFIX
	@echo Call make install to copy all binary files into $(GO4PREFIX)
endif


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
	@mkdir -p $(GO4TOPPATH)/qt4
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
	@rm -f $(GO4MAP) */$(DICT_PREFIX)*.*
	@rm -f include/*.h
	@rm -rf bin lib
	@rm -f build/dummy.d build/Makefile.gener go4login go4login.bat go4.bat
	@echo "Clean go4 done"

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

clean-svn:
	find . -name ".svn" -type d -exec rm -rf {} \;
	@echo "Clean svn-specific files done"

ifdef DOPACKAGE
-include build/Makefile.packaging
endif

Go4_Makefile_Rules =
include Makefile.rules

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include build/dummy.d
endif
