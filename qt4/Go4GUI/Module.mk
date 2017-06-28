GO4GUI4_NAME        = Go4GUI

## normally should be like this for every module, but can be specific

GO4GUI4_DIR         = qt4/$(GO4GUI4_NAME)

GO4FITGUI4_DIR      = qt4/Go4FitGUI

GO4GUI4_QTPRO       = go4gui.pro
GO4GUI4_QTMAKE      = Makefile.qt
GO4GUI4_EXES        = $(GO4GUI4_DIR)/MainGo4GUI.cpp
GO4GUI4_EXE         = bin/go4

GO4GUI4_RESOURCES   = $(GO4GUI4_DIR)/go4icons.qrc
GO4GUI4_GEN_QRC     = $(GO4GUI4_DIR)/qrc_go4icons.cpp
GO4GUI4_NOTLIBF     = $(GO4GUI4_GEN_QRC) $(wildcard $(GO4GUI4_DIR)/$(DICT_PREFIX)*.*)

## must be similar for every module

$(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE) : LDRPATHS += $(if $(USEDIM), $(DIMLIBPATH),)

GO4GUI4_QMAKEFLAGS = "unix:QMAKE_LFLAGS += $(LDFLAGS_RPATH)" 

ifdef force32
ifeq ($(GO4_OS),Darwin)
GO4GUI4_QMAKEFLAGS = "unix:QMAKE_LFLAGS += -Wl,-arch,i386"
endif
endif


ifdef USEDIM
GO4GUI4_FORMS       = $(wildcard $(GO4GUI4_DIR)/*.ui)
GO4GUI4_QMAKEFLAGS += "FORMS += TGo4DabcMonitor.ui" 
GO4GUI4_QMAKEFLAGS += "HEADERS += TGo4DabcMonitor.h" 
GO4GUI4_QMAKEFLAGS += "SOURCES += TGo4DabcMonitor.cpp" 
GO4GUI4_QMAKEFLAGS += "INCLUDEPATH += $(DIMINCPATH)"
else
GO4GUI4_FORMS       = $(filter-out $(GO4GUI4_DIR)/TGo4DabcMonitor.ui, $(wildcard $(GO4GUI4_DIR)/*.ui))
endif

GO4GUI4_PACKAGE_FORMS = $(wildcard $(GO4GUI4_DIR)/*.ui)

GO4GUI4_FH          = $(GO4GUI4_FORMS:.ui=.h)
GO4GUI4_UI_PRIVH    = $(patsubst $(GO4GUI4_DIR)/%.ui, $(GO4GUI4_DIR)/ui_%.h, $(GO4GUI4_FORMS))
GO4GUI4_UI_PUBH     = $(patsubst $(GO4GUI4_DIR)/%.ui, include/ui_%.h, $(GO4GUI4_FORMS))
GO4GUI4_FS          = $(GO4GUI4_FORMS:.ui=.cpp)

GO4GUI4_S           = $(filter-out $(GO4GUI4_EXES) $(GO4GUI4_NOTLIBF), $(wildcard $(GO4GUI4_DIR)/*.$(SrcSuf)))
GO4GUI4_H           = $(GO4GUI4_S:.$(SrcSuf)=.$(HedSuf))
GO4GUI4_QTS         = $(filter-out $(GO4GUI4_EXES) $(GO4GUI4_NOTLIBF), $(wildcard $(GO4GUI4_DIR)/*.cpp))
GO4GUI4_QTH         = $(GO4GUI4_QTS:.cpp=.h)


GO4FITGUI4_S        = $(wildcard $(GO4FITGUI4_DIR)/*.cpp)
GO4FITGUI4_H        = $(wildcard $(GO4FITGUI4_DIR)/*.h)
GO4FITGUI4_FORMS    = $(wildcard $(GO4FITGUI4_DIR)/*.ui)



GO4GUI4_PUBH        = $(patsubst $(GO4GUI4_DIR)/%.h, include/%.h, $(GO4GUI4_H) $(GO4GUI4_QTH) $(GO4GUI4_FH))


QT4ROOT_DIR       = qt4/Go4QtRoot

QT4ROOT_H         = $(QT4ROOT_DIR)/QRootApplication.h \
                    $(QT4ROOT_DIR)/QRootCanvas.h \
                    $(QT4ROOT_DIR)/QRootWindow.h \
                    $(QT4ROOT_DIR)/QRootDialog.h
QT4ROOT_S         = $(QT4ROOT_H:.h=.cpp)

QT4ROOT_PUBH    = $(patsubst $(QT4ROOT_DIR)/%.h, include/%.h, $(QT4ROOT_H))


# used in the main Makefile

GO4QT4HEADS += $(GO4GUI4_PUBH) $(QT4ROOT_PUBH)

ifdef DOPACKAGE
DISTRFILES         += $(GO4GUI4_S) $(GO4GUI4_H) $(GO4GUI4_FH) 
DISTRFILES         += $(GO4GUI4_PACKAGE_FORMS) $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO) $(GO4GUI4_RESOURCES)
DISTRFILES         += $(GO4GUI4_QTS) $(GO4GUI4_QTH) $(GO4GUI4_EXES)
DISTRFILES         += $(GO4GUI4_DIR)/Module.mk
DISTRFILES         += $(QT4ROOT_H) $(QT4ROOT_S)
DISTRFILES         += $(GO4FITGUI4_S) $(GO4FITGUI4_H) $(GO4FITGUI4_FORMS)
endif

##### local rules #####

ifneq ($(GO4_QT), 3)

$(GO4GUI4_UI_PRIVH): | qt4-GUI 

include/%.h: $(QT4ROOT_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

include/%.h: $(GO4GUI4_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@


endif

ifneq ($(wildcard $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO)),)
$(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE): $(GO4GUI4_DIR)/$(GO4GUI4_QTPRO) $(GO4GUI4_FORMS)
	@echo "Generating $(GO4GUI4_QTMAKE)"
	cd $(GO4GUI4_DIR); $(QMAKE) $(GO4GUI4_QTPRO) -o $(GO4GUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(LIBS_GUISET)" $(GO4GUI4_QMAKEFLAGS)
endif

$(GO4GUI4_EXE) : $(GO4QT4HEADS) $(BUILDGO4LIBS) $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE) \
                 $(GO4GUI4_FORMS) $(GO4GUI4_S) $(GO4GUI4_EXES) $(GO4GUI4_H) $(GO4GUI4_QTS) $(GO4GUI4_QTH) \
                 $(QT4ROOT_H) $(QT4ROOT_S) $(GO4FITGUI4_S) $(GO4FITGUI4_H) $(GO4FITGUI4_FORMS) 
	@echo "Generating Qt4 part of the MainGUI..."
	+cd $(GO4GUI4_DIR); $(MAKEFORQT) -f $(GO4GUI4_QTMAKE)


qt4-GUI: $(GO4GUI4_EXE)

qt4-heads: $(GO4GUI4_UI_PUBH) 

clean-qt4-GUI-bin:
ifneq ($(MAKEFORQT),)
ifneq ($(wildcard $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE)),)
	cd $(GO4GUI4_DIR); $(MAKEFORQT) -f $(GO4GUI4_QTMAKE) clean
endif
endif
	@$(RM) $(GO4GUI4_DIR)/$(GO4GUI4_QTMAKE)*
	@$(RMDIR) $(GO4GUI4_DIR)/.obj $(GO4GUI4_DIR)/.moc
	@$(RM) $(GO4GUI4_GEN_QRC) $(GO4GUI4_DIR)/.qmake.stash
ifeq ($(GO4_OS),Win32)
	@$(RMDIR) $(GO4GUI4_DIR)/release $(GO4GUI4_DIR)/debug 
	@$(RM) $(GO4GUI4_DIR)/go4_resource.rc
endif

clean-qt4-GUI: clean-qt4-GUI-bin
	@$(RM) $(GO4GUI4_EXE)
	@$(RM) $(GO4GUI4_UI_PUBH) $(GO4GUI4_PUBH) $(QT4ROOT_PUBH)
	@echo "Clean qt4 gui done"

