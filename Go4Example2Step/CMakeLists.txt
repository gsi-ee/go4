if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.9)
   project(Go4Example2Step)
   find_package(Go4)
   include(${GO4_USE_FILE})
endif()

GO4_USER_ANALYSIS(
   LINKDEF
      XXXLinkDef.h
   HEADERS
      TXXXAnalysis.h
      TXXXAnlEvent.h
      TXXXAnlProc.h
      TXXXParameter.h
      TXXXUnpackEvent.h
      TXXXUnpackProc.h
   SOURCES
      TXXXAnalysis.cxx
      TXXXAnlEvent.cxx
      TXXXAnlProc.cxx
      TXXXParameter.cxx
      TXXXUnpackEvent.cxx
      TXXXUnpackProc.cxx
   COPY
      set_Par.C
)
