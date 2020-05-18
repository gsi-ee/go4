GO4_INSTALL_HEADERS(
   QGo4Widget.h
   QGo4LineEdit.h
)

if(GO4_QTVERSION STREQUAL Qt4)
   set(qt_libs Qt4::QtCore Qt4::QtGui)
else()
   set(qt_libs Qt5::Core Qt5::Widgets)
endif()

GO4_LINK_LIBRARY(Go4Widgets
                 SOURCES QGo4Widget.h QGo4Widget.cpp QGo4LineEdit.h QGo4LineEdit.cpp
                 LIBRARIES ${qt_libs}
                 DEFINITIONS GO4_BUILD_WIDGETS_LIB)
