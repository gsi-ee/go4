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

if(GO4_QTVERSION STREQUAL Qt4)
   set(qt_libs Qt4::QtGui)
else()
   set(qt_libs Qt5::Widgets)
endif()

target_link_libraries(Go4QtRoot ${qt_libs} Gui X11)

target_include_directories(Go4QtRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

target_compile_definitions(Go4QtRoot PUBLIC -D${GO4_PLATFORM})

add_dependencies(Go4QtRoot Go4Gui)