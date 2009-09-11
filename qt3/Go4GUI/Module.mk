GO4GUI3_NAME        = Go4GUI

## normally should be like this for every module, but can be specific

GO4GUI3_DIR         = qt3/$(GO4GUI3_NAME)
GO4GUI3_LINKDEF     = $(GO4GUI3_DIR)/$(GO4GUI3_NAME)LinkDef.$(HedSuf)
GO4GUI3_QTLINKDEF   = $(GO4GUI3_DIR)/Go4QtGUILinkDef.$(HedSuf)

FITGUI3_DIR         = qt3/Go4FitGUI

GO4GUI3_QTPRO       = go4gui.pro
GO4GUI3_QTMAKE      = Makefile.qt
GO4GUI3_EXES        = $(GO4GUI3_DIR)/MainGo4GUI.cpp

GO4GUI3_NOTLIBF     =

## must be similar for every module

GO4GUI3_DICT        = $(GO4GUI3_DIR)/$(DICT_PREFIX)$(GO4GUI3_NAME)
GO4GUI3_DH          = $(GO4GUI3_DICT).$(HedSuf)
GO4GUI3_DS          = $(GO4GUI3_DICT).$(SrcSuf)
GO4GUI3_DO          = $(GO4GUI3_DICT).$(ObjSuf)

$(GO4GUI3_DIR)/$(GO4GUI3_QTMAKE) : LDRPATHS := $(ROOTLIBPATH) $(GO4LIBPATH) $(if $(USEDIM), $(DIMLIBPATH),)

GO4GUI3_QMAKEFLAGS = "QMAKE_LFLAGS += $(LDFLAGS_RPATH)" 

ifdef USEDIM
GO4GUI3_FORMS       = $(wildcard $(GO4GUI3_DIR)/*.ui)
GO4GUI3_FORMSI      = $(wildcard $(GO4GUI3_DIR)/*.ui.h)
GO4GUI3_QMAKEFLAGS += "FORMS += TGo4DabcMonitor.ui" 
GO4GUI3_QMAKEFLAGS += "INCLUDEPATH += $(DIMINCPATH)"
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

GO4GUI3_PUBH        = $(patsubst $(GO4GUI3_DIR)/%.h, include/%.h, $(GO4GUI3_H) $(GO4GUI3_QTH))


FITGUI3_FORMS       = $(wildcard $(FITGUI3_DIR)/*.ui)
FITGUI3_FORMSI      = $(wildcard $(FITGUI3_DIR)/*.ui.h)
FITGUI3_FH          = $(FITGUI3_FORMS:.ui=.h)
FITGUI3_FS          = $(FITGUI3_FORMS:.ui=.cpp)

FITGUI3_QTH         = $(FITGUI3_DIR)/QFitItem.h \
                      $(FITGUI3_DIR)/QFitWidget.h \
                      $(FITGUI3_DIR)/QFitNamedWidget.h \
                      $(FITGUI3_DIR)/QFitModelWidget.h
FITGUI3_QTS         = $(FITGUI3_QTH:.h=.cpp)


QT3ROOT_DIR       = qt3/Go4QtRoot

QT3ROOT_S       =  $(QT3ROOT_DIR)/qrootapplication.cpp \
                   $(QT3ROOT_DIR)/tqrootguifactory.cpp \
                   $(QT3ROOT_DIR)/tqcanvasimp.cpp \
                   $(QT3ROOT_DIR)/tqcanvasmenu.cpp \
                   $(QT3ROOT_DIR)/tqrootcanvas.cpp \
                   $(QT3ROOT_DIR)/tqrootwindow.cpp \
                   $(QT3ROOT_DIR)/tqrootdialog.cpp \
                   $(QT3ROOT_DIR)/tqapplication.cpp \
                   $(QT3ROOT_DIR)/qtframe.cpp

QT3ROOT_H       =  $(QT3ROOT_S:.cpp=.$(HedSuf))

QT3ROOT_PUBH    = $(patsubst $(QT3ROOT_DIR)/%.h, include/%.h, $(QT3ROOT_H))


# used in the main Makefile

GO4QT3HEADS         += $(GO4GUI3_FH) $(GO4GUI3_PUBH) $(QT3ROOT_PUBH)

GO4QT3DEP           += $(GO4GUI3_DEP) $(GO4GUI3_DDEP)


ifdef DOPACKAGE
DISTRFILES         += $(GO4GUI3_S) $(GO4GUI3_H) $(GO4GUI3_LINKDEF) 
DISTRFILES         += $(GO4GUI3_FORMS) $(GO4GUI3_FORMSI) $(GO4GUI3_DIR)/$(GO4GUI3_QTPRO)
DISTRFILES         += $(GO4GUI3_QTS) $(GO4GUI3_QTH) $(GO4GUI3_EXES)
DISTRFILES         += $(GO4GUI3_DIR)/Module.mk
DISTRFILES         += $(FITGUI3_FORMS) $(FITGUI3_FORMSI)
DISTRFILES         += $(FITGUI3_QTH) $(FITGUI3_QTS) 
DISTRFILES         += $(FITGUI3_DIR)/TGo4FitGuiTypes.h
DISTRFILES         += $(QT3ROOT_H) $(QT3ROOT_S)
DISTRFILES         += $(QT3ROOT_DIR)/CHANGELOG.txt
ifndef USEDIM
DISTRFILES         += $(GO4GUI3_DIR)/TGo4DabcMonitor.ui $(GO4GUI3_DIR)/TGo4DabcMonitor.ui.h
endif
endif

##### local rules #####

$(GO4GUI3_O) $(GO4GUI3_DEP) : CXXFLAGS += $(QTCXXFLAGS)

ifeq ($(GO4_QT), 3)
include/%.h: $(GO4GUI3_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

include/%.h: $(QT3ROOT_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@
endif

$(GO4GUI3_DS): $(GO4GUI3_H) $(GO4GUI3_LINKDEF)
	@$(ROOTCINTGO4) $(GO4GUI3_H) $(GO4GUI3_LINKDEF)

$(GO4GUI3_DIR)/$(GO4GUI3_QTMAKE): $(GO4GUI3_DIR)/$(GO4GUI3_QTPRO) $(GO4GUI3_FORMS)
	@echo "Generating $(GO4GUI3_QTMAKE)"
	cd $(GO4GUI3_DIR); $(QMAKE) $(GO4GUI3_QTPRO) -o $(GO4GUI3_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(LIBS_GUISET)" $(GO4GUI3_QMAKEFLAGS)

qt3-GUI: $(GO4QT3HEADS) libs $(GO4GUI3_DS) $(GO4GUI3_DIR)/$(GO4GUI3_QTMAKE)
	@echo "Generating Qt3 part of the MainGUI..."
	+cd $(GO4GUI3_DIR); $(MAKE) -f $(GO4GUI3_QTMAKE) "GO4SYS=../.."

clean-qt3-GUI-bin:
	@rm -f $(GO4GUI3_O) $(GO4GUI3_DEP) 
	@rm -f $(GO4GUI3_DIR)/.obj/*.o
	@rm -f $(GO4GUI3_DO) $(GO4GUI3_DDEP) $(GO4GUI3_DS) $(GO4GUI3_DH)
ifneq ($(wildcard $(GO4GUI3_DIR)/$(GO4GUI3_QTMAKE)),)
	cd $(GO4GUI3_DIR); $(MAKE) -f $(GO4GUI3_QTMAKE) clean "GO4SYS=../.."
endif
	@rm -f $(GO4GUI3_DIR)/$(GO4GUI3_QTMAKE) $(GO4GUI3_FH) $(FITGUI3_FH)
	@rm -rf $(GO4GUI3_DIR)/.obj $(GO4GUI3_DIR)/.moc

clean-qt3-GUI: clean-qt3-GUI-bin
	@rm -f $(GO4SYS)/bin/go4
	@rm -f $(GO4GUI3_PUBH) $(QT3ROOT_PUBH)
	@echo "Clean qt3 gui done"
