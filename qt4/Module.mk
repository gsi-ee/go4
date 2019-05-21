#this is just top module makefile for qt4

GO4QT4HEADS :=

GO4QT4DEP   :=


include qt4/Go4GUI/Module.mk

ifdef GO4_X11

include qt4/Go4UserGUI/Makefile

else

clean-qt4usergui: 
	rm -rf qt4/Go4UserGUI/*.so qt4/Go4UserGUI/moc_*.* qt4/Go4UserGUI/*.o qt4/Go4UserGUI/ui_QUserPanel.h qt4/Go4UserGUI/Makefile.qt

all-qt4usergui:

endif

ifdef GO4_WEB

include qt4/WebUserGUI/Makefile

else

clean-qt5webusergui:
	rm -rf qt4/WebUserGUI/*.so qt4/WebUserGUI/moc_*.* qt4/WebUserGUI/*.o qt4/WebUserGUI/ui_QUserPanel.h qt4/WebUserGUI/Makefile.qt

all-qt5webusergui:

endif

ifndef GO4_WIN32

include qt4/Go4plugin/Module.mk

endif

DISTRFILES         += qt4/Module.mk

DISTRFILES         += qt4/Go4QtRoot/CHANGELOG.txt
DISTRFILES         += $(wildcard qt4/Go4QtRoot/*.h)
DISTRFILES         += $(wildcard qt4/Go4QtRoot/*.cpp)
DISTRFILES         += $(wildcard qt4/Go4FitGUI/*.h)
DISTRFILES         += $(wildcard qt4/Go4FitGUI/*.ui)
DISTRFILES         += $(wildcard qt4/Go4FitGUI/*.cpp)

ifdef GO4_QT

ifneq ($(GO4_QT), 3)

GO4QTHEADS += $(GO4QT4HEADS)

LIBDEPENDENC += $(GO4QT4DEP)

.PHONY: qt4-GUI qt4-heads all-qt4usergui all-qt5webusergui clean-qt4-GUI clean-qt4usergui clean-qt5webusergui all-qt4plugin

gui:: libs qt4-GUI qt4-heads all-qt4usergui all-qt5webusergui

plugin:: all-qt4plugin

endif
endif

clean-qt4:: clean-qt4-GUI clean-qt4usergui clean-qt5webusergui
	@rm -f build/Makefile.gener

clean:: clean-qt4

clean-bin:: clean-qt4-GUI-bin

