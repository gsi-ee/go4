set(Go4Socket_headers
   TGo4Socket.h
   TGo4SocketSignalHandler.h
)

set(Go4Socket_sources
   TGo4Socket.cxx
   TGo4SocketSignalHandler.cxx
)

GO4_INSTALL_HEADERS(${Go4Socket_headers})

GO4_SOURCES(Go4TaskHandler HEADERS ${Go4Socket_headers} SOURCES ${Go4Socket_sources})