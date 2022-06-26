GO4_INSTALL_HEADERS(
   QWebCanvas.h
)

if(GO4_QTVERSION STREQUAL Qt6)
  set(qt_web_libs Qt6::Core Qt6::Widgets Qt6::WebEngineCore Qt6::WebEngineWidgets)
  set(root_qtweb_lib ROOT::ROOTQt6WebDisplay)
else()
  set(qt_web_libs Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
  set(root_qtweb_lib ROOT::ROOTQt5WebDisplay)
endif()

GO4_LINK_LIBRARY(Go4WebRoot
                 SOURCES QWebCanvas.cpp
                 DEFINITIONS -D${GO4_PLATFORM}
                 LIBRARIES
                    ${qt_web_libs}
                    ROOT::RHTTP ROOT::ROOTWebDisplay ROOT::WebGui6
                    ${root_qtweb_lib}
                    Go4Base)

target_include_directories(Go4WebRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(Go4WebRoot Go4Gui)

install(TARGETS Go4WebRoot EXPORT ${CMAKE_PROJECT_NAME}Exports
                           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries
                           LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
                           ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)