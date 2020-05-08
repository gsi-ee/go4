set(Go4Exceptions_headers
   TGo4ControlException.h
   TGo4Exception.h
   TGo4LogicException.h
   TGo4RuntimeException.h
)

set(Go4Exceptions_sources
   TGo4ControlException.cxx
   TGo4Exception.cxx
   TGo4LogicException.cxx
   TGo4RuntimeException.cxx
)

GO4_INSTALL_HEADERS(${Go4Exceptions_headers} Go4Exceptions.h)

GO4_SOURCES(Go4Base HEADERS ${Go4Exceptions_headers} SOURCES ${Go4Exceptions_sources})