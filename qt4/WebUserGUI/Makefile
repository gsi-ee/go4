ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
WEBUSERGUI_DIR         = qt4/WebUserGUI
WEBUSERGUI_QFLAGS      = GO4INCDIR=../../include GO4BINDIR=../../bin
WEBUSERGUI_DEPRULES    = | qt4-GUI qt4-heads
else
WEBUSERGUI_DIR         = $(CURDIR)
WEBUSERGUI_QFLAGS      = GO4INCDIR=$(shell go4-config --nativeincdir) GO4BINDIR=$(shell go4-config --nativebindir)
endif

WEBUSERGUI_LIBDIR      = $(WEBUSERGUI_DIR)
WEBUSERGUI_QTLIBNAME   = libGo4UserGui
WEBUSERGUI_QTPRO       = Go4UserGui.pro
WEBUSERGUI_QTMAKE      = Makefile.qt

WEBUSERGUI_NOTLIBF     =

## must be similar for every module

WEBUSERGUI_FORMS      := $(wildcard $(WEBUSERGUI_DIR)/*.ui)
WEBUSERGUI_FH         := $(WEBUSERGUI_FORMS:.ui=.h)
WEBUSERGUI_FS         := $(WEBUSERGUI_FORMS:.ui=.cpp)


WEBUSERGUI_QTLIB       = $(WEBUSERGUI_LIBDIR)/$(WEBUSERGUI_QTLIBNAME).$(DllSuf)


# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(WEBUSERGUI_FORMS) $(WEBUSERGUI_FS) $(WEBUSERGUI_FH)
DISTRFILES         += $(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTPRO) $(WEBUSERGUI_DIR)/README.txt
DISTRFILES         += $(WEBUSERGUI_DIR)/Makefile
endif

##### local rules #####

ifneq ($(GO4_QT), 3)

all:: all-qt5webusergui

clean-bin:: clean-qt5webusergui-bin

clean:: clean-qt5webusergui

endif


$(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTMAKE): $(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTPRO) $(WEBUSERGUI_FORMS) $(WEBUSERGUI_DEPRULES) 
	@echo "Generating $(WEBUSERGUI_QTMAKE)..."
	cd $(WEBUSERGUI_DIR); $(QMAKE) $(WEBUSERGUI_QTPRO) -o $(WEBUSERGUI_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(WEBUSERGUI_QTLIB): $(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTMAKE) $(WEBUSERGUI_FH) $(WEBUSERGUI_FS) $(WEBUSERGUI_DEPRULES)  
	@echo "Generating Qt part of the User GUI..."
	+cd $(WEBUSERGUI_DIR); $(MAKEFORQT) -f $(WEBUSERGUI_QTMAKE) $(WEBUSERGUI_QFLAGS)
ifeq ($(GO4_OS),Win32)
	$(MV) $(WEBUSERGUI_DIR)/release/$(WEBUSERGUI_QTLIBNAME).* $(WEBUSERGUI_DIR)
endif


ifneq ($(GO4_OS),Darwin)

all-qt5webusergui:  $(WEBUSERGUI_QTLIB)

else

all-qt5webusergui:
	@echo "No user gui (yet) under Darwin"

endif


clean-qt5webusergui-bin:
ifneq ($(wildcard $(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTMAKE)),)
	cd $(WEBUSERGUI_DIR); $(MAKEFORQT) -f $(WEBUSERGUI_QTMAKE) clean $(WEBUSERGUI_QFLAGS)
endif
	@$(RM) $(WEBUSERGUI_DIR)/$(WEBUSERGUI_QTMAKE)*
	@$(RM) $(WEBUSERGUI_DIR)/*.o $(WEBUSERGUI_DIR)/moc_*.* $(WEBUSERGUI_DIR)/ui_*.h *.json
ifeq ($(GO4_OS),Win32)
	@$(RM) $(WEBUSERGUI_LIBDIR)/$(WEBUSERGUI_QTLIBNAME).exp $(WEBUSERGUI_LIBDIR)/$(WEBUSERGUI_QTLIBNAME).lib
	@$(RMDIR) $(WEBUSERGUI_DIR)/release $(WEBUSERGUI_DIR)/debug 
endif

clean-qt5webusergui: clean-qt5webusergui-bin
	@$(RM) $(WEBUSERGUI_LIBDIR)/$(WEBUSERGUI_QTLIBNAME).* $(WEBUSERGUI_LIBDIR)/.qmake.stash

clean-qt4:: clean-qt5webusergui

include $(GO4SYS)/Makefile.rules
