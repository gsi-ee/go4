set(Go4AnalysisClient_headers
   TGo4AnalysisClient.h
   TGo4AnalysisClientImp.h
   TGo4AnalysisMainRunnable.h
   TGo4AnalysisWatchRunnable.h
)

set(Go4AnalysisClient_sources
  TGo4AnalysisClientImp.cxx
  TGo4AnalysisMainRunnable.cxx
  TGo4AnalysisWatchRunnable.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4AnalysisClient_headers} SOURCES ${Go4AnalysisClient_sources})

GO4_INSTALL_HEADERS(${Go4AnalysisClient_headers})
