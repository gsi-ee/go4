TEMPLATE	= lib
LANGUAGE	= C++

CONFIG += qt warn_off thread plugin

INCLUDEPATH += $(GO4SYS)/include

DEPENDPATH += $(GO4SYS)/include

SOURCES += QUserPanel.cpp

HEADERS += QUserPanel.h 

FORMS = QUserPanel.ui
