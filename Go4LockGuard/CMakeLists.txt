set(Go4LockGurad_headers
   TGo4LockGuard.h
)

set(Go4LockGurad_sources
   TGo4LockGuard.cxx
)

GO4_INSTALL_HEADERS(${Go4LockGurad_headers})

GO4_SOURCES(Go4Base HEADERS ${Go4LockGurad_headers} SOURCES ${Go4LockGurad_sources})