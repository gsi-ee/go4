GO4_INSTALL_HEADERS(
   QWebCanvas.h
)

if(ROOT_VERSION VERSION_LESS 6.23.00)
   set(GO4_WebGui6Full WebGui6Full)
endif()

GO4_LINK_LIBRARY(Go4WebRoot
                 SOURCES QWebCanvas.cpp
                 DEFINITIONS -D${GO4_PLATFORM}
                 LIBRARIES
                    Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets
                    ${ROOT_RHTTP_LIBRARY} ${ROOT_ROOTWebDisplay_LIBRARY}
                    ${ROOT_ROOTQt5WebDisplay_LIBRARY} ${ROOT_WebGui6_LIBRARY} ${GO4_WebGui6Full}
                    Go4Base)

target_include_directories(Go4WebRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(Go4WebRoot Go4Gui)

install(TARGETS Go4WebRoot EXPORT ${CMAKE_PROJECT_NAME}Exports
                           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries
                           LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
                           ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
