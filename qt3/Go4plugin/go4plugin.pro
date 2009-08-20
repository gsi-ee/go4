TEMPLATE = lib
LANGUAGE = C++
TARGET   = go4plugin
CONFIG   += qt warn_on release plugin thread

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include $(GO4SYS)/qt3/Go4FitGUI

SOURCES  = TGo4QtDesignerPlugin.cpp \
           QGo4Widget.cpp \
           QGo4LineEdit.cpp \
           QGo4Table.cpp \
           QGo4BrowserListView.cpp \
           QFitWidget.cpp \
           QFitNamedWidget.cpp \
           QFitModelWidget.cpp\
           QGo4CommandsHistory.cpp

HEADERS  = TGo4QtDesignerPlugin.h \
           QGo4Widget.h \
           QGo4LineEdit.h \
           QGo4Table.h \
           QGo4BrowserListView.h \
           QFitWidget.h \
           QFitNamedWidget.h \
           QFitModelWidget.h \
           QGo4CommandsHistory.h
