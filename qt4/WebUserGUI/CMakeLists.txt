cmake_minimum_required(VERSION 3.9)

project(WebUserGui)

find_package(Go4)

include(${GO4_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets WebEngine WebEngineWidgets)
if(NOT Qt5_FOUND)
  message(FATAL_ERROR "Could NOT find Qt5")
endif()

GO4_LINK_LIBRARY(Go4UserGui
                 SOURCES QUserPanel.cpp QUserPanel.ui
                 LIBRARIES
                     Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets
                     ${ROOT_LIBRARIES} ${Go4Widgets_LIBRARY} ${Go4WebRoot_LIBRARY} ROOT::Core)