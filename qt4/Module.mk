#this is just top module makefile for qt4

GO4QT4HEADS :=

GO4QT4DEP   :=

include qt4/Go4QtRoot/Module.mk

include qt4/Go4FitGUI/Module.mk

include qt4/Go4GUI/Module.mk

include qt4/Go4UserGUI/Module.mk

include qt4/Go4plugin/Module.mk


ifdef GO4_QT4

GO4QTHEADS += $(GO4QT4HEADS)

LIBDEPENDENC += $(GO4QT4DEP)

gui:: libs qt4-GUI all-qt4usergui

clean:: clean-qt4-interface clean-qt4-FitGUI clean-qt4-GUI clean-qt4usergui clean-qt4plugin

plugin:: all-qt4plugin

endif
