GO4_INSTALL_HEADERS(
   QGo4Widget.h
   QGo4LineEdit.h
)

if(GO4_QTVERSION STREQUAL Qt6)
   set(qt_libs Qt6::Core Qt6::Widgets)
else()
   set(qt_libs Qt5::Core Qt5::Widgets)
endif()

GO4_LINK_LIBRARY(Go4Widgets
                 SOURCES QGo4Widget.h QGo4Widget.cpp QGo4LineEdit.h QGo4LineEdit.cpp
                 LIBRARIES ${qt_libs}
                 DEFINITIONS GO4_BUILD_WIDGETS_LIB)

install(TARGETS Go4Widgets EXPORT ${CMAKE_PROJECT_NAME}Exports
                           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries
                           LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
                           ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
