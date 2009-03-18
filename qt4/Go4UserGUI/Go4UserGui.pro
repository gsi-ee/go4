TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_off thread

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS)

DEPENDPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS)

SOURCES	+= QUserPanel.cpp

HEADERS	+= QUserPanel.h 

FORMS	= QUserPanel.ui

#The following line was inserted by qt3to4
QT +=  qt3support 

