set(Go4CommandsBase_headers
   TGo4Command.h
   TGo4CommandInvoker.h
   TGo4CommandProtoList.h
   TGo4CommandReceiver.h
   TGo4RemoteCommand.h
)

set(Go4CommandsBase_sources
   TGo4Command.cxx
   TGo4CommandInvoker.cxx
   TGo4CommandProtoList.cxx
   TGo4CommandReceiver.cxx
   TGo4RemoteCommand.cxx
)

GO4_INSTALL_HEADERS(${Go4CommandsBase_headers})

GO4_SOURCES(Go4Base HEADERS ${Go4CommandsBase_headers} SOURCES ${Go4CommandsBase_sources})