#this is just top module makefile for qt4

GO4QT4HEADS :=

GO4QT4DEP   :=


include qt4/Go4GUI/Module.mk

include qt4/Go4UserGUI/Makefile


ifndef GO4_WIN32

include qt4/Go4plugin/Module.mk

endif

DISTRFILES         += qt4/Module.mk qt4/go4.conf

ifdef GO4_QT4

GO4QTHEADS += $(GO4QT4HEADS)

LIBDEPENDENC += $(GO4QT4DEP)

gui:: libs qt4-GUI all-qt4usergui

plugin:: all-qt4plugin

endif

FASTRULES += clean-qt4

clean-qt4:: clean-qt4-GUI

clean:: clean-qt4

