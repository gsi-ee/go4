TEMPLATE	= lib
LANGUAGE	= C++

CONFIG += qt warn_off thread plugin

win32: DESTDIR  = .

win32: TARGET   = libGo4UserGui

win32: LIBS += ..\..\bin\go4.lib libCore.lib


INCLUDEPATH += $(GO4SYS)/include

DEPENDPATH += $(GO4SYS)/include

SOURCES += QUserPanel.cpp

HEADERS += QUserPanel.h 

FORMS = QUserPanel.ui
