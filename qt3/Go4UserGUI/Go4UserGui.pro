TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_off thread

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS)

DEPENDPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS)

FORMS	= QUserPanel.ui
