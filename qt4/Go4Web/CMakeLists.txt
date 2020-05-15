GO4_INSTALL_HEADERS(
   QWebCanvas.h
)

GO4_LINK_LIBRARY(Go4WebRoot 
                 SOURCES QWebCanvas.cpp
                 DEFINITIONS -D${GO4_PLATFORM}
                 LIBRARIES 
                    Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets
                    ${ROOT_RHTTP_LIBRARY} ${ROOT_ROOTWebDisplay_LIBRARY} 
                    ${ROOT_ROOTQt5WebDisplay_LIBRARY} ${ROOT_WebGui6_LIBRARY} WebGui6Full
                    Go4Base)

target_include_directories(Go4WebRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(Go4WebRoot Go4Gui)