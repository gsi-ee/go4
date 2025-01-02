if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.10)
   project(Go4ExampleSimple)
   find_package(Go4)
   include(${GO4_USE_FILE})
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      XXXLinkDef.h
   HEADERS
      TXXXParam.h
      TXXXProc.h
   SOURCES
      TXXXProc.cxx
)
