QT += network

QT += widgets

TEMPLATE	= app
LANGUAGE	= C++
MOC_DIR     =.moc
OBJECTS_DIR =.obj

CONFIG	+= qt warn_off thread

INCLUDEPATH += . ../Go4QtRoot
DEPENDPATH  += . ../Go4QtRoot

TARGET      = qtrootexample
DESTDIR     = .
PROJECTNAME = QtRootExample

win32:QMAKE_LFLAGS  += /nodefaultlib:msvcrt /verbose:lib msvcrt.lib

win32:QMAKE_CXXFLAGS  += -MD

# this is necessary to solve error with non-initialized gSystem
unix:QMAKE_CXXFLAGS += -fPIC

FORMS += ExampleWidget.ui

HEADERS += ../Go4QtRoot/QRootApplication.h \
           ../Go4QtRoot/QRootCanvas.h \
           ../Go4QtRoot/QRootDialog.h \
           ../Go4QtRoot/QRootWindow.h \
           ExampleWidget.h

SOURCES += ../Go4QtRoot/QRootApplication.cpp \
           ../Go4QtRoot/QRootCanvas.cpp \
           ../Go4QtRoot/QRootDialog.cpp \
           ../Go4QtRoot/QRootWindow.cpp \
           ExampleWidget.cpp
