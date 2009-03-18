TEMPLATE = lib
LANGUAGE = C++
TARGET   = go4plugin
CONFIG   += qt warn_on release designer plugin thread
INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include
QT +=  qt3support 

RESOURCES += go4plugin.qrc

SOURCES  = QGo4WidgetPlugin.cpp \
           QGo4LineEditPlugin.cpp \
           QGo4BrowserListViewPlugin.cpp \
           QFitWidgetPlugin.cpp \
           QFitNamedWidgetPlugin.cpp \
           QFitModelWidgetPlugin.cpp\
           QGo4CommandsHistoryPlugin.cpp \
           QGo4RootCanvasPlugin.cpp \
           QGo4TablePlugin.cpp \
           TQrootCanvasPlugin.cpp \
           TQrootWindowPlugin.cpp \
           QGo4Widget.cpp \
           QGo4LineEdit.cpp \
           QGo4BrowserListView.cpp \
           QFitWidget.cpp \
           QFitNamedWidget.cpp \
           QFitModelWidget.cpp\
           QGo4CommandsHistory.cpp \
           QGo4RootCanvas.cpp \
           QGo4Table.cpp \
           tqrootcanvas.cpp \
           tqcanvasmenu.cpp \
           tqrootdialog.cpp\
           tqrootwindow.cpp\
           go4plugin.cpp


HEADERS  = QGo4WidgetPlugin.h \
           QGo4LineEditPlugin.h \
           QGo4BrowserListViewPlugin.h \
           QFitWidgetPlugin.h \
           QFitNamedWidgetPlugin.h \
           QFitModelWidgetPlugin.h \
           QGo4CommandsHistoryPlugin.h \
           QGo4RootCanvasPlugin.h \
           QGo4TablePlugin.h \
           TQrootCanvasPlugin.h \
           TQrootWindowPlugin.h \
           QGo4Widget.h \
           QGo4LineEdit.h \
           QGo4BrowserListView.h \
           QFitWidget.h \
           QFitNamedWidget.h \
           QFitModelWidget.h \
           QGo4CommandsHistory.h \
           QGo4RootCanvas.h \
           QGo4Table.h \
           tqrootcanvas.h \
           tqcanvasmenu.h \
           tqrootdialog.h \
           tqrootwindow.h\
           lockguard.h\
           go4plugin.h
