GO4_INSTALL_HEADERS(
   QRootCanvas.h
   QRootDialog.h
   QRootWindow.h
)

add_library(Go4QtRoot SHARED
   QRootCanvas.cpp
   QRootDialog.cpp
   QRootWindow.cpp
)

target_link_libraries(Go4QtRoot Qt5::Core Qt5::Widgets Gui X11)

target_include_directories(Go4QtRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

target_compile_definitions(Go4QtRoot PUBLIC -D${GO4_PLATFORM})

add_dependencies(Go4QtRoot Go4Gui)