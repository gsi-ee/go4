## normally should be like this for every module, but can be specific

FITGUI4_DIR         = $(GO4SYS)/qt4/Go4FitGUI
FITGUI4_QTLIBNAME   = libGo4FitGUI4
FITGUI4_QTPRO       = $(FITGUI4_DIR)/Go4FitGUI4.pro
FITGUI4_QTMAKE      = $(FITGUI4_DIR)/Makefile.qt

FITGUI4_NOTLIBF     =

## must be similar for every module

FITGUI4_FORMS       = $(wildcard $(FITGUI4_DIR)/*.ui)

FITGUI4_UI_H        = $(patsubst $(FITGUI4_FORMS)/%.ui,$(FITGUI4_FORMS)/ui_%.h,$(FITGUI4_FORMS))

FITGUI4_FH          = $(FITGUI4_FORMS:.ui=.h)
FITGUI4_FS          = $(FITGUI4_FORMS:.ui=.cpp)

FITGUI4_QTS         = $(filter-out $(FITGUI4_NOTLIBF), $(wildcard $(FITGUI4_DIR)/*.cpp))
FITGUI4_QTH         = $(FITGUI4_QTS:.cpp=.h)

FITGUI4_PUBH        = $(patsubst $(FITGUI4_DIR)/%.h, $(GO4SYS)/include/%.h, $(FITGUI4_QTH))

# used in the main Makefile

GO4QT4HEADS         += $(FITGUI4_UI_H) $(FITGUI4_PUBH)


ifdef DOPACKAGE
DISTRFILES         += $(FITGUI4_FORMS)  $(FITGUI4_QTPRO)
DISTRFILES         += $(FITGUI4_QTH) $(FITGUI4_QTS) $(FITGUI4_DIR)/TGo4FitGuiTypes.h
DISTRFILES         += $(FITGUI4_DIR)/Module.mk
endif


##### local rules #####

ifdef GO4_QT4
$(GO4SYS)/include/%.h: $(FITGUI4_DIR)/%.h
	@cp -f $< $@
endif

$(FITGUI4_DIR)/ui_%.h: $(FITGUI4_DIR)/%.ui
	@echo "Producing file $@ ..." 
	@$(UIC) $< -o $@

# generate makefile only if project is exists
ifneq ($(wildcard $(FITGUI4_QTPRO)),)
$(FITGUI4_QTMAKE): $(FITGUI4_QTPRO) $(FITGUI4_FORMS)
	@echo "Generating Makefile.qt of the Fit GUI..."
	cd $(FITGUI4_DIR); $(QMAKE) $(FITGUI4_QTPRO) -o $(FITGUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS) "DESTDIR=$(GO4DLLPATH)"
endif

qt4-FitGUI: $(FITGUI4_QTMAKE) $(FITGUI4_UI_H)
	@echo "Generating Qt4 part of the Fit GUI..."
	+cd $(FITGUI4_DIR); $(MAKE) -f $(FITGUI4_QTMAKE)

clean-qt4-FitGUI:
	@rm -f $(GO4DLLPATH)/$(FITGUI4_QTLIBNAME).$(DllSuf)*
	@rm -f $(FITGUI4_DIR)/*.o
ifneq ($(wildcard $(FITGUI4_QTMAKE)),)
	cd $(FITGUI4_DIR); $(MAKE) -f $(FITGUI4_QTMAKE) clean
endif
	@rm -f $(FITGUI4_QTMAKE) $(FITGUI4_PUBH)
