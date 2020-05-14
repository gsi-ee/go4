GO4_INSTALL_HEADERS(
   QWebCanvas.h
)

add_library(Go4WebRoot SHARED
   QWebCanvas.cpp
)

target_link_libraries(Go4WebRoot Qt5::Core Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets RHTTP ROOTWebDisplay ROOTQt5WebDisplay WebGui6 WebGui6Full)

target_include_directories(Go4WebRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

target_compile_definitions(Go4WebRoot PUBLIC -D${GO4_PLATFORM})

add_dependencies(Go4WebRoot Go4Gui)