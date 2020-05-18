GO4_INSTALL_HEADERS(
   QRootCanvas.h
   QRootDialog.h
   QRootWindow.h
)
if(GO4_QTVERSION STREQUAL Qt4)
   set(qt_libs Qt4::QtCore Qt4::QtGui)
else()
   set(qt_libs Qt5::Core Qt5::Widgets)
endif()

GO4_LINK_LIBRARY(Go4QtRoot 
                 SOURCES QRootCanvas.cpp QRootDialog.cpp QRootWindow.cpp
                 LIBRARIES ${qt_libs} ${ROOT_Gui_LIBRARY} X11
                 DEFINITIONS -D${GO4_PLATFORM})

target_include_directories(Go4QtRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(Go4QtRoot Go4Gui)