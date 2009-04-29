TEMPLATE = lib
LANGUAGE = C++
TARGET   = go4plugin
CONFIG   += qt warn_on release designer plugin thread
INCLUDEPATH += $(ROOTSYS)/include $(GO4SYS)/include

RESOURCES += go4plugin.qrc

SOURCES  = QGo4WidgetPlugin.cpp \
           QGo4LineEditPlugin.cpp \
           QGo4BrowserTreeWidgetPlugin.cpp \
           QFitWidgetPlugin.cpp \
           QFitNamedWidgetPlugin.cpp \
           QFitModelWidgetPlugin.cpp\
           QGo4CommandsHistoryPlugin.cpp \
           QRootCanvasPlugin.cpp \
           QRootWindowPlugin.cpp \
           QGo4Widget.cpp \
           QGo4LineEdit.cpp \
           QGo4BrowserTreeWidget.cpp \
           QFitWidget.cpp \
           QFitNamedWidget.cpp \
           QFitModelWidget.cpp\
           QGo4CommandsHistory.cpp \
           QRootCanvas.cpp \
           QRootDialog.cpp\
           QRootWindow.cpp\
           go4plugin.cpp

HEADERS  = QGo4WidgetPlugin.h \
           QGo4LineEditPlugin.h \
           QGo4BrowserTreeWidgetPlugin.h \
           QFitWidgetPlugin.h \
           QFitNamedWidgetPlugin.h \
           QFitModelWidgetPlugin.h \
           QGo4CommandsHistoryPlugin.h \
           QRootCanvasPlugin.h \
           QRootWindowPlugin.h \
           QGo4Widget.h \
           QGo4LineEdit.h \
           QGo4BrowserTreeWidget.h \
           QFitWidget.h \
           QFitNamedWidget.h \
           QFitModelWidget.h \
           QGo4CommandsHistory.h \
           QRootCanvas.h \
           QRootDialog.h \
           QRootWindow.h\
           TGo4LockGuard.h\
           go4plugin.h
