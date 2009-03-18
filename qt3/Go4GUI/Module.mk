GO4GUI_NAME        = Go4GUI
MODULE_NAME         = $(GO4GUI_NAME)

## normally should be like this for every module, but can be specific

GO4GUI_DIR         = $(GO4SYS)/qt3/$(GO4GUI_NAME)
GO4GUI_LINKDEF     = $(GO4GUI_DIR)/$(GO4GUI_NAME)LinkDef.$(HedSuf)
GO4GUI_QTLINKDEF   = $(GO4GUI_DIR)/Go4QtGUILinkDef.$(HedSuf)

GO4GUI_QMAKED1OPT   =
GO4GUI_QMAKED2OPT   =

GO4GUI_QTPRO       = $(GO4GUI_DIR)/go4gui.pro
GO4GUI_QTMAKE      = $(GO4GUI_DIR)/Makefile.qt
GO4GUI_EXES        = $(GO4GUI_DIR)/MainGo4GUI.cpp

GO4GUI_NOTLIBF     =

## must be similar for every module

GO4GUI_DICT        = $(GO4GUI_DIR)/$(DICT_PREFIX)$(GO4GUI_NAME)
GO4GUI_DH          = $(GO4GUI_DICT).$(HedSuf)
GO4GUI_DS          = $(GO4GUI_DICT).$(SrcSuf)
GO4GUI_DO          = $(GO4GUI_DICT).$(ObjSuf)

ifdef USEDIM
GO4GUI_FORMS       = $(wildcard $(GO4GUI_DIR)/*.ui)
GO4GUI_FORMSI      = $(wildcard $(GO4GUI_DIR)/*.ui.h)
GO4GUI_QMAKED1OPT  = "FORMS += TGo4DabcMonitor.ui"
GO4GUI_QMAKED2OPT  = "INCLUDEPATH += $(DIMDIR)/dim"
else
GO4GUI_FORMS       = $(filter-out $(GO4GUI_DIR)/TGo4DabcMonitor.ui, $(wildcard $(GO4GUI_DIR)/*.ui))
GO4GUI_FORMSI      = $(filter-out $(GO4GUI_DIR)/TGo4DabcMonitor.ui.h, $(wildcard $(GO4GUI_DIR)/*.ui.h))
endif

GO4GUI_FH          = $(GO4GUI_FORMS:.ui=.h)
GO4GUI_FS          = $(GO4GUI_FORMS:.ui=.cpp)
GO4GUI_S           = $(filter-out $(GO4GUI_DS), $(wildcard $(GO4GUI_DIR)/*.$(SrcSuf)))
GO4GUI_H           = $(GO4GUI_S:.$(SrcSuf)=.$(HedSuf))
GO4GUI_QTS         = $(filter-out $(GO4GUI_EXES) $(GO4GUI_FS) $(GO4GUI_DIR)/qmake_image_collection.cpp, $(wildcard $(GO4GUI_DIR)/*.cpp))
GO4GUI_QTH         = $(GO4GUI_QTS:.cpp=.h)
GO4GUI_O           = $(GO4GUI_S:.$(SrcSuf)=.$(ObjSuf))

GO4GUI_DEP         = $(GO4GUI_O:.$(ObjSuf)=.$(DepSuf))
GO4GUI_DDEP        = $(GO4GUI_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4GUI_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4GUI_H) $(GO4GUI_QTH))

LIBDEPENDENC       += $(GO4GUI_DEP) $(GO4GUI_DDEP)

GO4QTHEADS         += $(GO4GUI_FH)
GO4QTTAGS          += qt-$(GO4GUI_NAME)

ifdef DOPACKAGE
DISTRFILES         += $(GO4GUI_S) $(GO4GUI_H) $(GO4GUI_LINKDEF) 
DISTRFILES         += $(GO4GUI_FORMS) $(GO4GUI_FORMSI) $(GO4GUI_QTPRO)
DISTRFILES         += $(GO4GUI_QTS) $(GO4GUI_QTH) $(GO4GUI_EXES)
ifndef USEDIM
DISTRFILES         += $(GO4GUI_DIR)/TGo4DabcMonitor.ui $(GO4GUI_DIR)/TGo4DabcMonitor.ui.h
endif
endif

##### local rules #####

$(GO4GUI_O) $(GO4GUI_DEP) : INCLUDES += -I$(QTDIR)/include

$(GO4SYS)/include/%.h: $(GO4GUI_DIR)/%.h
	@cp -f $< $@

$(GO4GUI_DS): $(GO4GUI_H)  $(GO4GUI_LINKDEF)
	@$(ROOTCINTGO4) $(GO4GUI_H) $(GO4GUI_LINKDEF)

ifneq ($(wildcard $(GO4GUI_QTPRO)),)
$(GO4GUI_QTMAKE): $(GO4GUI_QTPRO) $(GO4GUI_FORMS)
	@echo "Generating $(GO4GUI_QTMAKE)"
	cd $(GO4GUI_DIR); $(QMAKE) $(GO4GUI_QTPRO) -o $(GO4GUI_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(LIBS_GUISET)" $(GO4GUI_QMAKED1OPT) $(GO4GUI_QMAKED2OPT)
endif

qt-$(GO4GUI_NAME): $(BUILDGO4LIBS) $(GO4GUI_QTMAKE) qt-$(FITGUI_NAME)
	@echo "Generating Qt part of the MainGUI..."
	+cd $(GO4GUI_DIR); $(MAKE) -f $(GO4GUI_QTMAKE)

all-$(GO4GUI_NAME):  $(GO4GUI_FH) $(GO4GUI_O) $(GO4GUI_DO) qt-$(GO4GUI_NAME)

clean-obj-$(GO4GUI_NAME):
	@rm -f $(GO4GUI_O) $(GO4GUI_DO)
	@rm -f $(GO4GUI_DIR)/.obj/*.o
	@rm -f $(GO4SYS)/bin/go4

clean-$(GO4GUI_NAME): clean-obj-$(GO4GUI_NAME)
	@rm -f $(GO4GUI_DEP) $(GO4GUI_DDEP) $(GO4GUI_DS) $(GO4GUI_DH)
ifneq ($(wildcard $(GO4GUI_QTMAKE)),)
	cd $(GO4GUI_DIR); $(MAKE) -f $(GO4GUI_QTMAKE) distclean
endif
	@rm -f $(GO4GUI_QTMAKE) $(GO4GUI_FH)
