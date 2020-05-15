cmake_minimum_required(VERSION 3.9)

project(Go4UserGui)

find_package(Go4)

include(${GO4_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(GO4_QTVERSION STREQUAL Qt4)
   set(CMAKE_INCLUDE_CURRENT_DIR ON)
   find_package(Qt4 4.7 REQUIRED QtGui)
   if(NOT Qt4_FOUND)
      message(FATAL_ERROR "Could NOT find Qt4")
   endif()
   set(qt_libs Qt4::QtCore Qt4::QtGui)
else()
   find_package(Qt5 COMPONENTS Widgets)
   if(NOT Qt5_FOUND)
      message(FATAL_ERROR "Could NOT find Qt5")
   endif()
   set(qt_libs Qt5::Widgets)
endif()


add_library(Go4UserGui SHARED
   QUserPanel.cpp
   QUserPanel.ui
)

target_link_libraries(Go4UserGui ${qt_libs} ${Go4Widgets_LIBRARY} ${Go4QtRoot_LIBRARY})
