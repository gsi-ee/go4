GO4GUI3_NAME        = Go4GUI

## normally should be like this for every module, but can be specific

GO4GUI3_DIR         = $(GO4SYS)/qt3/$(GO4GUI3_NAME)
GO4GUI3_LINKDEF     = $(GO4GUI3_DIR)/$(GO4GUI3_NAME)LinkDef.$(HedSuf)
GO4GUI3_QTLINKDEF   = $(GO4GUI3_DIR)/Go4QtGUILinkDef.$(HedSuf)

GO4SGUI3_LIBNAME    = $(LIB_PREFIX)Go4GUI3bis
GO4SGUI3_SLIB       = $(GO4DLLPATH)/$(GO4SGUI3_LIBNAME).$(DllSuf)
GO4SGUI3_LIB        = $(GO4SGUI3_SLIB).$(VERSSUF)

GO4GUI3_QMAKED1OPT   =
GO4GUI3_QMAKED2OPT   =

GO4GUI3_QTPRO       = $(GO4GUI3_DIR)/go4gui.pro
GO4GUI3_QTMAKE      = $(GO4GUI3_DIR)/Makefile.qt
GO4GUI3_EXES        = $(GO4GUI3_DIR)/MainGo4GUI.cpp

GO4GUI3_NOTLIBF     =

## must be similar for every module

GO4GUI3_DICT        = $(GO4GUI3_DIR)/$(DICT_PREFIX)$(GO4GUI3_NAME)
GO4GUI3_DH          = $(GO4GUI3_DICT).$(HedSuf)
GO4GUI3_DS          = $(GO4GUI3_DICT).$(SrcSuf)
GO4GUI3_DO          = $(GO4GUI3_DICT).$(ObjSuf)

ifdef USEDIM
GO4GUI3_FORMS       = $(wildcard $(GO4GUI3_DIR)/*.ui)
GO4GUI3_FORMSI      = $(wildcard $(GO4GUI3_DIR)/*.ui.h)
GO4GUI3_QMAKED1OPT  = "FORMS += TGo4DabcMonitor.ui"
GO4GUI3_QMAKED2OPT  = "INCLUDEPATH += $(DIMDIR)/dim"
else
GO4GUI3_FORMS       = $(filter-out $(GO4GUI3_DIR)/TGo4DabcMonitor.ui, $(wildcard $(GO4GUI3_DIR)/*.ui))
GO4GUI3_FORMSI      = $(filter-out $(GO4GUI3_DIR)/TGo4DabcMonitor.ui.h, $(wildcard $(GO4GUI3_DIR)/*.ui.h))
endif

GO4GUI3_FH          = $(GO4GUI3_FORMS:.ui=.h)
GO4GUI3_FS          = $(GO4GUI3_FORMS:.ui=.cpp)
GO4GUI3_S           = $(filter-out $(GO4GUI3_DS), $(wildcard $(GO4GUI3_DIR)/*.$(SrcSuf)))
GO4GUI3_H           = $(GO4GUI3_S:.$(SrcSuf)=.$(HedSuf))
GO4GUI3_QTS         = $(filter-out $(GO4GUI3_EXES) $(GO4GUI3_FS) $(GO4GUI3_DIR)/qmake_image_collection.cpp, $(wildcard $(GO4GUI3_DIR)/*.cpp))
GO4GUI3_QTH         = $(GO4GUI3_QTS:.cpp=.h)
GO4GUI3_O           = $(GO4GUI3_S:.$(SrcSuf)=.$(ObjSuf))

GO4GUI3_DEP         = $(GO4GUI3_O:.$(ObjSuf)=.$(DepSuf))
GO4GUI3_DDEP        = $(GO4GUI3_DO:.$(ObjSuf)=.$(DepSuf))

GO4GUI3_PUBH        = $(patsubst $(GO4GUI3_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4GUI3_H) $(GO4GUI3_QTH))

# used in the main Makefile

GO4QT3HEADS         += $(GO4GUI3_FH) $(GO4GUI3_PUBH)

GO4QT3DEP           += $(GO4GUI3_DEP) $(GO4GUI3_DDEP)


ifdef DOPACKAGE
DISTRFILES         += $(GO4GUI3_S) $(GO4GUI3_H) $(GO4GUI3_LINKDEF) 
DISTRFILES         += $(GO4GUI3_FORMS) $(GO4GUI3_FORMSI) $(GO4GUI3_QTPRO)
DISTRFILES         += $(GO4GUI3_QTS) $(GO4GUI3_QTH) $(GO4GUI3_EXES)
ifndef USEDIM
DISTRFILES         += $(GO4GUI3_DIR)/TGo4DabcMonitor.ui $(GO4GUI3_DIR)/TGo4DabcMonitor.ui.h
endif
endif

##### local rules #####

$(GO4GUI3_O) $(GO4GUI3_DEP) : INCLUDES += $(QTINCLUDES)
$(GO4GUI3_O) $(GO4GUI3_DEP) : DEFINITIONS += $(QTDEFINITIONS)

$(GO4SYS)/include/%.h: $(GO4GUI3_DIR)/%.h
	@cp -f $< $@

$(GO4GUI3_DS): $(GO4GUI3_H)  $(GO4GUI3_LINKDEF)
	@$(ROOTCINTGO4) $(GO4GUI3_H) $(GO4GUI3_LINKDEF)

$(GO4SGUI3_LIB): $(GO4GUI3_O) $(GO4GUI3_DO)
	@$(MakeLib) $(GO4SGUI3_LIBNAME) "$(GO4GUI3_O) $(GO4GUI3_DO)" $(GO4DLLPATH)

ifneq ($(wildcard $(GO4GUI3_QTPRO)),)
$(GO4GUI3_QTMAKE): $(GO4GUI3_QTPRO) $(GO4GUI3_FORMS)
	@echo "Generating $(GO4GUI3_QTMAKE)"
	cd $(GO4GUI3_DIR); $(QMAKE) $(GO4GUI3_QTPRO) -o $(GO4GUI3_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(LIBS_GUISET) -lGo4GUI3bis -lGo4Qt3Root -lGo4FitGUI3" $(GO4GUI3_QMAKED1OPT) $(GO4GUI3_QMAKED2OPT)
endif

qt3-GUI: $(GO4QT3HEADS) $(BUILDGO4LIBS) $(GO4SGUI3_LIB) qt3-interface qt3-FitGUI $(GO4GUI3_QTMAKE)
	@echo "Generating Qt3 part of the MainGUI..."
	+cd $(GO4GUI3_DIR); $(MAKE) -f $(GO4GUI3_QTMAKE)

clean-qt3-GUI:
	@rm -f $(GO4GUI3_O) $(GO4GUI3_DO)
	@$(CleanLib) $(GO4SGUI3_LIBNAME) $(GO4DLLPATH)
	@rm -f $(GO4GUI3_DIR)/.obj/*.o
	@rm -f $(GO4SYS)/bin/go4
	@rm -f $(GO4GUI3_DEP) $(GO4GUI3_DDEP) $(GO4GUI3_DS) $(GO4GUI3_DH)
ifneq ($(wildcard $(GO4GUI3_QTMAKE)),)
	cd $(GO4GUI3_DIR); $(MAKE) -f $(GO4GUI3_QTMAKE) distclean
endif
	@rm -f $(GO4GUI3_QTMAKE) $(GO4GUI3_FH) $(GO4GUI3_PUBH)
