set(Go4HistogramServer_headers
   TGo4HisConnectorRunnable.h
   TGo4HistogramServer.h
   TGo4MbsHist.h
   TGo4ObjClient.h
   TGo4ObjConnectorRunnable.h
   TGo4ObjConnectorTimer.h
)

set(Go4HistogramServer_sources
   TGo4HisConnectorRunnable.cxx
   TGo4HistogramServer.cxx
   TGo4MbsHist.cxx
   TGo4ObjClient.cxx
   TGo4ObjConnectorRunnable.cxx
   TGo4ObjConnectorTimer.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4HistogramServer_headers} SOURCES ${Go4HistogramServer_sources})

GO4_INSTALL_HEADERS(${Go4HistogramServer_headers})

add_executable(MainGo4ObjectClient MainGo4ObjectClient.cxx)
target_link_libraries(MainGo4ObjectClient Go4Base Go4ThreadManager Go4TaskHandler Go4AnalysisBase Go4Analysis)
target_include_directories(MainGo4ObjectClient PRIVATE ${CMAKE_BINARY_DIR}/include)
