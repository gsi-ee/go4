#this is just top module makefile for qt4

GO4QT4HEADS :=

GO4QT4DEP   :=


include qt4/Go4GUI/Module.mk

include qt4/Go4UserGUI/Makefile


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



ifeq ($(GO4_QT), 4)

GO4QTHEADS += $(GO4QT4HEADS)

LIBDEPENDENC += $(GO4QT4DEP)

gui:: libs qt4-GUI all-qt4usergui

plugin:: all-qt4plugin

endif

FASTRULES += clean-qt4

clean-qt4:: clean-qt4-GUI clean-qt4usergui
	@rm -f build/Makefile.gener

clean:: clean-qt4

clean-bin:: clean-qt4-GUI-bin

