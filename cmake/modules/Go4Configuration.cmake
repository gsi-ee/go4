set(LIBS_BASESET ${ROOT_LIBRARIES} XMLIO)

set(GO4_WITH_RFIO)

if(APPLE)
   set(GO4_DEFINITIONS "Darwin" CACHE STRING "Go4 platform definitions")
elseif(MSVC)
   set(GO4_DEFINITIONS "WIN32" CACHE STRING "Go4 platform definitions")
else()
   set(GO4_DEFINITIONS "Linux" CACHE STRING "Go4 platform definitions")
   set(GO4_WITH_RFIO ON)
endif()

set(GO4_CXX_FLAGS "" CACHE STRING "Go4 CXX flags")
set(GO4_C_FLAGS "" CACHE STRING "Go4 C flags")

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4Config.cmake.in
               ${CMAKE_BINARY_DIR}/Go4Config.cmake @ONLY NEWLINE_STYLE UNIX)

configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/Go4Macros.cmake
               ${CMAKE_BINARY_DIR}/Go4Macros.cmake COPYONLY)

set(GO4_ROOTDIR $ENV{ROOTSYS})

configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/Go4UseFile.cmake.in
               ${CMAKE_BINARY_DIR}/Go4UseFile.cmake @ONLY NEWLINE_STYLE UNIX)

