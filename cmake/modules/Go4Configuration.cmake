set(GO4_WITH_RFIO)

set(_platform Linux)
if(APPLE)
   set(_platform Darwin)
elseif(MSVC)
   set(_platform WIN32)
endif()

set(_qtvers Qt5)
if(qt4)
   set(_qtvers Qt4)
endif()
   

set(GO4_PLATFORM "${_platform}" CACHE STRING "Go4 platform definitions")
set(GO4_DEFINITIONS "" CACHE STRING "Go4 extra definitions")
set(GO4_CXX_FLAGS "" CACHE STRING "Go4 CXX flags")
set(GO4_C_FLAGS "" CACHE STRING "Go4 C flags")
set(GO4_QTVERSION "${_qtvers}" CACHE STRING "Go4 qt version")

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4Config.cmake.in
               ${CMAKE_BINARY_DIR}/Go4Config.cmake @ONLY NEWLINE_STYLE UNIX)

configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/Go4Macros.cmake
               ${CMAKE_BINARY_DIR}/Go4Macros.cmake COPYONLY)

set(GO4_ROOTDIR $ENV{ROOTSYS})

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4UseFile.cmake.in
               ${CMAKE_BINARY_DIR}/Go4UseFile.cmake @ONLY NEWLINE_STYLE UNIX)

# search for extra ROOT libraries
foreach(_cpt Gui XMLIO RHTTP RHTTPSniff Ged Spectrum ASImage Minuit ROOTWebDisplay ROOTQt5WebDisplay WebGui6)
  find_library(ROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${ROOT_LIBRARY_DIR})
  if(ROOT_${_cpt}_LIBRARY)
    mark_as_advanced(ROOT_${_cpt}_LIBRARY)
  endif()
endforeach()

