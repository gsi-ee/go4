set(Go4DynamicList_headers
   TGo4DynamicEntry.h
   TGo4DynamicListException.h
   TGo4HistogramEntry.h
   TGo4TreeHistogramEntry.h
)

set(Go4DynamicList_sources
   TGo4DynamicEntry.cxx
   TGo4DynamicListException.cxx
   TGo4HistogramEntry.cxx
   TGo4TreeHistogramEntry.cxx
)

GO4_INSTALL_HEADERS(${Go4DynamicList_headers})

GO4_SOURCES(Go4AnalysisBase HEADERS ${Go4DynamicList_headers} SOURCES ${Go4DynamicList_sources})
