set(Go4CommandsTaskHandler_headers
   TGo4ComDisconnectSlave.h
   TGo4ComExecLine.h
   TGo4ComMasterQuit.h
   TGo4ComQuit.h
   TGo4ComRemoveClient.h
   TGo4ComServerQuit.h
   TGo4ComStart.h
   TGo4ComStop.h
   TGo4TaskHandlerCommand.h
   TGo4TaskHandlerCommandList.h
)

set(Go4CommandsTaskHandler_sources
   TGo4ComDisconnectSlave.cxx
   TGo4ComExecLine.cxx
   TGo4ComMasterQuit.cxx
   TGo4ComQuit.cxx
   TGo4ComRemoveClient.cxx
   TGo4ComServerQuit.cxx
   TGo4ComStart.cxx
   TGo4ComStop.cxx
   TGo4TaskHandlerCommand.cxx
   TGo4TaskHandlerCommandList.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4CommandsTaskHandler_headers})

GO4_SOURCES(Go4TaskHandler HEADERS ${Go4CommandsTaskHandler_headers} SOURCES ${Go4CommandsTaskHandler_sources})
