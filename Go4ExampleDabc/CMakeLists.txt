if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.9)
   project(Go4ExampleDabc)
   find_package(Go4)
   include(${GO4_USE_FILE})
   find_package(DABC)
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      XXXLinkDef.h
   HEADERS
      TXXXProc.h
   SOURCES
      TXXXProc.cxx
   LIBRARIES
      ${DabcBase_LIBRARY}
   INCLUDE_DIRS
      ${DABC_INCLUDE_DIR}
)
