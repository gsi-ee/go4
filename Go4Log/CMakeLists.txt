set(Go4Log_headers
   TGo4Log.h
)

set(Go4Log_sources
   TGo4Log.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4Log_headers} go4iostream.h)

GO4_SOURCES(Go4Base HEADERS ${Go4Log_headers} SOURCES ${Go4Log_sources})