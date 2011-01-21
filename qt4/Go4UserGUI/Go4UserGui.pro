TEMPLATE	= lib
LANGUAGE	= C++

CONFIG += qt warn_off thread plugin

win32: TARGET = libGo4UserGui

win32: LIBS += $(GO4BINDIR)/go4.lib libCore.lib

win32:QMAKE_LFLAGS  += /nodefaultlib:msvcrt msvcrt.lib

win32:QMAKE_CXXFLAGS += -MD

macx: QMAKE_LFLAGS +=  -undefined dynamic_lookup

INCLUDEPATH += $(GO4INCDIR)

DEPENDPATH += $(GO4INCDIR)

SOURCES += QUserPanel.cpp

HEADERS += QUserPanel.h 

FORMS = QUserPanel.ui
