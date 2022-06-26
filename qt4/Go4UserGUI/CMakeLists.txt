cmake_minimum_required(VERSION 3.9)

project(Go4UserGui)

find_package(Go4)

include(${GO4_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(GO4_QTVERSION STREQUAL Qt4)
   find_package(Qt4 4.7 REQUIRED QtGui)
   if(NOT Qt4_FOUND)
      message(FATAL_ERROR "Could NOT find Qt4")
   endif()
   set(qt_libs Qt4::QtCore Qt4::QtGui)
elseif(GO4_QTVERSION STREQUAL Qt6)
   find_package(Qt6 COMPONENTS Widgets)
   if(NOT Qt6_FOUND)
      message(FATAL_ERROR "Could NOT find Qt6")
   endif()
   set(qt_libs Qt6::Core Qt6::Widgets)
else()
   find_package(Qt5 COMPONENTS Widgets)
   if(NOT Qt5_FOUND)
      message(FATAL_ERROR "Could NOT find Qt5")
   endif()
   set(qt_libs Qt5::Core Qt5::Widgets)
endif()


GO4_LINK_LIBRARY(Go4UserGui
                 SOURCES QUserPanel.cpp QUserPanel.ui
                 LIBRARIES ${qt_libs} ${Go4Widgets_LIBRARY} ${Go4QtRoot_LIBRARY} ROOT::Core)