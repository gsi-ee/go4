ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
USERGUI4_DIR         = qt4/Go4UserGUI
USERGUI4_QFLAGS      = GO4INCDIR=../../include GO4BINDIR=../../bin
USERGUI4_DEPRULES    = $(GO4GUI4_EXE)
else
USERGUI4_DIR         = $(CURDIR)
USERGUI4_QFLAGS      = GO4INCDIR=$(shell go4-config --nativeincdir) GO4BINDIR=$(shell go4-config --nativebindir)
endif

USERGUI4_LIBDIR      = $(USERGUI4_DIR)
USERGUI4_QTLIBNAME   = libGo4UserGui
USERGUI4_QTPRO       = Go4UserGui.pro
USERGUI4_QTMAKE      = Makefile.qt

USERGUI4_NOTLIBF     =

## must be similar for every module

USERGUI4_FORMS      := $(wildcard $(USERGUI4_DIR)/*.ui)
USERGUI4_FH         := $(USERGUI4_FORMS:.ui=.h)
USERGUI4_FS         := $(USERGUI4_FORMS:.ui=.cpp)


USERGUI4_QTLIB       = $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).$(DllSuf)

# used in the main Makefile

ifdef DOPACKAGE
DISTRFILES         += $(USERGUI4_FORMS) $(USERGUI4_FS) $(USERGUI4_FH)
DISTRFILES         += $(USERGUI4_DIR)/$(USERGUI4_QTPRO) $(USERGUI4_DIR)/README.md
DISTRFILES         += $(USERGUI4_DIR)/Makefile
endif

##### local rules #####


ifdef GO4_X11


ifdef GO4_QT
ifneq ($(GO4_QT), 3)
ifneq ($(GO4_OS),Win32)

all:: $(USERGUI4_QTLIB)

gui:: $(USERGUI4_QTLIB)

clean-bin:: clean-qt4usergui-bin

clean:: clean-qt4usergui

endif
endif
endif

$(USERGUI4_DIR)/$(USERGUI4_QTMAKE): $(USERGUI4_DIR)/$(USERGUI4_QTPRO) $(USERGUI4_FORMS) $(USERGUI4_DEPRULES) 
	@echo "Generating $(USERGUI4_QTMAKE)..."
	cd $(USERGUI4_DIR); $(QMAKE) $(USERGUI4_QTPRO) -o $(USERGUI4_QTMAKE) $(QMAKEOPTFLAG) $(QMAKEFLAGS)

$(USERGUI4_QTLIB): $(USERGUI4_DIR)/$(USERGUI4_QTMAKE) $(USERGUI4_FH) $(USERGUI4_FS) $(USERGUI4_DEPRULES)  
	@echo "Generating Qt part of the User GUI..."
	+cd $(USERGUI4_DIR); $(MAKEFORQT) -f $(USERGUI4_QTMAKE) $(USERGUI4_QFLAGS)
ifeq ($(GO4_OS),Win32)
	$(MV) $(USERGUI4_DIR)/release/$(USERGUI4_QTLIBNAME).* $(USERGUI4_DIR)
endif


clean-qt4usergui-bin:
ifneq ($(wildcard $(USERGUI4_DIR)/$(USERGUI4_QTMAKE)),)
	cd $(USERGUI4_DIR); $(MAKEFORQT) -f $(USERGUI4_QTMAKE) clean $(USERGUI4_QFLAGS)
endif
	@$(RM) $(USERGUI4_DIR)/$(USERGUI4_QTMAKE)*
	@$(RM) $(USERGUI4_DIR)/*.o $(USERGUI4_DIR)/moc_*.* $(USERGUI4_DIR)/ui_*.h
ifeq ($(GO4_OS),Win32)
	@$(RM) $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).exp $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).lib
	@$(RMDIR) $(USERGUI4_DIR)/release $(USERGUI4_DIR)/debug 
endif

clean-qt4usergui: clean-qt4usergui-bin
	@$(RM) $(USERGUI4_LIBDIR)/$(USERGUI4_QTLIBNAME).* $(USERGUI4_LIBDIR)/.qmake.stash

clean-qt4:: clean-qt4usergui


include $(GO4SYS)/Makefile.rules

else

# case when no X11 support provided on the machine

clean-qt4usergui: 
	@$(RM) qt4/Go4UserGUI/*.so qt4/Go4UserGUI/moc_*.* qt4/Go4UserGUI/*.o qt4/Go4UserGUI/ui_QUserPanel.h qt4/Go4UserGUI/Makefile.qt

endif
