set(Go4ThreadManager_headers
   TGo4AppControlTimer.h
   TGo4CancelException.h
   TGo4CreateException.h
   TGo4RemoveException.h
   TGo4ReplaceException.h
   TGo4RestartException.h
   TGo4Runnable.h
   TGo4StartException.h
   TGo4TerminateException.h
   TGo4ThreadException.h
   TGo4Thread.h
   TGo4ThreadHandler.h
   TGo4ThreadManager.h
)

set(Go4ThreadManager_sources
   TGo4AppControlTimer.cxx
   TGo4CancelException.cxx
   TGo4CreateException.cxx
   TGo4RemoveException.cxx
   TGo4ReplaceException.cxx
   TGo4RestartException.cxx
   TGo4Runnable.cxx
   TGo4StartException.cxx
   TGo4TerminateException.cxx
   TGo4Thread.cxx
   TGo4ThreadException.cxx
   TGo4ThreadHandler.cxx
   TGo4ThreadManager.cxx
)

GO4_INSTALL_HEADERS(${Go4ThreadManager_headers} Go4ThreadManagerExceptions.h)

GO4_STANDARD_LIBRARY(Go4ThreadManager
                     LINKDEF Go4ThreadManagerLinkDef.h
                     HEADERS ${Go4ThreadManager_headers}
                     SOURCES ${Go4ThreadManager_sources}
                     DEPENDENCIES Go4Base)
