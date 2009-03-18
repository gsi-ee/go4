USERGUI_NAME        = Go4UserGUI
MODULE_NAME         = $(USERGUI_NAME)

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
USERGUI_DIR         = $(GO4SYS)/qt4/$(USERGUI_NAME)
else
USERGUI_DIR         = $(CURDIR)
endif

USERGUI_LIBDIR      = $(USERGUI_DIR)
USERGUI_QTLIBNAME   = libGo4UserGui
USERGUI_QTPRO       = $(USERGUI_DIR)/Go4UserGui.pro
USERGUI_QTMAKE      = $(USERGUI_DIR)/Makefile.qt
USERGUI_QGO4WIDG    = $(GO4SYS)/include/QGo4Widget.h

USERGUI_NOTLIBF     =

## must be similar for every module

USERGUI_FORMS      := $(wildcard $(USERGUI_DIR)/*.ui)
#USERGUI_FORMSI     := $(wildcard $(USERGUI_DIR)/*.ui.h)
USERGUI_FH         := $(USERGUI_FORMS:.ui=.h)
USERGUI_FS         := $(USERGUI_FORMS:.ui=.cpp)
#USERGUI_UI_H        = $(patsubst $(USERGUI_FORMS)/%.ui,$(USERGUI_FORMS)/ui_%.h,$(USERGUI_FORMS))


USERGUI_QTLIB       = $(USERGUI_LIBDIR)/$(USERGUI_QTLIBNAME).$(DllSuf).$(VERSSUF)


# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(USERGUI_FORMS) $(USERGUI_FS) $(USERGUI_FH)
DISTRFILES         += $(USERGUI_QTPRO) $(USERGUI_DIR)/README.txt
endif

##### local rules #####

$(USERGUI_QTMAKE): $(USERGUI_QTPRO) $(USERGUI_FORMS) 
	@echo "Generating $(USERGUI_QTMAKE)..."
	cd $(USERGUI_DIR); $(QMAKE) $(USERGUI_QTPRO) -o $(USERGUI_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(USERGUI_QTLIB): $(USERGUI_QTMAKE) $(USERGUI_QGO4WIDG)
	@echo "Generating Qt part of the User GUI..."
	+cd $(USERGUI_DIR); $(MAKE) -f $(USERGUI_QTMAKE)

all-$(USERGUI_NAME):  $(USERGUI_QTLIB)

clean-obj-$(USERGUI_NAME):
	@rm -f $(USERGUI_LIBDIR)/$(USERGUI_QTLIBNAME).$(DllSuf)*
	@rm -f $(USERGUI_DIR)/*.o

clean-$(USERGUI_NAME): clean-obj-$(USERGUI_NAME)
ifneq ($(wildcard $(USERGUI_QTMAKE)),)
	cd $(USERGUI_DIR); $(MAKE) -f $(USERGUI_QTMAKE) clean
endif
	@rm -f $(USERGUI_QTMAKE)

map-$(USERGUI_NAME):
	@echo "No ROOT map can be generated for User Gui"
