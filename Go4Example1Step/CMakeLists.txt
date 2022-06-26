if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.9)
   project(Go4Example1Step)
   find_package(Go4)
   include(${GO4_USE_FILE})
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      XXXLinkDef.h
   HEADERS
      TXXXAnalysis.h
      TXXXControl.h
      TXXXEvent.h
      TXXXProc.h
   SOURCES
      TXXXAnalysis.cxx
      TXXXControl.cxx
      TXXXEvent.cxx
      TXXXProc.cxx
   COPY
      histofill.C
   LIBRARIES
      ROOT::Core
)