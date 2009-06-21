TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_off thread

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS) ../../include

DEPENDPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS) ../../include

SOURCES	+= QUserPanel.cpp

HEADERS	+= QUserPanel.h 

FORMS	= QUserPanel.ui
