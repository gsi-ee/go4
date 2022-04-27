set(Go4CommandsAnalysis_headers
   TGo4AnalysisCommand.h
   TGo4AnalysisCommandList.h
   TGo4AnalysisObjectCommand.h
   TGo4ComAddTreeHistogram.h
   TGo4ComAutosave.h
   TGo4ComClearObject.h
   TGo4ComCloseAnalysis.h
   TGo4ComDeleteObject.h
   TGo4ComGetAnalysisStatus.h
   TGo4ComGetCurrentEvent.h
   TGo4ComGetEnvelope.h
   TGo4ComGetNamesList.h
   TGo4ComGetObjectStatus.h
   TGo4ComInitAnalysis.h
   TGo4ComLoadAnalysisStatus.h
   TGo4ComPrintConditions.h
   TGo4ComPrintDynList.h
   TGo4ComPrintHistograms.h
   TGo4ComSaveAnalysisStatus.h
   TGo4ComSetAnalysisStatus.h
   TGo4ComSetObject.h
   TGo4ComSetPrintEvent.h
   TGo4ComSetProtections.h
)

set(Go4CommandsAnalysis_sources
   TGo4AnalysisCommand.cxx
   TGo4AnalysisCommandList.cxx
   TGo4AnalysisObjectCommand.cxx
   TGo4ComAddTreeHistogram.cxx
   TGo4ComAutosave.cxx
   TGo4ComClearObject.cxx
   TGo4ComCloseAnalysis.cxx
   TGo4ComDeleteObject.cxx
   TGo4ComGetAnalysisStatus.cxx
   TGo4ComGetCurrentEvent.cxx
   TGo4ComGetEnvelope.cxx
   TGo4ComGetNamesList.cxx
   TGo4ComGetObjectStatus.cxx
   TGo4ComInitAnalysis.cxx
   TGo4ComLoadAnalysisStatus.cxx
   TGo4ComPrintConditions.cxx
   TGo4ComPrintDynList.cxx
   TGo4ComPrintHistograms.cxx
   TGo4ComSaveAnalysisStatus.cxx
   TGo4ComSetAnalysisStatus.cxx
   TGo4ComSetObject.cxx
   TGo4ComSetPrintEvent.cxx
   TGo4ComSetProtections.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4CommandsAnalysis_headers} SOURCES ${Go4CommandsAnalysis_sources})

GO4_INSTALL_HEADERS(MAKESUB ${Go4CommandsAnalysis_headers})
