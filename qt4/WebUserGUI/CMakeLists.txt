cmake_minimum_required(VERSION 3.9)

project(WebUserGui)

find_package(Go4)

include(${GO4_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(GO4_QTVERSION STREQUAL Qt6)
   find_package(Qt6 COMPONENTS Widgets WebEngineCore WebEngineWidgets)
   if(NOT Qt6_FOUND)
      message(FATAL_ERROR "Could NOT find Qt6 libs")
   endif()
   set(qt_libs Qt6::Core Qt6::Widgets Qt6::WebEngineCore Qt6::WebEngineWidgets)
else()
   find_package(Qt5 COMPONENTS Widgets WebEngine WebEngineWidgets)
   if(NOT Qt5_FOUND)
      message(FATAL_ERROR "Could NOT find Qt5 libs")
   endif()
   set(qt_libs Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
endif()


GO4_LINK_LIBRARY(Go4UserGui
                 SOURCES QUserPanel.cpp QUserPanel.ui
                 LIBRARIES ${qt_libs} ${ROOT_LIBRARIES} ${Go4Widgets_LIBRARY} ${Go4WebRoot_LIBRARY})
