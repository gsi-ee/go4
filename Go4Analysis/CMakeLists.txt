GO4_SOURCES(Go4AnalBase HEADERS TGo4Version.h SOURCES TGo4Version.cxx)

set(Go4Analysis_headers
   TGo4Analysis.h
   TGo4AnalysisImp.h
   TGo4AnalysisObjectManager.h
   TGo4AnalysisSniffer.h
   TGo4AnalysisStepException.h
   TGo4AnalysisStep.h
   TGo4AnalysisStepManager.h
   TGo4AnalysisWebStatus.h
   TGo4DynamicList.h
   TGo4UserException.h
)

set(Go4Analysis_sources
   TGo4AnalysisImp.cxx
   TGo4AnalysisObjectManager.cxx
   TGo4AnalysisStep.cxx
   TGo4AnalysisStepException.cxx
   TGo4AnalysisStepManager.cxx
   TGo4AnalysisWebStatus.cxx
   TGo4DynamicList.cxx
   TGo4UserException.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4Analysis_headers} SOURCES ${Go4Analysis_sources})

GO4_INSTALL_HEADERS(TGo4Version.h ${Go4Analysis_headers})
