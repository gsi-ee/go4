GO4GUI4_NAME        = Go4GUI

## normally should be like this for every module, but can be specific

GO4GUI4_DIR         = $(GO4SYS)/qt4/$(GO4GUI4_NAME)
GO4GUI4_LINKDEF     = $(GO4GUI4_DIR)/$(GO4GUI4_NAME)LinkDef.$(HedSuf)
GO4GUI4_QTLINKDEF   = $(GO4GUI4_DIR)/Go4QtGUILinkDef.$(HedSuf)

GO4SGUI4_LIBNAME    = $(LIB_PREFIX)Go4GUI4bis
GO4SGUI4_LIB        = $(GO4DLLPATH)/$(GO4SGUI4_LIBNAME).$(DllSuf)

GO4GUI4_QMAKED1OPT   =
GO4GUI4_QMAKED2OPT   =
GO4GUI4_QMAKED3OPT   =
GO4GUI4_QMAKED4OPT   =


GO4GUI4_QTPRO       = go4gui.pro
GO4GUI4_QTMAKE      = Makefile.qt
GO4GUI4_EXES        = $(GO4GUI4_DIR)/MainGo4GUI.cpp

GO4GUI4_RESOURCES   = $(GO4GUI4_DIR)/go4icons.qrc
GO4GUI4_GEN_QRC     = $(GO4GUI4_DIR)/qrc_go4icons.cpp
GO4GUI4_NOTLIBF     = $(GO4GUI4_GEN_QRC)

## must be similar for every module

GO4GUI4_DICT        = $(GO4GUI4_DIR)/$(DICT_PREFIX)$(GO4GUI4_NAME)
GO4GUI4_DH          = $(GO4GUI4_DICT).$(HedSuf)
GO4GUI4_DS          = $(GO4GUI4_DICT).$(SrcSuf)
GO4GUI4_DO          = $(GO4GUI4_DICT).$(ObjSuf)


ifdef USEDIM
GO4GUI4_FORMS       = $(wildcard $(GO4GUI4_DIR)/*.ui)
GO4GUI4_QMAKED1OPT  = "FORMS += TGo4DabcMonitor.ui"
GO4GUI4_QMAKED2OPT  = "HEADERS += TGo4DabcMonitor.h"
GO4GUI4_QMAKED3OPT  = "SOURCES += TGo4DabcMonitor.cpp"
GO4GUI4_QMAKED4OPT  = "INCLUDEPATH += $(DIMDIR)/dim"

else
GO4GUI4_FORMS       = $(filter-out $(GO4GUI4_DIR)/TGo4DabcMonitor.ui, $(wildcard $(GO4GUI4_DIR)/*.ui))
endif

GO4GUI4_PACKAGE_FORMS = $(wildcard $(GO4GUI4_DIR)/*.ui)

GO4GUI4_FH          = $(GO4GUI4_FORMS:.ui=.h)
GO4GUI4_UI_H        = $(patsubst $(GO4GUI4_DIR)/%.ui, $(GO4GUI4_DIR)/ui_%.h, $(GO4GUI4_FORMS))
GO4GUI4_FS          = $(GO4GUI4_FORMS:.ui=.cpp)

GO4GUI4_S           = $(filter-out $(GO4GUI4_DS), $(wildcard $(GO4GUI4_DIR)/*.$(SrcSuf)))
GO4GUI4_H           = $(GO4GUI4_S:.$(SrcSuf)=.$(HedSuf))
GO4GUI4_QTS         = $(filter-out $(GO4GUI4_EXES) $(GO4GUI4_NOTLIBF), $(wildcard $(GO4GUI4_DIR)/*.cpp))
GO4GUI4_QTH         = $(GO4GUI4_QTS:.cpp=.h)
GO4GUI4_O           = $(GO4GUI4_S:.$(SrcSuf)=.$(ObjSuf))

GO4GUI4_DEP         = $(GO4GUI4_O:.$(ObjSuf)=.$(DepSuf))
GO4GUI4_DDEP        = $(GO4GUI4_DO:.$(ObjSuf)=.$(DepSuf))

GO4GUI4_PUBH        = $(patsubst $(GO4GUI4_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4GUI4_H) $(GO4GUI4_QTH) $(GO4GUI4_FH))


# used in the main Makefile

GO4QT4HEADS        += $(GO4GUI4_UI_H) $(GO4GUI4_PUBH)

GO4QT4DEP          += $(GO4GUI4_DEP) $(GO4GUI4_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4GUI4_S) $(GO4GUI4_H) $(GO4GUI4_FH) $(GO4GUI4_LINKDEF) 
DISTRFILES         += $(GO4GUI4_PACKAGE_FORMS) $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO) $(GO4GUI4_RESOURCES)
DISTRFILES         += $(GO4GUI4_QTS) $(GO4GUI4_QTH) $(GO4GUI4_EXES)
DISTRFILES         += $(GO4GUI4_DIR)/Module.mk
endif

##### local rules #####

$(GO4GUI4_O) $(GO4GUI4_DO) $(GO4GUI4_DEP) $(GO4GUI4_DDEP): CXXFLAGS += $(QTCXXFLAGS)

ifdef GO4_QT4
$(GO4SYS)/include/%.h: $(GO4GUI4_DIR)/%.h
	@cp -f $< $@
endif

$(GO4GUI4_DS): $(GO4GUI4_H)  $(GO4GUI4_LINKDEF)
	@$(ROOTCINTGO4) $(GO4GUI4_H) $(GO4GUI4_LINKDEF)

$(GO4SGUI4_LIB): $(GO4GUI4_O) $(GO4GUI4_DO)
	@$(MakeLibrary) $(GO4SGUI4_LIBNAME) "$(GO4GUI4_O) $(GO4GUI4_DO)" $(GO4DLLPATH)

$(GO4GUI4_DIR)/ui_%.h: $(GO4GUI4_DIR)/%.ui
	@echo "Producing file $@ ..." 
	@$(UIC) $< -o $@


ifneq ($(wildcard $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO)),)
$(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE): $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO) $(GO4GUI4_FORMS)
	@echo "Generating $(GO4GUI4_QTMAKE)"
	cd $(GO4GUI4_DIR); $(QMAKE) $(GO4GUI4_QTPRO) -o $(GO4GUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(LIBS_GUISET) -lGo4GUI4bis -lGo4Qt4Root -lGo4FitGUI4" $(GO4GUI4_QMAKED1OPT) $(GO4GUI4_QMAKED2OPT) $(GO4GUI4_QMAKED3OPT) $(GO4GUI4_QMAKED4OPT)
endif

qt4-GUI: $(GO4QT4HEADS) libs $(GO4SGUI4_LIB) qt4-interface qt4-FitGUI $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE)
	@echo "Generating Qt4 part of the MainGUI..."
	+cd $(GO4GUI4_DIR); $(MAKEFORQT) -f $(GO4GUI4_QTMAKE)

clean-qt4-GUI:
	@rm -f $(GO4GUI4_O) $(GO4GUI4_DO)
	@$(CleanLib) $(GO4SGUI4_LIBNAME) $(GO4DLLPATH)
	@rm -f $(GO4GUI4_DIR)/.obj/*.o
	@rm -f $(GO4SYS)/bin/go4
	@rm -f $(GO4GUI4_DEP) $(GO4GUI4_DDEP) $(GO4GUI4_DS) $(GO4GUI4_DH)
ifneq ($(wildcard $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE)),)
	cd $(GO4GUI4_DIR); $(MAKEFORQT) -f $(GO4GUI4_QTMAKE) clean
endif
	@rm -f $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE) $(GO4GUI4_PUBH) $(GO4GUI4_GEN_QRC)
	@rm -rf .obj .moc
	@echo "Clean qt4 gui done"
