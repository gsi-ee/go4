QT += network

QT += widgets

TEMPLATE	= app
LANGUAGE	= C++
MOC_DIR     =.moc
OBJECTS_DIR =.obj

CONFIG	+= qt warn_off thread

INCLUDEPATH += . $$(GO4SYS)/qt4/Go4QtRoot
DEPENDPATH  += . $$(GO4SYS)/qt4/Go4QtRoot

TARGET      = qtrootexample
DESTDIR     = .
PROJECTNAME = QtRootExample

win32:QMAKE_LFLAGS  += /nodefaultlib:msvcrt /verbose:lib msvcrt.lib

win32:QMAKE_CXXFLAGS  += -MD

# this is necessary to solve error with non-initialized gSystem
unix:QMAKE_CXXFLAGS += -fPIC

FORMS += ExampleWidget.ui

HEADERS += $$(GO4SYS)/qt4/Go4QtRoot/QRootApplication.h \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootCanvas.h \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootDialog.h \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootWindow.h \
           ExampleWidget.h

SOURCES += $$(GO4SYS)/qt4/Go4QtRoot/QRootApplication.cpp \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootCanvas.cpp \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootDialog.cpp \
           $$(GO4SYS)/qt4/Go4QtRoot/QRootWindow.cpp \
           ExampleWidget.cpp \
           ExampleMain.cpp
