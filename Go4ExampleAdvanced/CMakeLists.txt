if(NOT CMAKE_PROJECT_NAME STREQUAL Go4)
   cmake_minimum_required(VERSION 3.10)
   project(Go4ExampleAdvanced)
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
      TXXXCalibPar.h
      TXXXParameter.h
      TXXXUnpackEvent.h
      TXXXUnpackProc.h
   SOURCES
      TXXXAnalysis.cxx
      TXXXAnlEvent.cxx
      TXXXAnlProc.cxx
      TXXXCalibPar.cxx
      TXXXParameter.cxx
      TXXXUnpackEvent.cxx
      TXXXUnpackProc.cxx
   COPY
      calilines.txt
      browse.C
      convert.C
      convertfile.C
      findobjects.C
      plothistos.C
      setup.C
)
