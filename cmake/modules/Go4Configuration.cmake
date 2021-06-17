set(GO4_VERSION 6.1.2)

if(APPLE)
   set(_platform Darwin)
elseif(MSVC)
   set(_platform WIN32)
else()
   set(GO4_WITH_RFIO 1)
   set(_platform Linux)
endif()

if(qt4)
   set(_qtvers Qt4)
elseif(qt6)
   set(_qtvers Qt6)
else()
   set(_qtvers Qt5)
endif()

set(GO4_PLATFORM "${_platform}" CACHE STRING "Go4 platform definitions")
set(GO4_DEFINITIONS "" CACHE STRING "Go4 extra definitions")
set(GO4_CXX_FLAGS "" CACHE STRING "Go4 CXX flags")
set(GO4_C_FLAGS "" CACHE STRING "Go4 C flags")
set(GO4_QTVERSION "${_qtvers}" CACHE STRING "Go4 qt version")

set(GO4_ROOT_DIR $ENV{ROOTSYS})
if(MSVC)
   string(REGEX REPLACE "\\\\" "/" GO4_ROOT_DIR ${GO4_ROOT_DIR})
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4Config.cmake.in
               ${CMAKE_BINARY_DIR}/Go4Config.cmake @ONLY NEWLINE_STYLE UNIX)

configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/Go4Macros.cmake
               ${CMAKE_BINARY_DIR}/Go4Macros.cmake COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4UseFile.cmake.in
               ${CMAKE_BINARY_DIR}/Go4UseFile.cmake @ONLY NEWLINE_STYLE UNIX)

install(FILES ${CMAKE_BINARY_DIR}/Go4Config.cmake
              ${CMAKE_BINARY_DIR}/Go4Macros.cmake
              ${CMAKE_BINARY_DIR}/Go4UseFile.cmake
              DESTINATION ${GO4_INSTALL_MAINDIR})

if(MSVC)

else()
endif()


# ================== Copy support files for plain Makefile ==========

if (MSVC)
   get_filename_component(_qt_dir_ "${Qt5_DIR}/../../.." ABSOLUTE)

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4login.bat.in
                  ${CMAKE_BINARY_DIR}/go4login.bat @ONLY NEWLINE_STYLE WIN32)
   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4.bat.in
                  ${CMAKE_BINARY_DIR}/go4.bat COPYONLY)

else()

   execute_process(COMMAND ${ROOT_BINDIR}/root-config --version OUTPUT_VARIABLE _root_vers_)
   execute_process(COMMAND ${ROOT_BINDIR}/root-config --exec-prefix OUTPUT_VARIABLE _root_exec_prefix_)
   execute_process(COMMAND ${ROOT_BINDIR}/root-config --cflags OUTPUT_VARIABLE _root_cflags_)
   execute_process(COMMAND ${ROOT_BINDIR}/root-config --libs OUTPUT_VARIABLE _root_libs_)

   set(_go4top_ ${CMAKE_BINARY_DIR})
   set(_go4bin_ ${CMAKE_BINARY_DIR}/bin)
   set(_go4lib_ ${CMAKE_BINARY_DIR}/lib)
   set(_go4inc_ ${CMAKE_BINARY_DIR}/include)

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4login.in
                  ${CMAKE_BINARY_DIR}/go4login @ONLY NEWLINE_STYLE UNIX)

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Makefile.gener.in
                  ${CMAKE_BINARY_DIR}/build/Makefile.gener @ONLY NEWLINE_STYLE UNIX)

   if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      set(_go4top_ ${GO4_INSTALL_MAINDIR})
   else()
      set(_go4top_ ${GO4_INSTALL_MAINDIR})
   endif()
   set(_go4bin_ ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
   set(_go4lib_ ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
   set(_go4inc_ ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4login.in
                  ${CMAKE_BINARY_DIR}/_install/go4login @ONLY NEWLINE_STYLE UNIX)

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Makefile.gener.in
                  ${CMAKE_BINARY_DIR}/_install/Makefile.gener @ONLY NEWLINE_STYLE UNIX)

   file(COPY ${CMAKE_SOURCE_DIR}/Makefile.config DESTINATION ${CMAKE_BINARY_DIR})
   file(COPY ${CMAKE_SOURCE_DIR}/Makefile.rules DESTINATION ${CMAKE_BINARY_DIR})
   file(COPY ${CMAKE_SOURCE_DIR}/build/cleanlib.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/cxx.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/depend.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/makedict.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/makelib6.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/makemap.sh DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/Makefile.Linux DESTINATION ${CMAKE_BINARY_DIR}/build)
   file(COPY ${CMAKE_SOURCE_DIR}/build/Makefile.Darwin DESTINATION ${CMAKE_BINARY_DIR}/build)

   install(FILES ${CMAKE_BINARY_DIR}/Makefile.config ${CMAKE_BINARY_DIR}/Makefile.rules DESTINATION ${GO4_INSTALL_MAINDIR})
   install(FILES ${CMAKE_BINARY_DIR}/_install/Makefile.gener DESTINATION ${GO4_INSTALL_BUILDDIR})
   install(DIRECTORY ${CMAKE_BINARY_DIR}/build/ DESTINATION ${GO4_INSTALL_BUILDDIR})
   install(DIRECTORY ${CMAKE_BINARY_DIR}/build/ DESTINATION ${GO4_INSTALL_BUILDDIR} FILES_MATCHING PATTERN "*.sh" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)  
   install(FILES ${CMAKE_BINARY_DIR}/_install/go4login DESTINATION ${GO4_INSTALL_MAINDIR})

endif()




# search for extra ROOT libraries
foreach(_cpt Gui XMLIO RHTTP RHTTPSniff Ged Spectrum ASImage Minuit ROOTWebDisplay ROOTQt5WebDisplay WebGui6)
  find_library(ROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${ROOT_LIBRARY_DIR})
  if(ROOT_${_cpt}_LIBRARY)
    mark_as_advanced(ROOT_${_cpt}_LIBRARY)
  endif()
endforeach()

