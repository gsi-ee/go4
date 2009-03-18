#this is just top module makefile for qt4

GO4QT3HEADS :=
GO4QT3DEP   :=

include qt3/Go4QtRoot/Module.mk

include qt3/Go4FitGUI/Module.mk

include qt3/Go4GUI/Module.mk

include qt3/Go4UserGUI/Module.mk

include qt3/Go4plugin/Module.mk

ifdef GO4_QT3

GO4QTHEADS += $(GO4QT3HEADS)

LIBDEPENDENC += $(GO4QT3DEP)

gui:: libs qt3-GUI all-qt3usergui

clean:: clean-qt3-interface clean-qt3-FitGUI clean-qt3-GUI clean-qt3usergui clean-qt3plugin

plugin:: all-qt3plugin

endif
