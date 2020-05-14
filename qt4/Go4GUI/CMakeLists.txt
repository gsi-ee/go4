GO4_INSTALL_HEADERS(
   QGo4Widget.h
   QGo4LineEdit.h
)

add_library(Go4Widgets SHARED
   QGo4Widget.cpp
   QGo4LineEdit.cpp
)

if(GO4_QTVERSION STREQUAL Qt4)
   set(qt_libs Qt4::QtCore Qt4::QtGui)
else()
   set(qt_libs Qt5::Widgets Qt5::Network)
endif()


target_link_libraries(Go4Widgets ${qt_libs})
