USERGUI4_NAME       = qt4usergui
MODULE_NAME         = $(USERGUI4_NAME)

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
USERGUI4_DIR         = $(GO4SYS)/qt4/Go4UserGUI
else
USERGUI4_DIR         = $(CURDIR)
endif

USERGUI4_LIBDIR      = $(USERGUI4_DIR)
USERGUI4_QTLIBNAME   = libGo4UserGui
USERGUI4_QTPRO       = $(USERGUI4_DIR)/Go4UserGui.pro
USERGUI4_QTMAKE      = $(USERGUI4_DIR)/Makefile.qt
USERGUI4_QGO4WIDG    = $(GO4SYS)/include/QGo4Widget.h

USERGUI4_NOTLIBF     =

## must be similar for every module

USERGUI4_FORMS      := $(wildcard $(USERGUI4_DIR)/*.ui)
#USERGUI4_FORMSI     := $(wildcard $(USERGUI4_DIR)/*.ui.h)
USERGUI4_FH         := $(USERGUI4_FORMS:.ui=.h)
USERGUI4_FS         := $(USERGUI4_FORMS:.ui=.cpp)
#USERGUI4_UI_H        = $(patsubst $(USERGUI4_FORMS)/%.ui,$(USERGUI4_FORMS)/ui_%.h,$(USERGUI4_FORMS))


USERGUI4_QTLIB       = $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).$(DllSuf).$(VERSSUF)


# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(USERGUI4_FORMS) $(USERGUI4_FS) $(USERGUI4_FH)
DISTRFILES         += $(USERGUI4_QTPRO) $(USERGUI4_DIR)/README.txt
DISTRFILES         += $(USERGUI4_DIR)/Makefile $(USERGUI4_DIR)/Module.mk
endif

##### local rules #####

$(USERGUI4_QTMAKE): $(USERGUI4_QTPRO) $(USERGUI4_FORMS) 
	@echo "Generating $(USERGUI4_QTMAKE)..."
	cd $(USERGUI4_DIR); $(QMAKE) $(USERGUI4_QTPRO) -o $(USERGUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(USERGUI4_QTLIB): $(USERGUI4_QTMAKE) $(USERGUI4_QGO4WIDG)
	@echo "Generating Qt part of the User GUI..."
	+cd $(USERGUI4_DIR); $(MAKE) -f $(USERGUI4_QTMAKE)

all-$(USERGUI4_NAME):  $(USERGUI4_QTLIB)

clean-obj-$(USERGUI4_NAME):
	@rm -f $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).$(DllSuf)*
	@rm -f $(USERGUI4_DIR)/*.o

clean-$(USERGUI4_NAME): clean-obj-$(USERGUI4_NAME)
ifneq ($(wildcard $(USERGUI4_QTMAKE)),)
	cd $(USERGUI4_DIR); $(MAKE) -f $(USERGUI4_QTMAKE) clean
endif
	@rm -f $(USERGUI4_QTMAKE)

map-$(USERGUI4_NAME):
	@echo "No ROOT map can be generated for User Gui"
