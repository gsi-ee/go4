GO4_INSTALL_HEADERS(
   QRootCanvas.h
   QRootDialog.h
   QRootWindow.h
)

if(GO4_QTVERSION STREQUAL Qt6)
   set(qt_libs Qt6::Core Qt6::Widgets)
else()
   set(qt_libs Qt5::Core Qt5::Widgets)
endif()

GO4_LINK_LIBRARY(Go4QtRoot
                 SOURCES QRootCanvas.cpp QRootDialog.cpp QRootWindow.cpp
                 LIBRARIES ${qt_libs} ${ROOT_Gui_LIBRARY} ${ROOT_Ged_LIBRARY} X11
                 DEFINITIONS -D${GO4_PLATFORM})

target_include_directories(Go4QtRoot PRIVATE ${CMAKE_BINARY_DIR}/include)

add_dependencies(Go4QtRoot Go4Gui)

install(TARGETS Go4QtRoot EXPORT ${CMAKE_PROJECT_NAME}Exports
                          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries
                          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
                          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
