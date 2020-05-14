set(Go4Http_headers
   TGo4MsgList.h
   TGo4Sniffer.h
)

set(Go4Http_sources
   TGo4MsgList.cxx
   TGo4Sniffer.cxx
)

GO4_INSTALL_HEADERS(${Go4Http_headers})

GO4_STANDARD_LIBRARY(Go4Http
                     LINKDEF LinkDef.h
                     HEADERS ${Go4Http_headers}
                     SOURCES ${Go4Http_sources}
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler Go4AnalysisBase Go4Analysis
                     LIBRARIES RHTTP RHTTPSniff
                     DEFINITIONS SNIFFER_FULL)
