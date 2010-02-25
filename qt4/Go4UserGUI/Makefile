ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
USERGUI4_DIR         = qt4/Go4UserGUI
USERGUI4_QFLAGS      = "GO4SYS=../.."
else
USERGUI4_DIR         = $(CURDIR)
endif

USERGUI4_LIBDIR      = $(USERGUI4_DIR)
USERGUI4_QTLIBNAME   = libGo4UserGui
USERGUI4_QTPRO       = Go4UserGui.pro
USERGUI4_QTMAKE      = Makefile.qt
USERGUI4_QGO4WIDG    = $(GO4SYS)/include/QGo4Widget.h

USERGUI4_NOTLIBF     =

## must be similar for every module

USERGUI4_FORMS      := $(wildcard $(USERGUI4_DIR)/*.ui)
USERGUI4_FH         := $(USERGUI4_FORMS:.ui=.h)
USERGUI4_FS         := $(USERGUI4_FORMS:.ui=.cpp)


USERGUI4_QTLIB       = $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).$(DllSuf)


# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(USERGUI4_FORMS) $(USERGUI4_FS) $(USERGUI4_FH)
DISTRFILES         += $(USERGUI4_DIR)/$(USERGUI4_QTPRO) $(USERGUI4_DIR)/README.txt
DISTRFILES         += $(USERGUI4_DIR)/Makefile
endif

##### local rules #####

ifeq ($(GO4_QT), 4)

all:: all-qt4usergui

clean-bin:: clean-qt4usergui-bin

clean:: clean-qt4usergui

endif


$(USERGUI4_DIR)/$(USERGUI4_QTMAKE): $(USERGUI4_DIR)/$(USERGUI4_QTPRO) $(USERGUI4_FORMS) 
	@echo "Generating $(USERGUI4_QTMAKE)..."
	cd $(USERGUI4_DIR); $(QMAKE) $(USERGUI4_QTPRO) -o $(USERGUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(USERGUI4_QTLIB): $(USERGUI4_DIR)/$(USERGUI4_QTMAKE) $(USERGUI4_QGO4WIDG)
	@echo "Generating Qt part of the User GUI..."
	+cd $(USERGUI4_DIR); $(MAKEFORQT) -f $(USERGUI4_QTMAKE) $(USERGUI4_QFLAGS)

all-qt4usergui:  $(USERGUI4_QTLIB)

clean-qt4usergui-bin:
ifneq ($(wildcard $(USERGUI4_DIR)/$(USERGUI4_QTMAKE)),)
	cd $(USERGUI4_DIR); $(MAKEFORQT) -f $(USERGUI4_QTMAKE) clean $(USERGUI4_QFLAGS)
endif
	@rm -f $(USERGUI4_DIR)/$(USERGUI4_QTMAKE)
	@rm -f $(USERGUI4_DIR)/*.o $(USERGUI4_DIR)/moc_*.* $(USERGUI4_DIR)/ui_*.h

clean-qt4usergui: clean-qt4usergui-bin
	@rm -f $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).$(DllSuf)*

clean-qt4:: clean-qt4usergui

include $(GO4SYS)/Makefile.rules
