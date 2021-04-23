set(Go4StatusAnalysis_headers
   TGo4AnalysisClientStatus.h
   TGo4AnalysisObjectNames.h
   TGo4AnalysisObjectResult.h
   TGo4AnalysisStatus.h
   TGo4AnalysisStepStatus.h
   TGo4ExportManager.h
   TGo4FitterEnvelope.h
   TGo4HistogramStatus.h
   TGo4ObjEnvelope.h
   TGo4Ratemeter.h
   TGo4RollingGraph.h
   TGo4TreeStructure.h
)

set(Go4StatusAnalysis_sources
   TGo4AnalysisClientStatus.cxx
   TGo4AnalysisObjectNames.cxx
   TGo4AnalysisObjectResult.cxx
   TGo4AnalysisStatus.cxx
   TGo4AnalysisStepStatus.cxx
   TGo4ExportManager.cxx
   TGo4FitterEnvelope.cxx
   TGo4HistogramStatus.cxx
   TGo4ObjEnvelope.cxx
   TGo4Ratemeter.cxx
   TGo4RollingGraph.cxx
   TGo4TreeStructure.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4StatusAnalysis_headers})

GO4_SOURCES(Go4AnalBase HEADERS ${Go4StatusAnalysis_headers} SOURCES ${Go4StatusAnalysis_sources})
