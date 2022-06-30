if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.9)
   project(Go4ExampleUserStore)
   find_package(Go4)
   include(${GO4_USE_FILE})
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      XXXLinkDef.h
   HEADERS
      TXXXAnalysis.h
      TXXXEvent.h
      TXXXControl.h
      TXXXStore.h
      TXXXProc.h
   SOURCES
      TXXXAnalysis.cxx
      TXXXEvent.cxx
      TXXXControl.cxx
      TXXXStore.cxx
      TXXXProc.cxx
   LIBRARIES
      ROOT::Core
)