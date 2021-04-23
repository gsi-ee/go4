set(Go4TaskHanlder_headers
   TGo4ClientStatus.h
   TGo4ClientTask.h
   TGo4CommandRunnable.h
   TGo4ConnectorRunnable.h
   TGo4DataRunnable.h
   TGo4LocalCommandRunnable.h
   TGo4Master.h
   TGo4ServerTask.h
   TGo4Slave.h
   TGo4StatusRunnable.h
   TGo4TaskConnectorTimer.h
   TGo4Task.h
   TGo4TaskHandlerAbortException.h
   TGo4TaskHandlerException.h
   TGo4TaskHandler.h
   TGo4TaskHandlerRunnable.h
   TGo4TaskHandlerStatus.h
   TGo4TaskManager.h
   TGo4TaskOwner.h
   TGo4TaskStatus.h
)

set(Go4TaskHanlder_sources
   TGo4ClientStatus.cxx
   TGo4ClientTask.cxx
   TGo4CommandRunnable.cxx
   TGo4ConnectorRunnable.cxx
   TGo4DataRunnable.cxx
   TGo4LocalCommandRunnable.cxx
   TGo4Master.cxx
   TGo4ServerTask.cxx
   TGo4Slave.cxx
   TGo4StatusRunnable.cxx
   TGo4TaskConnectorTimer.cxx
   TGo4Task.cxx
   TGo4TaskHandlerAbortException.cxx
   TGo4TaskHandler.cxx
   TGo4TaskHandlerException.cxx
   TGo4TaskHandlerRunnable.cxx
   TGo4TaskHandlerStatus.cxx
   TGo4TaskManager.cxx
   TGo4TaskOwner.cxx
   TGo4TaskStatus.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4TaskHanlder_headers} Go4EmergencyCommands.h)

GO4_SOURCES(Go4TaskHandler HEADERS ${Go4TaskHanlder_headers} SOURCES ${Go4TaskHanlder_sources})