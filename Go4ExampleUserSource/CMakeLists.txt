if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.9)
   project(Go4ExampleUserSource)
   find_package(Go4)
   include(${GO4_USE_FILE})
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      YYYLinkDef.h
   HEADERS
      TYYYAnalysis.h
      TYYYEventSource.h
      TYYYParameter.h
      TYYYRawEvent.h
      TYYYUnpackEvent.h
      TYYYUnpackProc.h
   SOURCES
      TYYYAnalysis.cxx
      TYYYEventSource.cxx
      TYYYParameter.cxx
      TYYYRawEvent.cxx
      TYYYUnpackEvent.cxx
      TYYYUnpackProc.cxx
   COPY
      befoil50.scf
      tafoil50.scf
)
