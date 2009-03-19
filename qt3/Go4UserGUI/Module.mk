USERGUI3_NAME       = qt3usergui
MODULE_NAME         = $(USERGUI3_NAME)

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
USERGUI3_DIR         = $(GO4SYS)/qt3/Go4UserGUI
else
USERGUI3_DIR         = $(CURDIR)
endif

USERGUI3_LIBDIR      = $(USERGUI3_DIR)
USERGUI3_QTLIBNAME   = libGo4UserGui
USERGUI3_QTPRO       = $(USERGUI3_DIR)/Go4UserGui.pro
USERGUI3_QTMAKE      = $(USERGUI3_DIR)/Makefile.qt
USERGUI3_QGO4WIDG    = $(GO4SYS)/include/QGo4Widget.h

USERGUI3_NOTLIBF     =

## must be similar for every module

USERGUI3_FORMS      := $(wildcard $(USERGUI3_DIR)/*.ui)
USERGUI3_FORMSI     := $(wildcard $(USERGUI3_DIR)/*.ui.h)
USERGUI3_FH         := $(USERGUI3_FORMS:.ui=.h)
USERGUI3_FS         := $(USERGUI3_FORMS:.ui=.cpp)

USERGUI3_QTLIB       = $(USERGUI3_LIBDIR)/$(USERGUI3_QTLIBNAME).$(DllSuf).$(VERSSUF)


# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(USERGUI3_FORMS) $(USERGUI3_FORMSI)
DISTRFILES         += $(USERGUI3_QTPRO) $(USERGUI3_DIR)/README.txt
DISTRFILES         += $(USERGUI3_DIR)/Makefile $(USERGUI3_DIR)/Module.mk
endif

##### local rules #####

$(USERGUI3_QTMAKE): $(USERGUI3_QTPRO) $(USERGUI3_FORMS) 
	@echo "Generating $(USERGUI3_QTMAKE)..."
	cd $(USERGUI3_DIR); $(QMAKE) $(USERGUI3_QTPRO) -o $(USERGUI3_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(USERGUI3_QTLIB): $(USERGUI3_QTMAKE) $(USERGUI3_FORMSI) $(USERGUI3_QGO4WIDG)
	@echo "Generating Qt part of the User GUI..."
	+cd $(USERGUI3_DIR); $(MAKE) -f $(USERGUI3_QTMAKE)

all-$(USERGUI3_NAME):  $(USERGUI3_QTLIB)

clean-obj-$(USERGUI3_NAME):
	@rm -f $(USERGUI3_LIBDIR)/$(USERGUI3_QTLIBNAME).$(DllSuf)*
	@rm -f $(USERGUI3_DIR)/*.o

clean-$(USERGUI3_NAME): clean-obj-$(USERGUI3_NAME)
ifneq ($(wildcard $(USERGUI3_QTMAKE)),)
	cd $(USERGUI3_DIR); $(MAKE) -f $(USERGUI3_QTMAKE) clean
endif
	@rm -f $(USERGUI3_QTMAKE)

map-$(USERGUI3_NAME):
	@echo "No ROOT map can be generated for User Gui"
