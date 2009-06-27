## normally should be like this for every module, but can be specific

FITGUI3_DIR         = $(GO4SYS)/qt3/Go4FitGUI
FITGUI3_QTLIBNAME   = libGo4FitGUI3
FITGUI3_QTPRO       = $(FITGUI3_DIR)/Go4FitGUI3.pro
FITGUI3_QTMAKE      = $(FITGUI3_DIR)/Makefile.qt

FITGUI3_NOTLIBF     =

## must be similar for every module

FITGUI3_FORMS       = $(wildcard $(FITGUI3_DIR)/*.ui)
FITGUI3_FORMSI      = $(wildcard $(FITGUI3_DIR)/*.ui.h)
FITGUI3_FH          = $(FITGUI3_FORMS:.ui=.h)
FITGUI3_FS          = $(FITGUI3_FORMS:.ui=.cpp)

FITGUI3_QTH         = $(FITGUI3_DIR)/QFitItem.h \
                      $(FITGUI3_DIR)/QFitWidget.h \
                      $(FITGUI3_DIR)/QFitNamedWidget.h \
                      $(FITGUI3_DIR)/QFitModelWidget.h
FITGUI3_QTS         = $(FITGUI3_QTH:.h=.cpp)

FITGUI3_PUBH        = $(patsubst $(FITGUI3_DIR)/%.h, $(GO4SYS)/include/%.h, $(FITGUI3_QTH) $(FITGUI3_DIR)/TGo4FitPanel.h)

# used in the main Makefile

GO4QT3HEADS        += $(FITGUI3_FH) $(FITGUI3_PUBH)

ifdef DOPACKAGE
DISTRFILES         += $(FITGUI3_FORMS) $(FITGUI3_FORMSI) $(FITGUI3_QTPRO)
DISTRFILES         += $(FITGUI3_QTH) $(FITGUI3_QTS) 
DISTRFILES         += $(FITGUI3_DIR)/TGo4FitGuiTypes.h $(FITGUI3_DIR)/Module.mk
endif

##### local rules #####

ifdef GO4_QT3
$(GO4SYS)/include/%.h: $(FITGUI3_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@
endif


# generate makefile only if project is exists
ifneq ($(wildcard $(FITGUI3_QTPRO)),)
$(FITGUI3_QTMAKE): $(FITGUI3_QTPRO) $(FITGUI3_FORMS)
	@echo "Generating Makefile.qt of the Fit GUI..."
	cd $(FITGUI3_DIR); $(QMAKE) $(FITGUI3_QTPRO) -o $(FITGUI3_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) "DESTDIR=$(GO4DLLPATH)"
endif

qt3-FitGUI: $(FITGUI3_QTMAKE) $(GO4QT3HEADS)
	@echo "Generating Qt part of the Fit GUI..."
	+cd $(FITGUI3_DIR); $(MAKE) -f $(FITGUI3_QTMAKE)

clean-qt3-FitGUI-bin:
	@rm -f $(FITGUI3_DIR)/*.o
ifneq ($(wildcard $(FITGUI3_QTMAKE)),)
	cd $(FITGUI3_DIR); $(MAKE) -f $(FITGUI3_QTMAKE) clean
endif
	@rm -f $(FITGUI3_QTMAKE) $(FITGUI3_FH)
	@rm -rf .obj .moc
	@echo "Clean qt3 fitgui done"

clean-qt3-FitGUI: clean-qt3-FitGUI-bin
	@rm -f $(GO4DLLPATH)/$(FITGUI3_QTLIBNAME).$(DllSuf)*
	@rm -f $(FITGUI3_PUBH)
	@echo "Clean qt3 fitgui done"
